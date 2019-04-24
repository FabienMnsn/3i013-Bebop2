/*
  Copyright (C) 2014 Parrot SA

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions
  are met:
  * Redistributions of source code must retain the above copyright
  notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright
  notice, this list of conditions and the following disclaimer in
  the documentation and/or other materials provided with the
  distribution.
  * Neither the name of Parrot nor the names
  of its contributors may be used to endorse or promote products
  derived from this software without specific prior written
  permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
  OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
  AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
  OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
  SUCH DAMAGE.
*/
/**
 * @file BebopSample.c
 * @brief This file contains sources about basic arsdk example sending commands to a bebop drone to pilot it,
 * receive its battery level and display the video stream.
 * @date 15/01/2015
 */

/*****************************************
 *
 *             include file :
 *
 *****************************************/
#include <stdio.h>
#include <stdlib.h>
#include <curses.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <libARSAL/ARSAL.h>
#include <libARController/ARController.h>
#include <libARDiscovery/ARDiscovery.h>
#include <libARCommands/ARCommands.h>
//#include <libARMavlink/libARMavlink.h>
#include <libARUtils/ARUtils.h>
#include <libARDataTransfer/ARDataTransfer.h>

#include "ARMAVLINK_FileGenerator.h"
#include "ARMAVLINK_Manager.h"
#include "ARUTILS_Manager.h"
#include "ARMAVLINK_ListUtils.h"
#include "ARMAVLINK_MissionItemUtils.h"

#include "BebopSample.h"

/*****************************************
 *
 *             define :
 *
 *****************************************/
#define TAG "BebopSample"

#define ERROR_STR_LENGTH 2048

#define BEBOP_IP_ADDRESS "192.168.42.1"
#define BEBOP_DISCOVERY_PORT 44444

#define DISPLAY_WITH_MPLAYER 1
#define KEYBOARD_INTERRUPT 1
#define MAVLINK_FILE_PATH "flightPlan.mavlink"
#define MAVLINK_FILE_NAME "flightPlan.mavlink"

#define FIFO_DIR_PATTERN "/tmp/arsdk_XXXXXX"
#define FIFO_NAME "arsdk_fifo.mp4"

/*****************************************
 *
 *             private header:
 *
 ****************************************/


/*****************************************
 *
 *             implementation :
 *
 *****************************************/


static char fifo_dir[] = FIFO_DIR_PATTERN;
static char fifo_name[128] = "";
int run = 1;
char gErrorStr[ERROR_STR_LENGTH];

FILE *videoOut = NULL;
int frameNb = 0;
ARSAL_Sem_t stateSem;
int isBebop2 = 0;
float progressArg = 0;
float completionArg = 0;
int autonomousFlightAvailable = 0;
int gpsFixed = 0;


eARCOMMANDS_ARDRONE3_PILOTINGSTATE_FLYINGSTATECHANGED_STATE getFlyingState(ARCONTROLLER_Device_t *deviceController)
{
    eARCOMMANDS_ARDRONE3_PILOTINGSTATE_FLYINGSTATECHANGED_STATE flyingState = ARCOMMANDS_ARDRONE3_PILOTINGSTATE_FLYINGSTATECHANGED_STATE_MAX;
    eARCONTROLLER_ERROR error;
    ARCONTROLLER_DICTIONARY_ELEMENT_t *elementDictionary = ARCONTROLLER_ARDrone3_GetCommandElements(deviceController->aRDrone3, ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_PILOTINGSTATE_FLYINGSTATECHANGED, &error);
    if (error == ARCONTROLLER_OK && elementDictionary != NULL)
    {
        ARCONTROLLER_DICTIONARY_ARG_t *arg = NULL;
        ARCONTROLLER_DICTIONARY_ELEMENT_t *element = NULL;
        HASH_FIND_STR (elementDictionary, ARCONTROLLER_DICTIONARY_SINGLE_KEY, element);
        if (element != NULL)
        {
            // Get the value
            HASH_FIND_STR(element->arguments, ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_PILOTINGSTATE_FLYINGSTATECHANGED_STATE, arg);
            if (arg != NULL)
            {
                // Enums are stored as I32
                flyingState = arg->value.I32;
            }
        }
    }
    return flyingState;
}

/*typedef void (*ARDATATRANSFER_Uploader_ProgressCallback_t) (void* arg, float percent);
typedef void (*ARDATATRANSFER_Uploader_CompletionCallback_t) (void* arg, float percent);*/

void ftpProgressCallBack(void *arg, float percent){//ça marche pas des masses....
	ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "PROGRESS %f %", percent);
	progressArg = percent;
}

void ftpCompletionCallBack(void *arg, float percent){
	ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "COMPLETION 100%");//appelé quand envoi ftp terminé, pas besoin de valeur autre que "finished" ici.
	completionArg = 100.00;
}


int main (int argc, char *argv[])
{


    // local declarations
    int failed = 0;
    ARDISCOVERY_Device_t *device = NULL;
    ARCONTROLLER_Device_t *deviceController = NULL;
    eARCONTROLLER_ERROR error = ARCONTROLLER_OK;
    eARCONTROLLER_DEVICE_STATE deviceState = ARCONTROLLER_DEVICE_STATE_MAX;
    pid_t child = 0;


    if (mkdtemp(fifo_dir) == NULL)
    {
        ARSAL_PRINT(ARSAL_PRINT_ERROR, "ERROR", "Mkdtemp failed.");
        return 1;
    }
    snprintf(fifo_name, sizeof(fifo_name), "%s/%s", fifo_dir, FIFO_NAME);

    if(mkfifo(fifo_name, 0666) < 0)
    {
        ARSAL_PRINT(ARSAL_PRINT_ERROR, "ERROR", "Mkfifo failed: %d, %s", errno, strerror(errno));
        return 1;
    }

    ARSAL_Sem_Init (&(stateSem), 0, 0);

  
    isBebop2 = 1;

    ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "-- Bebop 2 Sample --");

	//LA CREATION MAVLINK UTILISE DES FLOAT AVEC UNE PRECISION TRES MAUVAISE (TRONCAGE OU ARRONDI DANS LE SDK) DONC MAINTENANT C'EST LA CARTE QUI GENERE LE MAVLINK.
	//on créé le fichier mavlink ici, car pas besoin d'être connecté au drone pour ça :

	/*ARSAL_PRINT (ARSAL_PRINT_INFO, TAG, "Debut création mavlink");
	eARMAVLINK_ERROR merror;
	ARMAVLINK_FileGenerator_t *generator = ARMAVLINK_FileGenerator_New(&merror);//on créé le générateur mavlink

	float pitch = 0.0; // c'est quoi ça ? "pas"?


	// création des points de passage :
	FILE *lecteur = NULL;
	lecteur = fopen("out.txt","r");
	ARSAL_PRINT (ARSAL_PRINT_INFO, TAG, "ouverture fichier text");
	if(lecteur == NULL){
		ARSAL_PRINT (ARSAL_PRINT_ERROR, TAG, "ouverture fichier text échouée");
	}
	else{
	int nbMaxParams = 3;// a changer si on ajoute le yaw
	float params[nbMaxParams];
	float y = 0.0;//yaw des points de passage par défaut (vu qu'on le récupère pas)
	int cntParams = 0;
	int totalCount = 0;
	float buffer;
	while(fscanf(lecteur, "%f",&buffer)!=EOF){
		char floatPrint[64];
		sprintf(floatPrint, "%f", buffer);
		ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "%s",floatPrint);
		cntParams++;
		totalCount++;
		params[cntParams-1]=buffer;
		if(cntParams==3){
			if(totalCount==3){
				// 1) création d'un élément de décollage : 
				mavlink_mission_item_t item1;
				merror = ARMAVLINK_MissionItemUtils_CreateMavlinkTakeoffMissionItem(&item1, params[0], params[1], params[2], y, pitch);
				//on l'ajoute au générateur :
				merror= ARMAVLINK_FileGenerator_AddMissionItem(generator, &item1);
			}
			else{//2) création d'un élément de vol:
				mavlink_mission_item_t itemNav;
				merror = ARMAVLINK_MissionItemUtils_CreateMavlinkNavWaypointMissionItem(&itemNav, params[0], params[1], params[2], y);
				//on l'ajoute au générateur :
				merror= ARMAVLINK_FileGenerator_AddMissionItem(generator, &itemNav);
			}
			cntParams = 0;
		}
		
	}
	fclose(lecteur);

	// 3) création d'un élément d'attérissage:
	mavlink_mission_item_t item3;
	merror = ARMAVLINK_MissionItemUtils_CreateMavlinkLandMissionItem(&item3, params[0], params[1], params[2], y);
	//on l'ajoute au générateur :
	merror= ARMAVLINK_FileGenerator_AddMissionItem(generator, &item3);

	//on créé le fichier mavlink :
	ARSAL_PRINT (ARSAL_PRINT_INFO, TAG, "génération fichier mavlink");
	ARMAVLINK_FileGenerator_CreateMavlinkFile(generator, "outTest.mavlink");
	ARSAL_PRINT (ARSAL_PRINT_INFO, TAG, "fichier créé !");
	//on supprime le générateur maintenant pour libérer de l'espace mémoire:
	//ARMAVLINK_FileGenerator_Delete(&generator);
	}
	*/
	//

    if (!failed)
    {
       if (DISPLAY_WITH_MPLAYER)  //on n'ouvre plus la vidéo sur le PC
        {
	    if ((child = fork()) == 0) 
            {
		//execlp("xterm", "xterm", "-e", "mplayer", "-demuxer",  "h264es", fifo_name, "-benchmark", "-really-quiet", NULL);
//avec sortie copiée ailleurs:
               execlp("xterm", "xterm", "-e", "mplayer", "-demuxer",  "h264es", fifo_name, "-benchmark", "-really-quiet","-dumpstream","-dumpfile","/tmp/test.mp4", NULL);

//conversion mp4:
               ARSAL_PRINT(ARSAL_PRINT_ERROR, TAG, "Missing mplayer, you will not see the video. Please install mplayer and xterm.");
                return -1;
            }
        }

        if (DISPLAY_WITH_MPLAYER)
        {
	    ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "-- opening videoOut pointer --");
            videoOut = fopen(fifo_name, "w"); //pour écrire dedans par la suite ce qu'on reçoit du drone
        }
    }


    // create a discovery device
    if (!failed)
    {
        ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "- init discovey device ... ");
        eARDISCOVERY_ERROR errorDiscovery = ARDISCOVERY_OK;

        device = ARDISCOVERY_Device_New (&errorDiscovery);

        if (errorDiscovery == ARDISCOVERY_OK)
        {
            ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "    - ARDISCOVERY_Device_InitWifi ...");
            // create a Bebop drone discovery device (ARDISCOVERY_PRODUCT_ARDRONE)


       errorDiscovery = ARDISCOVERY_Device_InitWifi (device, ARDISCOVERY_PRODUCT_BEBOP_2, "bebop2", BEBOP_IP_ADDRESS, BEBOP_DISCOVERY_PORT);
	
            if (errorDiscovery != ARDISCOVERY_OK)
            {
                failed = 1;
                ARSAL_PRINT(ARSAL_PRINT_ERROR, TAG, "Discovery error :%s", ARDISCOVERY_Error_ToString(errorDiscovery));
            }
	    else{
		//on essaie d'envoyer le mavlink au drone en ftp maintenant qu'on y est connecté :
		//on créé le gestionnaire mavlink:
		//IHM_PrintHeader (ihm, "-- TEST 2 --");
	 	//IHM_PrintInfo(ihm, "PART MAVLINK SEND ...");
		ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "PART MAVLINK SEND ...");
		eARUTILS_ERROR utilError = ARUTILS_OK;
		eARDATATRANSFER_ERROR transferError = ARDATATRANSFER_OK;
		ARUTILS_Manager_t *utilsManager = ARUTILS_Manager_New(&utilError);
		ARDATATRANSFER_Manager_t *transfer = ARDATATRANSFER_Manager_New(&transferError);
		ARSAL_PRINT (ARSAL_PRINT_INFO, TAG, "transfer utils created");
		//on envoie le fichier mavlink au drone en ftp :
		ARUTILS_Manager_InitWifiFtp(utilsManager, BEBOP_IP_ADDRESS, 61, "","");
		ARSAL_PRINT (ARSAL_PRINT_INFO, TAG, "wifi transfer initialized");
		//ARUTILS_Ftp_ProgressCallback_t ftpCallback = FTPProgress;
		ARDATATRANSFER_Uploader_ProgressCallback_t ftpProgress = &ftpProgressCallBack;
		ARDATATRANSFER_Uploader_CompletionCallback_t ftpCompletion = &ftpCompletionCallBack;
		eARDATATRANSFER_UPLOADER_RESUME resume = ARDATATRANSFER_UPLOADER_RESUME_FALSE;
		ARDATATRANSFER_Uploader_New(transfer, utilsManager, MAVLINK_FILE_NAME, MAVLINK_FILE_PATH, ftpProgress, &progressArg, ftpCompletion, &completionArg, resume);
		ARDATATRANSFER_Uploader_ThreadRun(transfer);
		usleep(100000);//pour être sûr que l'envoi est terminé, vu qu'on a pas de callback...
		ARSAL_PRINT (ARSAL_PRINT_INFO, TAG, "file sended to drone");
		//IHM_PrintInfo(ihm, "OK FILE SENDED ...");
		if(utilError!=ARUTILS_OK || transferError != ARDATATRANSFER_OK){
			ARSAL_PRINT(ARSAL_PRINT_ERROR, TAG, "FAIL FILE SENDED !");
		}
		else{
			ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "OK FILE SENDED...");
		}
	    }
        }
        else
        {
            ARSAL_PRINT(ARSAL_PRINT_ERROR, TAG, "Discovery error :%s", ARDISCOVERY_Error_ToString(errorDiscovery));
            failed = 1;
        }
    }

    // create a device controller
    if (!failed)
    {
        deviceController = ARCONTROLLER_Device_New (device, &error);//on créé le device controller (nécessaire aussi pour le mavlink)

        if (error != ARCONTROLLER_OK)
        {
            ARSAL_PRINT (ARSAL_PRINT_ERROR, TAG, "Creation of deviceController failed.");
            failed = 1;
        }
       else 
        {
            ARSAL_PRINT (ARSAL_PRINT_ERROR, TAG, "Creation of deviceController OK.");
        }
    }

    if (!failed)
    {
        ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "- delete discovey device ... ");
        ARDISCOVERY_Device_Delete (&device);
    }



    // add the state change callback to be informed when the device controller starts, stops...
    if (!failed)
    {
        error = ARCONTROLLER_Device_AddStateChangedCallback (deviceController, stateChanged, deviceController);

        if (error != ARCONTROLLER_OK)
        {
            ARSAL_PRINT (ARSAL_PRINT_ERROR, TAG, "add State callback failed.");
            failed = 1;
        }
    }

    // add the command received callback to be informed when a command has been received from the device
    if (!failed)
    {
        error = ARCONTROLLER_Device_AddCommandReceivedCallback (deviceController, commandReceived, deviceController);

        if (error != ARCONTROLLER_OK)
        {
            ARSAL_PRINT (ARSAL_PRINT_ERROR, TAG, "add callback failed.");
            failed = 1;
        }
    }

    // add the frame received callback to be informed when a streaming frame has been received from the device
    if (!failed)
    {
        ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "- set Video callback ... ");
        error = ARCONTROLLER_Device_SetVideoStreamCallbacks (deviceController, decoderConfigCallback, didReceiveFrameCallback, NULL , NULL);

        if (error != ARCONTROLLER_OK)
        {
            failed = 1;
            ARSAL_PRINT(ARSAL_PRINT_ERROR, TAG, "- error: %s", ARCONTROLLER_Error_ToString(error));
        }
    }

    if (!failed)
    {
       // IHM_PrintInfo(ihm, "Connecting ...");
        ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "Connecting ...");
        error = ARCONTROLLER_Device_Start (deviceController);

        if (error != ARCONTROLLER_OK)
        {
            failed = 1;
            ARSAL_PRINT(ARSAL_PRINT_ERROR, TAG, "- error :%s", ARCONTROLLER_Error_ToString(error));
        }
    }

    if (!failed)
    {
        // wait state update update
        ARSAL_Sem_Wait (&(stateSem));

        deviceState = ARCONTROLLER_Device_GetState (deviceController, &error);

        if ((error != ARCONTROLLER_OK) || (deviceState != ARCONTROLLER_DEVICE_STATE_RUNNING))
        {
            failed = 1;
            ARSAL_PRINT(ARSAL_PRINT_ERROR, TAG, "- deviceState :%d", deviceState);
            ARSAL_PRINT(ARSAL_PRINT_ERROR, TAG, "- error :%s", ARCONTROLLER_Error_ToString(error));
        }
    }

    // send the command that tells to the Bebop to begin its streaming
    if (!failed)
    {
        ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "- send StreamingVideoEnable ... ");
        error = deviceController->aRDrone3->sendMediaStreamingVideoEnable (deviceController->aRDrone3, 1);
        if (error != ARCONTROLLER_OK)
        {
            ARSAL_PRINT(ARSAL_PRINT_ERROR, TAG, "- error :%s", ARCONTROLLER_Error_ToString(error));
            failed = 1;
        }
    }

//1) fix GPS
//ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "calibrating gps...");
//deviceController->common->sendCalibrationMagnetoCalibration(deviceController->common, 1);//apparemment ça le calibre pas ça
//2) set outdoor mode
  error = deviceController->common->sendWifiSettingsOutdoorSetting(deviceController->common, (uint8_t)1);//1 pour outoor, 0 pour indoor
if(error!=ARCONTROLLER_OK){
ARSAL_PRINT(ARSAL_PRINT_ERROR, TAG, "error setting outdoor mode");
}
//3) launch mavlink:
   //on exécute le fichier:
eARCONTROLLER_ERROR mavlinkError = ARCONTROLLER_OK;
ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "sending mavlink start...");
   eARCOMMANDS_COMMON_MAVLINK_START_TYPE mavlinkType = ARCOMMANDS_COMMON_MAVLINK_START_TYPE_FLIGHTPLAN;
	//enum type { flightPlan, MapMyHouse};
	//enum type mavlinkType = flightPlan;
ARSAL_PRINT(ARSAL_PRINT_ERROR, TAG, "MavlinkType : %d",mavlinkType);
//flat trim :
//deviceController->aRDrone3->sendPilotingFlatTrim(deviceController->aRDrone3);

eARCOMMANDS_ARDRONE3_PILOTINGSTATE_FLYINGSTATECHANGED_STATE flyingState = getFlyingState(deviceController);
//waiting flying state to be available :
while (flyingState != ARCOMMANDS_ARDRONE3_PILOTINGSTATE_FLYINGSTATECHANGED_STATE_LANDED){
	ARSAL_PRINT(ARSAL_PRINT_ERROR, TAG, "DRONE NOT LANDED !");
	error = deviceController->aRDrone3->sendPilotingLanding(deviceController->aRDrone3);
	flyingState = getFlyingState(deviceController);
}
ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "Drone landed -> OK");
 
while(completionArg<100){
	ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "transfer not completed...");
}

//waiting autonomous flight to be available :
ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "waiting for autonomous flight to be available :");
while(autonomousFlightAvailable==0){
	if(gpsFixed==0){
		ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "waiting gps to be fixed...");
	}
	else{
		ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "gpsFixed -> OK");
		ARSAL_PRINT(ARSAL_PRINT_ERROR, TAG, "another error occured for autonomous flight");
	}
	sleep(1);
}

ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "autonomous flight available -> OK");

//attente du décollage
/*FILE* fichierStart = NULL;
fichierStart = fopen("/var/www/public/iPod_Ping/start.txt", "r");
if(fichierStart!=NULL){
	int caractereActuel = 0;
	 do
	{
	    rewind(fichierStart);
	    caractereActuel = fgetc(fichierStart); // On lit le caractère
	} while (caractereActuel!=115);//tant qu'on ne lit pas 's'
	fclose(fichierStart);
}*/

//attente de la demande de démarage  :
FILE* fichierStart = NULL;

int caractereActuel = 0;
 do
{
    fichierStart = fopen("/var/www/public/iPod_Ping/start.txt", "r");
    if(fichierStart!=NULL){
	    rewind(fichierStart);
	    caractereActuel = fgetc(fichierStart); // On lit le caractère
	    fclose(fichierStart);
	    //ARSAL_PRINT(ARSAL_PRINT_INFO, TAG,"char : %d", caractereActuel);
    }
    else{
	ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "NULL");

    }
} while (caractereActuel!=115);//tant qu'on ne lit pas 's'

ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "STARTING");

//fin attente de démarage

	//lancement du mavlink : 
   mavlinkError = deviceController->common->sendMavlinkStart(deviceController->common,"flightPlan.mavlink", mavlinkType);


if(mavlinkError!=ARCONTROLLER_OK){
ARSAL_PRINT(ARSAL_PRINT_ERROR, TAG, "MAVLINK RUN ORDER FAILED");
}
else{
ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "MAVLINK RUN ORDER SEND");
}

    if (!failed) 
    {
	//error = deviceController->aRDrone3->sendPilotingTakeOff(deviceController->aRDrone3);
	//char c = "";
	//while (run && c!="q")
        //{
	// interrupt autonomous flight on keyboard enter


	//attente arrêt d'urgence
	if(KEYBOARD_INTERRUPT){
		char c = getchar();
   		ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "KEYBOARD INTERRUPT");
	}else{
		//attente d'une requête d'interruption
		FILE* fichierStop = NULL;
		int caractereActuelstop = 0;
		 do
		{
		    fichierStop = fopen("/var/www/public/iPod_Ping/stop.txt", "r");
		    if(fichierStop!=NULL){
			    rewind(fichierStop);
			    caractereActuelstop = fgetc(fichierStop); // On lit le caractère
			    fclose(fichierStop);
			    //ARSAL_PRINT(ARSAL_PRINT_INFO, TAG,"char : %d", caractereActuelstop);
		    }
		    else{
			ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "NULL");

		    }
		} while (caractereActuelstop!=115);//tant qu'on ne lit pas 's'
	}
	
	//char c = getchar();
	//ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "KEYBOARD INTERRUPT");
    }

    // we are here because of a disconnection or user has quit IHM, so safely delete everything
    if (deviceController != NULL)
    {


        deviceState = ARCONTROLLER_Device_GetState (deviceController, &error);
        if ((error == ARCONTROLLER_OK) && (deviceState != ARCONTROLLER_DEVICE_STATE_STOPPED))
        {
	    deviceController->common->sendMavlinkStop(deviceController->common);

	     error = deviceController->aRDrone3->sendPilotingLanding(deviceController->aRDrone3);
            //IHM_PrintInfo(ihm, "Disconnecting ...");
            ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "Disconnecting ...");

            error = ARCONTROLLER_Device_Stop (deviceController);

            if (error == ARCONTROLLER_OK)
            {
                // wait state update update
                ARSAL_Sem_Wait (&(stateSem));
            }
        }

        //IHM_PrintInfo(ihm, "");
        ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "ARCONTROLLER_Device_Delete ...");
        ARCONTROLLER_Device_Delete (&deviceController);

        if (DISPLAY_WITH_MPLAYER)
        {
            fflush (videoOut);
            fclose (videoOut);

            if (child > 0)
            {
                kill(child, SIGKILL);
            }
        }
    }

    ARSAL_Sem_Destroy (&(stateSem));

    unlink(fifo_name);
    rmdir(fifo_dir);

    ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "-- END --");

    return EXIT_SUCCESS;
}

/*****************************************
 *
 *             private implementation:
 *
 ****************************************/

// called when the state of the device controller has changed
void stateChanged (eARCONTROLLER_DEVICE_STATE newState, eARCONTROLLER_ERROR error, void *customData)
{
    ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "    - stateChanged newState: %d .....", newState);

    switch (newState)
    {
    case ARCONTROLLER_DEVICE_STATE_STOPPED:
        ARSAL_Sem_Post (&(stateSem));
        //stop
        //gIHMRun = 0;
	run = 0;

        break;

    case ARCONTROLLER_DEVICE_STATE_RUNNING:
        ARSAL_Sem_Post (&(stateSem));
        break;

    default:
        break;
    }
}

static void cmdBatteryStateChangedRcv(ARCONTROLLER_Device_t *deviceController, ARCONTROLLER_DICTIONARY_ELEMENT_t *elementDictionary)
{
    ARCONTROLLER_DICTIONARY_ARG_t *arg = NULL;
    ARCONTROLLER_DICTIONARY_ELEMENT_t *singleElement = NULL;

    if (elementDictionary == NULL) {
        ARSAL_PRINT(ARSAL_PRINT_ERROR, TAG, "elements is NULL");
        return;
    }

    // get the command received in the device controller
    HASH_FIND_STR (elementDictionary, ARCONTROLLER_DICTIONARY_SINGLE_KEY, singleElement);

    if (singleElement == NULL) {
        ARSAL_PRINT(ARSAL_PRINT_ERROR, TAG, "singleElement is NULL");
        return;
    }

    // get the value
    HASH_FIND_STR (singleElement->arguments, ARCONTROLLER_DICTIONARY_KEY_COMMON_COMMONSTATE_BATTERYSTATECHANGED_PERCENT, arg);

    if (arg == NULL) {
        ARSAL_PRINT(ARSAL_PRINT_ERROR, TAG, "arg is NULL");
        return;
    }

    // update UI
    batteryStateChanged(arg->value.U8);
}

static void cmdSensorStateListChangedRcv(ARCONTROLLER_Device_t *deviceController, ARCONTROLLER_DICTIONARY_ELEMENT_t *elementDictionary)
{
    ARCONTROLLER_DICTIONARY_ARG_t *arg = NULL;
    ARCONTROLLER_DICTIONARY_ELEMENT_t *dictElement = NULL;
    ARCONTROLLER_DICTIONARY_ELEMENT_t *dictTmp = NULL;

    eARCOMMANDS_COMMON_COMMONSTATE_SENSORSSTATESLISTCHANGED_SENSORNAME sensorName = ARCOMMANDS_COMMON_COMMONSTATE_SENSORSSTATESLISTCHANGED_SENSORNAME_MAX;
    int sensorState = 0;

    if (elementDictionary == NULL) {
        ARSAL_PRINT(ARSAL_PRINT_ERROR, TAG, "elements is NULL");
        return;
    }

    // get the command received in the device controller
    HASH_ITER(hh, elementDictionary, dictElement, dictTmp) {
        // get the Name
        HASH_FIND_STR (dictElement->arguments, ARCONTROLLER_DICTIONARY_KEY_COMMON_COMMONSTATE_SENSORSSTATESLISTCHANGED_SENSORNAME, arg);
        if (arg != NULL) {
	    sensorName = arg->value.I32;
        }
        // get the state
        HASH_FIND_STR (dictElement->arguments, ARCONTROLLER_DICTIONARY_KEY_COMMON_COMMONSTATE_SENSORSSTATESLISTCHANGED_SENSORSTATE, arg);
        if (arg != NULL) {
            sensorState = arg->value.U8;
            ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "sensorName %d ; sensorState: %d", sensorName, sensorState);
		//GPS = 3, magnetometer = 4. State : 1=OK, 0=KO
        }    
    }
}

// called when a command has been received from the drone
void commandReceived (eARCONTROLLER_DICTIONARY_KEY commandKey, ARCONTROLLER_DICTIONARY_ELEMENT_t *elementDictionary, void *customData)
{
    ARCONTROLLER_Device_t *deviceController = customData;
    ARCONTROLLER_DICTIONARY_ARG_t *arg = NULL;
    ARCONTROLLER_DICTIONARY_ELEMENT_t *element = NULL;
    ARCONTROLLER_DICTIONARY_ELEMENT_t *dictElement = NULL;
    ARCONTROLLER_DICTIONARY_ELEMENT_t *dictTmp = NULL;
    int component;

    if (deviceController == NULL)
        return;

    // if the command received is a battery state changed
    switch(commandKey) {
    case ARCONTROLLER_DICTIONARY_KEY_COMMON_COMMONSTATE_BATTERYSTATECHANGED:
        cmdBatteryStateChangedRcv(deviceController, elementDictionary);
        break;
    case ARCONTROLLER_DICTIONARY_KEY_COMMON_COMMONSTATE_SENSORSSTATESLISTCHANGED:
        cmdSensorStateListChangedRcv(deviceController, elementDictionary);
        break;
    case ARCONTROLLER_DICTIONARY_KEY_COMMON_FLIGHTPLANEVENT_STARTINGERROREVENT :
	ARSAL_PRINT(ARSAL_PRINT_ERROR, TAG, "Mavlink start error");
	break;
    case ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_GPSSETTINGSSTATE_GPSFIXSTATECHANGED :
        HASH_FIND_STR (elementDictionary, ARCONTROLLER_DICTIONARY_SINGLE_KEY, element);
        if (element != NULL)
        {
            HASH_FIND_STR (element->arguments, ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_GPSSETTINGSSTATE_GPSFIXSTATECHANGED_FIXED, arg);
            if (arg != NULL)
            {
                uint8_t fixed = arg->value.U8;
		if(fixed==1){
			ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "GPS SUCCESSFULLY FIXED");
		}
            }
        }
	break;
    case  ARCONTROLLER_DICTIONARY_KEY_COMMON_FLIGHTPLANSTATE_AVAILABILITYSTATECHANGED :
	 HASH_FIND_STR (elementDictionary, ARCONTROLLER_DICTIONARY_SINGLE_KEY, element);
	if (element != NULL)
	{
	    HASH_FIND_STR (element->arguments, ARCONTROLLER_DICTIONARY_KEY_COMMON_FLIGHTPLANSTATE_AVAILABILITYSTATECHANGED_AVAILABILITYSTATE, arg);
	    if (arg != NULL)
	    {
	        uint8_t AvailabilityState = arg->value.U8;
		ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "Mavlink availability state: %d", AvailabilityState);
		autonomousFlightAvailable = AvailabilityState;
	    }
	}
    	break;
    case ARCONTROLLER_DICTIONARY_KEY_COMMON_FLIGHTPLANSTATE_COMPONENTSTATELISTCHANGED :
        HASH_ITER(hh, elementDictionary, dictElement, dictTmp)
        {
            HASH_FIND_STR (dictElement->arguments, ARCONTROLLER_DICTIONARY_KEY_COMMON_FLIGHTPLANSTATE_COMPONENTSTATELISTCHANGED_COMPONENT, arg);
            if (arg != NULL)
            {
               component = arg->value.I32;//0: GPS, 1: calibration, 2: mavlink file, 3: take off
            }
            HASH_FIND_STR (dictElement->arguments, ARCONTROLLER_DICTIONARY_KEY_COMMON_FLIGHTPLANSTATE_COMPONENTSTATELISTCHANGED_STATE, arg);
            if (arg != NULL)
            {
                uint8_t State = arg->value.U8;
		ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "Mavlink component %d state: %d",component, State);//1: OK, 0: KO
		if(component == 0){
			gpsFixed = State;//on met à jour l'état du GPS
		}
            }
        }
	break;
    case ARCONTROLLER_DICTIONARY_KEY_COMMON_MAVLINKSTATE_MAVLINKFILEPLAYINGSTATECHANGED:
	HASH_FIND_STR(elementDictionary, ARCONTROLLER_DICTIONARY_SINGLE_KEY, element);
	if(element!=NULL){
		HASH_FIND_STR(element->arguments, ARCONTROLLER_DICTIONARY_KEY_COMMON_MAVLINKSTATE_MAVLINKFILEPLAYINGSTATECHANGED_STATE, arg);
		if(arg !=NULL){
			eARCOMMANDS_COMMON_MAVLINKSTATE_MAVLINKFILEPLAYINGSTATECHANGED_STATE state = arg->value.I32;
			ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "MAVLINK STATE : %d", state);//0 : playing, 1: stopped, 2:paused, 3:loaded
		}
		
		HASH_FIND_STR(element->arguments, ARCONTROLLER_DICTIONARY_KEY_COMMON_MAVLINKSTATE_MAVLINKFILEPLAYINGSTATECHANGED_FILEPATH, arg);
		if(arg !=NULL){
			char *filepathmav = arg->value.String;
			ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "MAVLINK FILEPATH : %s", filepathmav);
		}
		
		HASH_FIND_STR(element->arguments, ARCONTROLLER_DICTIONARY_KEY_COMMON_MAVLINKSTATE_MAVLINKFILEPLAYINGSTATECHANGED_TYPE, arg);
		if(arg !=NULL){
			eARCOMMANDS_COMMON_MAVLINKSTATE_MAVLINKFILEPLAYINGSTATECHANGED_TYPE type = arg->value.I32;
			ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "MAVLINK TYPE : %d", type);
		}
	}
	break;
    case ARCONTROLLER_DICTIONARY_KEY_COMMON_MAVLINKSTATE_MAVLINKPLAYERRORSTATECHANGED:
	ARSAL_PRINT(ARSAL_PRINT_ERROR, TAG, "autonomous flight state error");
	HASH_FIND_STR(elementDictionary, ARCONTROLLER_DICTIONARY_SINGLE_KEY, element);
	if(element!=NULL){
		HASH_FIND_STR(element->arguments, ARCONTROLLER_DICTIONARY_KEY_COMMON_MAVLINKSTATE_MAVLINKPLAYERRORSTATECHANGED_ERROR, arg);
		if(arg !=NULL){
			eARCOMMANDS_COMMON_MAVLINKSTATE_MAVLINKPLAYERRORSTATECHANGED_ERROR mavError = arg->value.I32;
			if (mavError == ARCOMMANDS_COMMON_MAVLINKSTATE_MAVLINKPLAYERRORSTATECHANGED_ERROR_NOTINOUTDOORMODE){
				ARSAL_PRINT(ARSAL_PRINT_ERROR, TAG, "NOT IN OUTDOOR MODE ERROR");
			}
			if (mavError == ARCOMMANDS_COMMON_MAVLINKSTATE_MAVLINKPLAYERRORSTATECHANGED_ERROR_GPSNOTFIXED){
				ARSAL_PRINT(ARSAL_PRINT_ERROR, TAG, "GPS NOT FIXED ERROR");
			}
			if (mavError == ARCOMMANDS_COMMON_MAVLINKSTATE_MAVLINKPLAYERRORSTATECHANGED_ERROR_NOTCALIBRATED){
				ARSAL_PRINT(ARSAL_PRINT_ERROR, TAG, "DRONE NOT CALIBRATED ERROR");
			}
			if (mavError == ARCOMMANDS_COMMON_MAVLINKSTATE_MAVLINKPLAYERRORSTATECHANGED_ERROR_NONE){
				ARSAL_PRINT(ARSAL_PRINT_ERROR, TAG, "NO ERROR");
			}
		}
	}
        break;
    default:
	break;
    }
}

void batteryStateChanged (uint8_t percent)
{
    // callback of changing of battery level

   // if (ihm != NULL)
   // {
        //IHM_PrintBattery (ihm, percent);
	ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "BATTERY %d %", percent);
   // }
}

eARCONTROLLER_ERROR decoderConfigCallback (ARCONTROLLER_Stream_Codec_t codec, void *customData)
{
    if (videoOut != NULL)
    {
        if (codec.type == ARCONTROLLER_STREAM_CODEC_TYPE_H264)
        {
            if (DISPLAY_WITH_MPLAYER)
            {
                fwrite(codec.parameters.h264parameters.spsBuffer, codec.parameters.h264parameters.spsSize, 1, videoOut);
                fwrite(codec.parameters.h264parameters.ppsBuffer, codec.parameters.h264parameters.ppsSize, 1, videoOut);

                fflush (videoOut);
            }
        }

    }
    else
    {
        //ARSAL_PRINT(ARSAL_PRINT_WARNING, TAG, "videoOut is NULL.");
    }

    return ARCONTROLLER_OK;
}


eARCONTROLLER_ERROR didReceiveFrameCallback (ARCONTROLLER_Frame_t *frame, void *customData)
{
    if (videoOut != NULL)
    {
        if (frame != NULL)
        {
            if (DISPLAY_WITH_MPLAYER)
            {
		//ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "Data Frame %d", frame->data);
		
                fwrite(frame->data, frame->used, 1, videoOut);

                fflush (videoOut);
            }
        }
        else
        {
          //  ARSAL_PRINT(ARSAL_PRINT_WARNING, TAG, "frame is NULL.");
        }
    }
    else
    {
      //  ARSAL_PRINT(ARSAL_PRINT_WARNING, TAG, "videoOut is NULL.");
    }

    return ARCONTROLLER_OK;
}



// callbacks:

void onInputEvent (eIHM_INPUT_EVENT event, void *customData)
{
    // Manage IHM input events
    ARCONTROLLER_Device_t *deviceController = (ARCONTROLLER_Device_t *)customData;
    eARCONTROLLER_ERROR error = ARCONTROLLER_OK;

    switch (event)
    {
    case IHM_INPUT_EVENT_EXIT:
	run = 0;
        break;
    case IHM_INPUT_EVENT_EMERGENCY:
        if(deviceController != NULL)
        {
            // send a Emergency command to the drone
            error = deviceController->aRDrone3->sendPilotingEmergency(deviceController->aRDrone3);
        }
        break;
   /* case IHM_INPUT_EVENT_LAND: //on retire le pilotage manuel, on ne garde que l'arrêt d'urgence et la sortie du programme	
        if(deviceController != NULL)
        {
            // send a takeoff command to the drone
            error = deviceController->aRDrone3->sendPilotingLanding(deviceController->aRDrone3);
        }
        break;
    case IHM_INPUT_EVENT_TAKEOFF:
        if(deviceController != NULL)
        {
            // send a landing command to the drone
            error = deviceController->aRDrone3->sendPilotingTakeOff(deviceController->aRDrone3);
        }
        break;
    case IHM_INPUT_EVENT_UP:
        if(deviceController != NULL)
        {
            // set the flag and speed value of the piloting command
            error = deviceController->aRDrone3->setPilotingPCMDGaz(deviceController->aRDrone3, 50);
        }
        break;
    case IHM_INPUT_EVENT_DOWN:
        if(deviceController != NULL)
        {
            error = deviceController->aRDrone3->setPilotingPCMDGaz(deviceController->aRDrone3, -50);
        }
        break;
    case IHM_INPUT_EVENT_RIGHT:
        if(deviceController != NULL)
        {
            error = deviceController->aRDrone3->setPilotingPCMDYaw(deviceController->aRDrone3, 50);
        }
        break;
    case IHM_INPUT_EVENT_LEFT:
        if(deviceController != NULL)
        {
            error = deviceController->aRDrone3->setPilotingPCMDYaw(deviceController->aRDrone3, -50);
        }
        break;
    case IHM_INPUT_EVENT_FORWARD:
        if(deviceController != NULL)
        {
            error = deviceController->aRDrone3->setPilotingPCMDPitch(deviceController->aRDrone3, 50);
            error = deviceController->aRDrone3->setPilotingPCMDFlag(deviceController->aRDrone3, 1);
        }
        break;
    case IHM_INPUT_EVENT_BACK:
        if(deviceController != NULL)
        {
            error = deviceController->aRDrone3->setPilotingPCMDPitch(deviceController->aRDrone3, -50);
            error = deviceController->aRDrone3->setPilotingPCMDFlag(deviceController->aRDrone3, 1);
        }
        break;
    case IHM_INPUT_EVENT_ROLL_LEFT:
        if(deviceController != NULL)
        {
            error = deviceController->aRDrone3->setPilotingPCMDRoll(deviceController->aRDrone3, -50);
            error = deviceController->aRDrone3->setPilotingPCMDFlag(deviceController->aRDrone3, 1);
        }
        break;
    case IHM_INPUT_EVENT_ROLL_RIGHT:
        if(deviceController != NULL)
        {
            error = deviceController->aRDrone3->setPilotingPCMDRoll(deviceController->aRDrone3, 50);
            error = deviceController->aRDrone3->setPilotingPCMDFlag(deviceController->aRDrone3, 1);
        }
        break;
    case IHM_INPUT_EVENT_NONE:
        if(deviceController != NULL)
        {
            error = deviceController->aRDrone3->setPilotingPCMD(deviceController->aRDrone3, 0, 0, 0, 0, 0, 0);
        }
        break;*/
    default:
        break;
    }

    // This should be improved, here it just displays that one error occured
    if (error != ARCONTROLLER_OK)
    {
        //IHM_PrintInfo(ihm, "Error sending an event");
    }
}

int customPrintCallback (eARSAL_PRINT_LEVEL level, const char *tag, const char *format, va_list va)
{
    // Custom callback used when ncurses is runing for not disturb the IHM

    if ((level == ARSAL_PRINT_ERROR) && (strcmp(TAG, tag) == 0))
    {
        // Save the last Error
        vsnprintf(gErrorStr, (ERROR_STR_LENGTH - 1), format, va);
        gErrorStr[ERROR_STR_LENGTH - 1] = '\0';
    }

    return 1;
}
