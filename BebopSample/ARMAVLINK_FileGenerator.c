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
 * @file ARMAVLINK_FileGenerator.c
 * @brief Mavlink File Generator
 * @date 14/05/2014
 * @author djavan.bertrand@parrot.com
 */
#include <stdlib.h>
#include <stdio.h>
#include <libARSAL/ARSAL_Print.h>
#include "ARMAVLINK_FileGenerator.h"
#include "ARMAVLINK_MissionItemUtils.h"

/* ***************************************
 *
 *             define :
 *
 *****************************************/
#define ARMAVLINK_FILE_GENERATOR_TAG                                "ARMAVLINK_FileGenerator"

#define ARMAVLINK_FILE_GENERATOR_FILE_HEADER                        "QGC WPL 120"

/* ***************************************
 *
 *             functions :
 *
 *****************************************/

ARMAVLINK_FileGenerator_t* ARMAVLINK_FileGenerator_New(eARMAVLINK_ERROR *error)
{
    ARMAVLINK_FileGenerator_t *fileGenerator = NULL;
    eARMAVLINK_ERROR err = ARMAVLINK_OK;
    
    /* Check parameters */
    if(err == ARMAVLINK_OK)
    {
        /* Create the file generator */
        fileGenerator = malloc (sizeof (ARMAVLINK_FileGenerator_t));
        if (fileGenerator == NULL)
        {
            err = ARMAVLINK_ERROR_ALLOC;
        }
    }
    
    if(err == ARMAVLINK_OK)
    {
        fileGenerator->missionItemList = ARMAVLINK_ListUtils_MissionItemListNew();
        if (fileGenerator->missionItemList == NULL)
        {
            err = ARMAVLINK_ERROR_ALLOC;
        }
    }
    
    /* delete the file generator if an error occurred */
    if (err != ARMAVLINK_OK)
    {
        ARSAL_PRINT (ARSAL_PRINT_ERROR, ARMAVLINK_FILE_GENERATOR_TAG, "error: %s", ARMAVLINK_Error_ToString (err));
        ARMAVLINK_FileGenerator_Delete (&fileGenerator);
    }
    
    /* return the error */
    if (error != NULL)
    {
        *error = err;
    }
    
    return fileGenerator;
}

void ARMAVLINK_FileGenerator_Delete(ARMAVLINK_FileGenerator_t **fileGenerator)
{
    ARMAVLINK_FileGenerator_t *fileGeneratorPtr = NULL;
    
    if (fileGenerator)
    {
        fileGeneratorPtr = *fileGenerator;
        
        // Uninitialize here
        ARMAVLINK_ListUtils_MissionItemListDelete(&fileGeneratorPtr->missionItemList);
        
        
        if (fileGeneratorPtr)
        {
            free (fileGeneratorPtr);
            fileGeneratorPtr = NULL;
        }
        
        *fileGenerator = NULL;
    }
}

eARMAVLINK_ERROR ARMAVLINK_FileGenerator_AddMissionItem(ARMAVLINK_FileGenerator_t *fileGenerator, mavlink_mission_item_t *missionItem)
{
    eARMAVLINK_ERROR error = ARMAVLINK_OK;
    mavlink_mission_item_t missionItemCpy;
    
    if ((fileGenerator == NULL) || (missionItem == NULL))
    {
        error = ARMAVLINK_ERROR_BAD_PARAMETER;
    }
    
    if (error == ARMAVLINK_OK)
    {
        error = ARMAVLINK_MissionItemUtils_CopyMavlinkMissionItem(&missionItemCpy, missionItem);
    }
    
    if (error == ARMAVLINK_OK)
    {
        int seq = ARMAVLINK_ListUtils_MissionItemListGetSize(fileGenerator->missionItemList);
        missionItemCpy.seq = seq;
        ARMAVLINK_ListUtils_MissionItemListAdd(fileGenerator->missionItemList, &missionItemCpy);
    }
    
    return error;
}

eARMAVLINK_ERROR ARMAVLINK_FileGenerator_ReplaceMissionItem(ARMAVLINK_FileGenerator_t *fileGenerator, mavlink_mission_item_t *missionItem, int index)
{
    eARMAVLINK_ERROR error = ARMAVLINK_OK;
    mavlink_mission_item_t missionItemCpy;
    
    if ((fileGenerator == NULL) || (missionItem == NULL))
    {
        error = ARMAVLINK_ERROR_BAD_PARAMETER;
    }
    
    if (error == ARMAVLINK_OK)
    {
        error = ARMAVLINK_MissionItemUtils_CopyMavlinkMissionItem(&missionItemCpy, missionItem);
    }
    
    if (error == ARMAVLINK_OK)
    {
        missionItemCpy.seq = index;
        ARMAVLINK_ListUtils_MissionItemListReplaceMissionItem(fileGenerator->missionItemList, &missionItemCpy, index);
    }
    
    return error;
}

eARMAVLINK_ERROR ARMAVLINK_FileGenerator_InsertMissionItem(ARMAVLINK_FileGenerator_t *fileGenerator, mavlink_mission_item_t *missionItem, int index)
{
    eARMAVLINK_ERROR error = ARMAVLINK_OK;
    mavlink_mission_item_t missionItemCpy;
    
    if ((fileGenerator == NULL) || (missionItem == NULL))
    {
        error = ARMAVLINK_ERROR_BAD_PARAMETER;
    }
    
    if (error == ARMAVLINK_OK)
    {
        error = ARMAVLINK_MissionItemUtils_CopyMavlinkMissionItem(&missionItemCpy, missionItem);
    }
    
    if (error == ARMAVLINK_OK)
    {
        missionItemCpy.seq = index;
        ARMAVLINK_ListUtils_MissionItemListInsertMissionItem(fileGenerator->missionItemList, &missionItemCpy, index);
        ARMAVLINK_FileGenerator_ResetSequences(fileGenerator->missionItemList, index+1);
    }
    
    return error;
}

eARMAVLINK_ERROR ARMAVLINK_FileGenerator_DeleteMissionItem(ARMAVLINK_FileGenerator_t *fileGenerator, int index)
{
    eARMAVLINK_ERROR error = ARMAVLINK_OK;

    error = ARMAVLINK_ListUtils_MissionItemListDeleteMissionItem(fileGenerator->missionItemList, index);
    ARMAVLINK_FileGenerator_ResetSequences(fileGenerator->missionItemList, index);
    
    return error;
}

eARMAVLINK_ERROR ARMAVLINK_FileGenerator_ResetSequences(mission_item_list_t *list, int startIndex)
{
    eARMAVLINK_ERROR error = ARMAVLINK_OK;
    
    int i;
    int size = ARMAVLINK_ListUtils_MissionItemListGetSize(list);
    for (i = startIndex; i < size; i++)
    {
        mavlink_mission_item_t *missionItem = ARMAVLINK_ListUtils_MissionItemListGet(list, i);
        missionItem->seq = i;
    }
    
    return error;
}


void ARMAVLINK_FileGenerator_CreateMavlinkFile(ARMAVLINK_FileGenerator_t *fileGenerator, const char *const filePath)
{
    // open the file
    FILE *file;
    file = fopen(filePath,"w");
    
    fprintf(file, "%s\n", ARMAVLINK_FILE_GENERATOR_FILE_HEADER);
    
    int missionItemIdx = 0;
    int missionItemCount = ARMAVLINK_ListUtils_MissionItemListGetSize(fileGenerator->missionItemList);
    for (missionItemIdx = 0; missionItemIdx < missionItemCount; missionItemIdx++)
    {
        mavlink_mission_item_t *missionItem = ARMAVLINK_ListUtils_MissionItemListGet(fileGenerator->missionItemList, missionItemIdx);
        // <INDEX> <CURRENT WP> <COORD FRAME> <COMMAND> <PARAM1> <PARAM2> <PARAM3> <PARAM4> <PARAM5/X/LONGITUDE> <PARAM6/Y/LATITUDE> <PARAM7/Z/ALTITUDE> <AUTOCONTINUE>
        // where each space is a tabulation
        fprintf(file, "%i\t%i\t%i\t%i\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%i\n",
                missionItem->seq,
                missionItem->current,
                missionItem->frame,
                missionItem->command,
                missionItem->param1,
                missionItem->param2,
                missionItem->param3,
                missionItem->param4,
                missionItem->x,
                missionItem->y,
                missionItem->z,
                missionItem->autocontinue);
    }
    fclose(file);
}

mission_item_list_t *ARMAVLINK_FileGenerator_GetCurrentMissionItemList(ARMAVLINK_FileGenerator_t *fileGenerator, eARMAVLINK_ERROR *err)
{
    eARMAVLINK_ERROR error = ARMAVLINK_OK;
    
    mission_item_list_t *listToReturn = NULL;
    
    if (fileGenerator == NULL)
    {
        error = ARMAVLINK_ERROR_BAD_PARAMETER;
    }
    
    if (ARMAVLINK_OK == error)
    {
        listToReturn = fileGenerator->missionItemList;
    }
    
    return listToReturn;
}

