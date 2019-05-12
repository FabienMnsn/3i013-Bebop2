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
 * @file ARUTILS_Manager.c
 * @brief libARUtils Manager c file.
 * @date 19/12/2013
 * @author david.flattin.ext@parrot.com
 **/

#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <libARSAL/ARSAL_Sem.h>
#include <libARSAL/ARSAL_Print.h>
#include <libARDiscovery/ARDISCOVERY_Discovery.h>
#include <libARDiscovery/ARDISCOVERY_Device.h>
#include <curl/curl.h>

#include "libARUtils/ARUTILS_Error.h"
#include "libARUtils/ARUTILS_Manager.h"
#include "libARUtils/ARUTILS_Ftp.h"
#include "ARUTILS_Manager.h"

#define ARUTILS_MANAGER_TAG "Manager"

#define FTP_GENERIC 21
#define FTP_GENERIC_SKY 121
#define FTP_UPDATE 51
#define FTP_UPDATE_SKY 151
#define FTP_FLIGHTPLAN 61
#define FTP_FLIGHTPLAN_SKY 161



ARUTILS_Manager_t* ARUTILS_Manager_New(eARUTILS_ERROR *error)
{
    ARUTILS_Manager_t *newManager = NULL;
    eARUTILS_ERROR result = ARUTILS_OK;

    ARSAL_PRINT(ARSAL_PRINT_DEBUG, ARUTILS_MANAGER_TAG, "%s", "");

    newManager = calloc(1, sizeof(ARUTILS_Manager_t));
    if (newManager == NULL) {
        result = ARUTILS_ERROR_ALLOC;
        goto out;
    }
    newManager->networkType = ARDISCOVERY_NETWORK_TYPE_UNKNOWN;

out:
    *error = result;
    return newManager;
}

void ARUTILS_Manager_Delete(ARUTILS_Manager_t **managerAddr)
{
    ARSAL_PRINT(ARSAL_PRINT_DEBUG, ARUTILS_MANAGER_TAG, "%s", "");

    if (managerAddr != NULL)
    {
        ARUTILS_Manager_t *manager = *managerAddr;
        if (manager != NULL)
        {
            free(manager);
        }
        *managerAddr = NULL;
    }
}

eARUTILS_ERROR ARUTILS_Manager_InitFtp(ARUTILS_Manager_t *manager,
                                       ARDISCOVERY_Device_t *device,
                                       eARUTILS_DESTINATION destination,
                                       eARUTILS_FTP_TYPE type)
{
    int sky, old_sky, new_sky, wifi;
    int port;
    static const char *drone = "drone";
    static const char *skyctrl = "skycontroller";
    const char *dest;
    char ip[16];
    struct mux_ctx *mux;

    eARDISCOVERY_PRODUCT product;
    eARUTILS_ERROR err = ARUTILS_OK;
    eARDISCOVERY_ERROR derr;
    ARNETWORKAL_BLEDevice_t *bleDevice;

    if (!manager || !device)
        return ARUTILS_ERROR_BAD_PARAMETER;

    product = device->productID;
    sky = (ARDISCOVERY_getProductFamily(product) ==
           ARDISCOVERY_PRODUCT_FAMILY_SKYCONTROLLER);
    old_sky = (product == ARDISCOVERY_PRODUCT_SKYCONTROLLER);
    new_sky = (sky && !old_sky);
    wifi = (device->networkType == ARDISCOVERY_NETWORK_TYPE_NET);


    if (!new_sky && destination != ARUTILS_DESTINATION_DRONE)
        return ARUTILS_ERROR_BAD_PARAMETER;

    if (type == ARUTILS_FTP_TYPE_FLIGHTPLAN &&
        destination == ARUTILS_DESTINATION_SKYCONTROLLER)
        return ARUTILS_ERROR_BAD_PARAMETER;

    switch(type) {
    case ARUTILS_FTP_TYPE_GENERIC:
        port = (new_sky && wifi && destination == ARUTILS_DESTINATION_DRONE) ? FTP_GENERIC_SKY : FTP_GENERIC;
        break;
    case ARUTILS_FTP_TYPE_UPDATE:
        port = (new_sky && wifi && destination == ARUTILS_DESTINATION_DRONE) ? FTP_UPDATE_SKY : FTP_UPDATE;
        break;
    case ARUTILS_FTP_TYPE_FLIGHTPLAN:
        port = (new_sky && wifi) ? FTP_FLIGHTPLAN_SKY : FTP_FLIGHTPLAN;
        break;
    default:
        err = ARUTILS_ERROR_BAD_PARAMETER;
        goto out;
    }

    switch(device->networkType) {
    case ARDISCOVERY_NETWORK_TYPE_NET:
        derr = ARDISCOVERY_DEVICE_WifiGetIpAddress(device, ip, sizeof(ip));
        if (derr == ARDISCOVERY_OK)
            err = ARUTILS_Manager_InitWifiFtp(manager, ip, port,
                                              ARUTILS_FTP_ANONYMOUS, "");
        else
            err = ARUTILS_ERROR_SYSTEM;
        break;
    case ARDISCOVERY_NETWORK_TYPE_BLE:
        derr = ARDISCOVERY_Device_BLEGetDevice(device, &bleDevice);
        if (derr == ARDISCOVERY_OK)
            err = ARUTILS_Manager_InitBLEFtp(manager, bleDevice, port);
        else
            err = ARUTILS_ERROR_SYSTEM;
        break;
    case ARDISCOVERY_NETWORK_TYPE_USBMUX:
        dest = (destination == ARUTILS_DESTINATION_DRONE) ? drone : skyctrl;
        derr = ARDISCOVERY_Device_UsbGetMux(device, &mux);
        if (derr == ARDISCOVERY_OK)
            err = ARUTILS_Manager_InitWifiFtpOverMux(manager, dest, port, mux,
                                                     ARUTILS_FTP_ANONYMOUS, "");
        else
            err = ARUTILS_ERROR_SYSTEM;
        break;
    default:
        err = ARUTILS_ERROR_BAD_PARAMETER;
        break;
    }

out:
    return err;

}

void ARUTILS_Manager_CloseFtp(ARUTILS_Manager_t *manager,
                              ARDISCOVERY_Device_t *device)
{
    if (!manager || !device)
        return;

    switch(device->networkType) {
    case ARDISCOVERY_NETWORK_TYPE_NET:
    case ARDISCOVERY_NETWORK_TYPE_USBMUX:
        ARUTILS_Manager_CloseWifiFtp(manager);
        break;
    case ARDISCOVERY_NETWORK_TYPE_BLE:
        ARUTILS_Manager_CloseBLEFtp(manager);
        break;
    default:
        break;
    }
}

eARUTILS_ERROR ARUTILS_Manager_Ftp_Connection_Disconnect(ARUTILS_Manager_t *manager)
{
    eARUTILS_ERROR result = ARUTILS_OK;
    if ((manager == NULL) || (manager->ftpConnectionDisconnect == NULL))
    {
        result = ARUTILS_ERROR_BAD_PARAMETER;
    }
    else
    {
        result = manager->ftpConnectionDisconnect(manager);
    }
    return result;
}

eARUTILS_ERROR ARUTILS_Manager_Ftp_Connection_Reconnect(ARUTILS_Manager_t *manager)
{
    eARUTILS_ERROR result = ARUTILS_OK;
    if ((manager == NULL) || (manager->ftpConnectionReconnect == NULL))
    {
        result = ARUTILS_ERROR_BAD_PARAMETER;
    }
    else
    {
        result = manager->ftpConnectionReconnect(manager);
    }
    return result;
}

eARUTILS_ERROR ARUTILS_Manager_Ftp_Connection_Cancel(ARUTILS_Manager_t *manager)
{
    eARUTILS_ERROR result = ARUTILS_OK;
    if ((manager == NULL) || (manager->ftpConnectionCancel == NULL))
    {
        result = ARUTILS_ERROR_BAD_PARAMETER;
    }
    else
    {
        result = manager->ftpConnectionCancel(manager);
    }
    return result;
}

eARUTILS_ERROR ARUTILS_Manager_Ftp_Connection_IsCanceled(ARUTILS_Manager_t *manager)
{
    eARUTILS_ERROR result = ARUTILS_OK;

    if ((manager == NULL) || (manager->ftpConnectionIsCanceled == NULL))
    {
        result = ARUTILS_ERROR_BAD_PARAMETER;
    }
    else
    {
        result = manager->ftpConnectionIsCanceled(manager);
    }
    return result;
}

eARUTILS_ERROR ARUTILS_Manager_Ftp_Connection_Reset(ARUTILS_Manager_t *manager)
{
    eARUTILS_ERROR result = ARUTILS_OK;
    if ((manager == NULL) || (manager->ftpConnectionReset == NULL))
    {
        result = ARUTILS_ERROR_BAD_PARAMETER;
    }
    else
    {
        result = manager->ftpConnectionReset(manager);
    }
    return result;
}

eARUTILS_ERROR ARUTILS_Manager_Ftp_List(ARUTILS_Manager_t *manager, const char *namePath, char **resultList, uint32_t *resultListLen)
{
    eARUTILS_ERROR result = ARUTILS_OK;
    if ((manager == NULL) || (manager->ftpList == NULL))
    {
        result = ARUTILS_ERROR_BAD_PARAMETER;
    }
    else
    {
        result = manager->ftpList(manager, namePath, resultList, resultListLen);
    }
    return result;
}

eARUTILS_ERROR ARUTILS_Manager_Ftp_Size(ARUTILS_Manager_t *manager, const char *namePath, double *fileSize)
{
    eARUTILS_ERROR result = ARUTILS_OK;
    if ((manager == NULL) || (manager->ftpSize == NULL))
    {
        result = ARUTILS_ERROR_BAD_PARAMETER;
    }
    else
    {
        result = manager->ftpSize(manager, namePath, fileSize);
    }
    return result;
}

eARUTILS_ERROR ARUTILS_Manager_Ftp_Get_WithBuffer(ARUTILS_Manager_t *manager, const char *namePath, uint8_t **data, uint32_t *dataLen,  ARUTILS_Ftp_ProgressCallback_t progressCallback, void* progressArg)
{
    eARUTILS_ERROR result = ARUTILS_OK;
    if ((manager == NULL) || (manager->ftpGetWithBuffer == NULL))
    {
        result = ARUTILS_ERROR_BAD_PARAMETER;
    }
    else
    {
        result = manager->ftpGetWithBuffer(manager, namePath, data, dataLen, progressCallback, progressArg);
    }
    return result;
}

eARUTILS_ERROR ARUTILS_Manager_Ftp_Get(ARUTILS_Manager_t *manager, const char *namePath, const char *dstFile, ARUTILS_Ftp_ProgressCallback_t progressCallback, void* progressArg, eARUTILS_FTP_RESUME resume)
{
    eARUTILS_ERROR result = ARUTILS_OK;
    if ((manager == NULL) || (manager->ftpGet == NULL))
    {
        result = ARUTILS_ERROR_BAD_PARAMETER;
    }
    else
    {
        result = manager->ftpGet(manager, namePath, dstFile, progressCallback, progressArg, resume);
    }
    return result;
}

eARUTILS_ERROR ARUTILS_Manager_Ftp_Put(ARUTILS_Manager_t *manager, const char *namePath, const char *srcFile, ARUTILS_Ftp_ProgressCallback_t progressCallback, void* progressArg, eARUTILS_FTP_RESUME resume)
{
    eARUTILS_ERROR result = ARUTILS_OK;
    if ((manager == NULL) || (manager->ftpPut == NULL))
    {
        result = ARUTILS_ERROR_BAD_PARAMETER;
    }
    else
    {
        result = manager->ftpPut(manager, namePath, srcFile, progressCallback, progressArg, resume);
    }
    return result;
}

eARUTILS_ERROR ARUTILS_Manager_Ftp_Delete(ARUTILS_Manager_t *manager, const char *namePath)
{
    eARUTILS_ERROR result = ARUTILS_OK;
    if ((manager == NULL) || (manager->ftpDelete == NULL))
    {
        result = ARUTILS_ERROR_BAD_PARAMETER;
    }
    else
    {
        result = manager->ftpDelete(manager, namePath);
    }
    return result;
}

eARUTILS_ERROR ARUTILS_Manager_Ftp_RemoveDir(ARUTILS_Manager_t *manager, const char *namePath)
{
    eARUTILS_ERROR result = ARUTILS_OK;
    if ((manager == NULL) || (manager->ftpRemoveDir == NULL))
    {
        result = ARUTILS_ERROR_BAD_PARAMETER;
    }
    else
    {
        result = manager->ftpRemoveDir(manager, namePath);
    }
    return result;
}

eARUTILS_ERROR ARUTILS_Manager_Ftp_Rename(ARUTILS_Manager_t *manager, const char *oldNamePath, const char *newNamePath)
{
    eARUTILS_ERROR result = ARUTILS_OK;
    if ((manager == NULL) || (manager->ftpRename == NULL))
    {
        result = ARUTILS_ERROR_BAD_PARAMETER;
    }
    else
    {
        result = manager->ftpRename(manager, oldNamePath, newNamePath);
    }
    return result;
}
