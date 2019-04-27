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
 * @file ARUTILS_Ftp.h
 * @brief libARUtils Ftp header file.
 * @date 19/12/2013
 * @author david.flattin.ext@parrot.com
 **/

#ifndef _ARUTILS_BLE_FTP_PRIVATE_H_
#define _ARUTILS_BLE_FTP_PRIVATE_H_

#include "libARUtils/ARUTILS_Manager.h"


/**
 * @brief BLEFtp Connection structure containing private connection data.
 * @see ARUTILS_BLEFtp_Connection_New
 */
typedef struct _ARUTILS_BLEFtp_Connection_t_
{
    ARUTILS_Manager_t *manager; /**< Backpointer to the ARUTILS_Manager. */

} ARUTILS_BLEFtp_Connection_t;


@interface ARUtils_BLEFtp : NSObject
DECLARE_SINGLETON_FOR_CLASS(ARUtils_BLEFtp)

- (id)initBLEFtp;
- (eARUTILS_ERROR)registerConnection:(ARUTILS_BLEFtp_Connection_t*)connection withPeripheral:(CBPeripheral *)peripheral port:(int)port;
- (eARUTILS_ERROR)unregisterConnection:(ARUTILS_BLEFtp_Connection_t*)connection;
- (eARUTILS_ERROR)registerCharacteristics;
- (eARUTILS_ERROR)unregisterCharacteristics;

- (eARUTILS_ERROR)cancelConnection:(ARUTILS_BLEFtp_Connection_t*)connection;
- (eARUTILS_ERROR)listFiles:(NSString*)remotePath resultList:(char **)resultList resultListLen:(uint32_t *)resultListLen forConnection:(ARUTILS_BLEFtp_Connection_t*)connection;
- (eARUTILS_ERROR)sizeFile:(NSString*)remoteFile fileSize:(double*)fileSize forConnection:(ARUTILS_BLEFtp_Connection_t*)connection;
- (eARUTILS_ERROR)getFile:(NSString*)remoteFile localFile:(NSString*)localFile progressCallback:(ARUTILS_Ftp_ProgressCallback_t)progressCallback progressArg:(void *)progressArg forConnection:(ARUTILS_BLEFtp_Connection_t*)connection;
- (eARUTILS_ERROR)getFileWithBuffer:(NSString*)remoteFile data:(uint8_t**)data dataLen:(uint32_t*)dataLen progressCallback:(ARUTILS_Ftp_ProgressCallback_t)progressCallback progressArg:(void *)progressArg forConnection:(ARUTILS_BLEFtp_Connection_t*)connection;
- (eARUTILS_ERROR)putFile:(NSString*)remoteFile localFile:(NSString*)localFile progressCallback:(ARUTILS_Ftp_ProgressCallback_t)progressCallback progressArg:(void *)progressArg resume:(BOOL)resume forConnection:(ARUTILS_BLEFtp_Connection_t*)connection;
- (eARUTILS_ERROR)abortPutFileResume:(NSString*)remoteFile forConnection:(ARUTILS_BLEFtp_Connection_t*)connection;
- (eARUTILS_ERROR)deleteFile:(NSString*)remoteFile forConnection:(ARUTILS_BLEFtp_Connection_t*)connection;
- (eARUTILS_ERROR)renameFile:(NSString*)oldNamePath newNamePath:(NSString*)newNamePath forConnection:(ARUTILS_BLEFtp_Connection_t*)connection;

@end


/**
 * @brief Create a new Ftp Connection
 * @warning This function allocates memory
 * @param[in] manager A pointer to the manager the connection is tied to.
 * @param[in] device The BLE Ftp device
 * @param[out] error The pointer of the error code: if success ARUTILS_OK, otherwise an error number of eARUTILS_ERROR
 * @retval On success, returns an ARUTILS_BLEFtp_Connection_t. Otherwise, it returns NULL.
 * @see ARUTILS_FtpAL_DeleteConnection ()
 */
ARUTILS_BLEFtp_Connection_t * ARUTILS_BLEFtp_Connection_New(ARUTILS_Manager_t *manager, ARUTILS_BLEDevice_t device, int port, eARUTILS_ERROR *error);

/**
 * @brief Delete an Ftp Connection
 * @warning This function frees memory
 * @param connection The address of the pointer on the Ftp Connection
 * @see ARUTILS_FtpAL_NewConnection ()
 */
void ARUTILS_BLEFtp_Connection_Delete(ARUTILS_BLEFtp_Connection_t **connection);


/**
 * @brief Disconnect an remote Ftp server connection
 * @param connection The address of the pointer on the Ftp Manager
 * @retval On success, returns ARUTILS_OK. Otherwise, it returns an error number of eARUTILS_ERROR.
 * @see ARUTILS_Manager_NewBLEFtp ()
 */
eARUTILS_ERROR ARUTILS_BLEFtpAL_Connection_Disconnect(ARUTILS_Manager_t *manager);

/**
 * @brief Reconnect an remote Ftp server connection
 * @param connection The address of the pointer on the Ftp Manager
 * @retval On success, returns ARUTILS_OK. Otherwise, it returns an error number of eARUTILS_ERROR.
 * @see ARUTILS_Manager_NewBLEFtp ()
 */
eARUTILS_ERROR ARUTILS_BLEFtpAL_Connection_Reconnect(ARUTILS_Manager_t *manager);


/**
 * @brief Cancel an Ftp Connection command in progress (get, put, list etc)
 * @param connection The address of the pointer on the Ftp Connection
 * @retval On success, returns ARUTILS_OK. Otherwise, it returns an error number of eARUTILS_ERROR.
 * @see ARUTILS_Ftp_NewConnection ()
 */
eARUTILS_ERROR ARUTILS_BLEFtp_Connection_Cancel(ARUTILS_BLEFtp_Connection_t *connection);

/**
 * @brief Check if the connection has received a cancel to it's semaphore
 * @param connection The address of the pointer on the Ftp Connection
 * @retval On success, returns ARUTILS_OK. Otherwise, it returns an error number of eARUTILS_ERROR.
 * @see cURL
 */
eARUTILS_ERROR ARUTILS_BLEFtp_Connection_IsCanceled(ARUTILS_BLEFtp_Connection_t *connection);

/**
 * @brief Reset an Ftp Connection command in progress (get, put, list etc)
 * @param connection The address of the pointer on the Ftp Connection
 * @retval On success, returns ARUTILS_OK. Otherwise, it returns an error number of eARUTILS_ERROR.
 * @see ARUTILS_Ftp_NewConnection ()
 */
eARUTILS_ERROR ARUTILS_BLEFtp_Connection_Reset(ARUTILS_BLEFtp_Connection_t *connection);

/**
 * @brief Execute Ftp List command to retrieve directory content
 * @warning This function allocates memory
 * @param connection The address of the pointer on the Ftp Connection
 * @param namePath The string of the directory path on the remote Ftp server
 * @param resultList The pointer of the string of the directory content null terminated
 * @param resultListLen The pointer of the lenght of the resultList string including null terminated
 * @retval On success, returns ARUTILS_OK. Otherwise, it returns an error number of eARUTILS_ERROR.
 * @see ARUTILS_BLEFtp_NewConnection ()
 */
eARUTILS_ERROR ARUTILS_BLEFtp_List(ARUTILS_BLEFtp_Connection_t *connection, const char *remotePath, char **resultList, uint32_t *resultListLen);

/**
 * @brief Execute Ftp Size command to retrieve file size
 * @warning This function allocates memory
 * @param connection The address of the pointer on the Ftp Connection
 * @param namePath The string of the directory path on the remote Ftp server
 * @param fileSize The returned file size
 * @retval On success, returns ARUTILS_OK. Otherwise, it returns an error number of eARUTILS_ERROR.
 * @see ARUTILS_BLEFtp_NewConnection ()
 */
eARUTILS_ERROR ARUTILS_BLEFtp_Size(ARUTILS_BLEFtp_Connection_t *connection, const char *remotePath, double *fileSize);

/**
 * @brief Delete an remote Ftp server file
 * @param connection The address of the pointer on the Ftp Connection
 * @param namePath The string of the file name path on the remote Ftp server
 * @retval On success, returns ARUTILS_OK. Otherwise, it returns an error number of eARUTILS_ERROR.
 * @see ARUTILS_Ftp_NewConnection ()
 */
eARUTILS_ERROR ARUTILS_BLEFtp_Delete(ARUTILS_BLEFtp_Connection_t *connection, const char *remotePath);

/**
 * @brief Rename a remote Ftp server file
 * @param connection The address of the pointer on the Ftp Connection
 * @param oldNamePath The string of the old file name path on the remote Ftp server
 * @param newNamePath The string of the new file name path on the remote Ftp server
 * @retval On success, returns ARUTILS_OK. Otherwise, it returns an error number of eARUTILS_ERROR.
 * @see ARUTILS_Ftp_NewConnection ()
 */
eARUTILS_ERROR ARUTILS_BLEFtp_Rename(ARUTILS_BLEFtp_Connection_t *connection, const char *oldNamePath, const char *newNamePath);

/**
 * @brief Get an remote Ftp server file
 * @warning This function allocates memory
 * @param connection The address of the pointer on the Ftp Connection
 * @param namePath The string of the file name path on the remote Ftp server
 * @param data The pointer of the data buffer of the file data
 * @param dataLen The pointer of the length of the data buffer
 * @param progressCallback The progress callback function
 * @param progressArg The progress callback function arg
 * @retval On success, returns ARUTILS_OK. Otherwise, it returns an error number of eARUTILS_ERROR.
 * @see ARUTILS_BLEFtp_NewConnection (), ARUTILS_Ftp_ProgressCallback_t, eARUTILS_FTP_RESUME
 */
eARUTILS_ERROR ARUTILS_BLEFtp_Get_WithBuffer(ARUTILS_BLEFtp_Connection_t *connection, const char *remotePath, uint8_t **data, uint32_t *dataLen,  ARUTILS_Ftp_ProgressCallback_t progressCallback, void* progressArg);

/**
 * @brief Get an remote Ftp server file
 * @param connection The address of the pointer on the Ftp Connection
 * @param namePath The string of the file name path on the remote Ftp server
 * @param dstFile The string of the local file name path to be get
 * @param progressCallback The progress callback function
 * @param progressArg The progress callback function arg
 * @param resume The resume capability requested
 * @retval On success, returns ARUTILS_OK. Otherwise, it returns an error number of eARUTILS_ERROR.
 * @see ARUTILS_BLEFtp_NewConnection (), ARUTILS_Ftp_ProgressCallback_t, eARUTILS_FTP_RESUME
 */
eARUTILS_ERROR ARUTILS_BLEFtp_Get(ARUTILS_BLEFtp_Connection_t *connection, const char *remotePath, const char *dstFile, ARUTILS_Ftp_ProgressCallback_t progressCallback, void* progressArg, eARUTILS_FTP_RESUME resume);

/**
 * @brief Put an remote Ftp server file
 * @param connection The address of the pointer on the Ftp Connection
 * @param namePath The string of the file name path on the remote Ftp server
 * @param srcFile The string of the local file name path to be put
 * @param progressCallback The progress callback function
 * @param progressArg The progress callback function arg
 * @param resume The resume capability requested
 * @retval On success, returns ARUTILS_OK. Otherwise, it returns an error number of eARUTILS_ERROR.
 * @see ARUTILS_BLEFtp_NewConnection (), ARUTILS_Ftp_ProgressCallback_t, eARUTILS_FTP_RESUME
 */
eARUTILS_ERROR ARUTILS_BLEFtp_Put(ARUTILS_BLEFtp_Connection_t *connection, const char *remotePath, const char *srcFile, ARUTILS_Ftp_ProgressCallback_t progressCallback, void* progressArg, eARUTILS_FTP_RESUME resume);

/**
 * @brief Cancel an Ftp Connection command in progress (get, put, list etc)
 * @param connection The address of the pointer on the Ftp Connection
 * @retval On success, returns ARUTILS_OK. Otherwise, it returns an error number of eARUTILS_ERROR.
 * @see ARUTILS_Manager_NewBLEFtp ()
 */
eARUTILS_ERROR ARUTILS_BLEFtpAL_Connection_Cancel(ARUTILS_Manager_t *manager);

/**
 * @brief Test canceled status of Ftp Connection command in progress (get, put, list etc)
 * @param connection The address of the pointer on the Ftp Connection
 * @retval On success, returns ARUTILS_OK. Otherwise, it returns an error number of eARUTILS_ERROR.
 * @see ARUTILS_Manager_NewBLEFtp ()
 */
eARUTILS_ERROR ARUTILS_BLEFtpAL_Connection_IsCanceled(ARUTILS_Manager_t *manager);

/**
 * @brief Reset an Ftp Connection
 * @param connection The address of the pointer on the Ftp Connection
 * @retval On success, returns ARUTILS_OK. Otherwise, it returns an error number of eARUTILS_ERROR.
 * @see ARUTILS_Manager_NewBLEFtp ()
 */
eARUTILS_ERROR ARUTILS_BLEFtpAL_Connection_Reset(ARUTILS_Manager_t *manager);

/**
 * @brief Execute Ftp List command to retrieve directory content
 * @warning This function allocates memory
 * @param manager The address of the pointer on the Ftp Connection
 * @param namePath The string of the directory path on the remote Ftp server
 * @param resultList The pointer of the string of the directory content null terminated
 * @param resultListLen The pointer of the lenght of the resultList string including null terminated
 * @retval On success, returns ARUTILS_OK. Otherwise, it returns an error number of eARUTILS_ERROR.
 * @see ARUTILS_Manager_NewBLEFtp ()
 */
eARUTILS_ERROR ARUTILS_BLEFtpAL_List(ARUTILS_Manager_t *manager, const char *namePath, char **resultList, uint32_t *resultListLen);

/**
 * @brief Execute Ftp Size command to retrieve file size
 * @warning This function allocates memory
 * @param manager The address of the pointer on the Ftp Connection
 * @param namePath The string of the directory path on the remote Ftp server
 * @param fileSize The returned file size
 * @retval On success, returns ARUTILS_OK. Otherwise, it returns an error number of eARUTILS_ERROR.
 * @see ARUTILS_Manager_NewBLEFtp ()
 */
eARUTILS_ERROR ARUTILS_BLEFtpAL_Size(ARUTILS_Manager_t *manager, const char *namePath, double *fileSize);

/**
 * @brief Get an remote Ftp server file
 * @warning This function allocates memory
 * @param manager The address of the pointer on the Ftp Connection
 * @param namePath The string of the file name path on the remote Ftp server
 * @param data The pointer of the data buffer of the file data
 * @param dataLen The pointer of the length of the data buffer
 * @param progressCallback The progress callback function
 * @param progressArg The progress callback function arg
 * @retval On success, returns ARUTILS_OK. Otherwise, it returns an error number of eARUTILS_ERROR.
 * @see ARUTILS_Manager_NewBLEFtp (), ARUTILS_Ftp_ProgressCallback_t, eARUTILS_FTP_RESUME
 */
eARUTILS_ERROR ARUTILS_BLEFtpAL_Get_WithBuffer(ARUTILS_Manager_t *manager, const char *namePath, uint8_t **data, uint32_t *dataLen,  ARUTILS_Ftp_ProgressCallback_t progressCallback, void* progressArg);

/**
 * @brief Get an remote Ftp server file
 * @param manager The address of the pointer on the Ftp Connection
 * @param namePath The string of the file name path on the remote Ftp server
 * @param dstFile The string of the local file name path to be get
 * @param progressCallback The progress callback function
 * @param progressArg The progress callback function arg
 * @param resume The resume capability requested
 * @retval On success, returns ARUTILS_OK. Otherwise, it returns an error number of eARUTILS_ERROR.
 * @see ARUTILS_Manager_NewBLEFtp (), ARUTILS_Ftp_ProgressCallback_t, eARUTILS_FTP_RESUME
 */
eARUTILS_ERROR ARUTILS_BLEFtpAL_Get(ARUTILS_Manager_t *manager, const char *namePath, const char *dstFile, ARUTILS_Ftp_ProgressCallback_t progressCallback, void* progressArg, eARUTILS_FTP_RESUME resume);

/**
 * @brief Put an remote Ftp server file
 * @param manager The address of the pointer on the Ftp Connection
 * @param namePath The string of the file name path on the remote Ftp server
 * @param srcFile The string of the local file name path to be put
 * @param progressCallback The progress callback function
 * @param progressArg The progress callback function arg
 * @param resume The resume capability requested
 * @retval On success, returns ARUTILS_OK. Otherwise, it returns an error number of eARUTILS_ERROR.
 * @see ARUTILS_Manager_NewBLEFtp (), ARUTILS_Ftp_ProgressCallback_t, eARUTILS_FTP_RESUME
 */
eARUTILS_ERROR ARUTILS_BLEFtpAL_Put(ARUTILS_Manager_t *manager, const char *namePath, const char *srcFile, ARUTILS_Ftp_ProgressCallback_t progressCallback, void* progressArg, eARUTILS_FTP_RESUME resume);

/**
 * @brief Delete an remote Ftp server file
 * @param manager The address of the pointer on the Ftp Connection
 * @param namePath The string of the file name path on the remote Ftp server
 * @retval On success, returns ARUTILS_OK. Otherwise, it returns an error number of eARUTILS_ERROR.
 * @see ARUTILS_Manager_NewBLEFtp ()
 */
eARUTILS_ERROR ARUTILS_BLEFtpAL_Delete(ARUTILS_Manager_t *manager, const char *namePath);

/**
 * @brief Rename an remote Ftp server file
 * @param manager The address of the pointer on the Ftp Connection
 * @param oldNamePath The string of the old file name path on the remote Ftp server
 * @param newNamePath The string of the new file name path on the remote Ftp server
 * @retval On success, returns ARUTILS_OK. Otherwise, it returns an error number of eARUTILS_ERROR.
 * @see ARUTILS_Manager_NewWifiFtp ()
 */
eARUTILS_ERROR ARUTILS_BLEFtpAL_Rename(ARUTILS_Manager_t *manager, const char *oldNamePath, const char *newNamePath);

#endif /* _ARUTILS_BLE_FTP_PRIVATE_H_ */
