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
 * @file ARUTILS_FileSystem.c
 * @brief libARUtils FileSystem c file.
 * @date 19/12/2013
 * @author david.flattin.ext@parrot.com
 **/

#include "config.h"
#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#ifdef HAVE_SYS_STATFS_H
#include <sys/statfs.h> //linux
#endif
#ifdef HAVE_SYS_MOUNT_H
#include <sys/mount.h> //ios
#endif

#include <libARSAL/ARSAL_Print.h>
#include <libARSAL/ARSAL_Ftw.h>

#include "libARUtils/ARUTILS_Error.h"
#include "libARUtils/ARUTILS_FileSystem.h"

#define ARUTILS_FILE_SYSTEM_TAG   "FileSystem"

/*****************************************
 *
 *             Private implementation:
 *
 *****************************************/

eARUTILS_ERROR ARUTILS_FileSystem_IsExist(const char *namePath)
{
	struct stat statbuf;
    eARUTILS_ERROR result = ARUTILS_OK;
    int resultSys = 0;

    memset(&statbuf, 0, sizeof(statbuf));
    
    if (namePath == NULL)
    {
        return ARUTILS_ERROR_BAD_PARAMETER;
    }
    
    resultSys = stat(namePath, &statbuf);
    if (resultSys != 0)
    {
        if (errno == ENOENT)
        {
            result = ARUTILS_ERROR_FILE_NOT_FOUND;
        }
        else
        {
            result = ARUTILS_ERROR_SYSTEM;
        }
    }
    
    return result;
}

eARUTILS_ERROR ARUTILS_FileSystem_GetFileSize(const char *namePath, int64_t *size)
{
#ifdef stat64
    struct stat64 statbuf;
#else
    struct stat statbuf;
#endif
    int64_t fileSize = 0;
    eARUTILS_ERROR result = ARUTILS_OK;
    int resultSys = 0;

    memset(&statbuf, 0, sizeof(statbuf));

    ARSAL_PRINT(ARSAL_PRINT_DEBUG, ARUTILS_FILE_SYSTEM_TAG, "%s", namePath ? namePath : "null");

    if (namePath == NULL)
    {
        return ARUTILS_ERROR_BAD_PARAMETER;
    }
    
#ifdef stat64
    resultSys = stat64(namePath, &statbuf);
#else
    resultSys = stat(namePath, &statbuf);
#endif

    if (resultSys == 0)
    {
        if (S_ISREG(statbuf.st_mode))
        {
            fileSize = (int64_t)statbuf.st_size;
        }
    }
    else
    {
        result = ARUTILS_ERROR_SYSTEM;

        if (errno == ENOENT)
        {
            result = ARUTILS_ERROR_FILE_NOT_FOUND;
        }
    }

    *size = fileSize;
    return result;
}

eARUTILS_ERROR ARUTILS_FileSystem_Rename(const char *oldName, const char *newName)
{
    eARUTILS_ERROR result = ARUTILS_OK;
    int resultSys = 0;

    ARSAL_PRINT(ARSAL_PRINT_DEBUG, ARUTILS_FILE_SYSTEM_TAG, "%s, %s", oldName ? oldName : "null", newName ? newName : "null");

    resultSys = rename(oldName, newName);

    if (resultSys != 0)
    {
        result = ARUTILS_ERROR_SYSTEM;
    }

    return result;
}

eARUTILS_ERROR ARUTILS_FileSystem_RemoveFile(const char *localPath)
{
    eARUTILS_ERROR result = ARUTILS_OK;
    int resultSys = 0;

    ARSAL_PRINT(ARSAL_PRINT_DEBUG, ARUTILS_FILE_SYSTEM_TAG, "%s", localPath ? localPath : "null");

    resultSys = remove(localPath);

    if (resultSys != 0)
    {
        result = ARUTILS_ERROR_SYSTEM;
    }

    return result;
}

static int ARUTILS_FileSystem_RemoveDirCallback(const char* fpath, const struct stat *sb, eARSAL_FTW_TYPE typeflag, ARSAL_FTW_t *ftwbuf)
{
    int ret;
     if(typeflag == ARSAL_FTW_F)
    {
       ret = remove(fpath);
       if (ret < 0 && errno != ENOENT) {
            ret = errno;
            ARSAL_PRINT(ARSAL_PRINT_ERROR, ARUTILS_FILE_SYSTEM_TAG, "remove '%s' error: %s", fpath, strerror(ret));
       }
    }
    else if(typeflag == ARSAL_FTW_D)
    {
        if (ftwbuf->level > 0)
        {
            ARUTILS_FileSystem_RemoveDir(fpath);
        }
    }

    return 0;
}

eARUTILS_ERROR ARUTILS_FileSystem_RemoveDir(const char *localPath)
{
    eARUTILS_ERROR result = ARUTILS_OK;

    ARSAL_PRINT(ARSAL_PRINT_DEBUG, ARUTILS_FILE_SYSTEM_TAG, "%s", localPath ? localPath : "null");

    int resultSys = ARSAL_Nftw(localPath, ARUTILS_FileSystem_RemoveDirCallback, ARUTILS_FILE_SYSTEM_MAX_FD_FOR_FTW, ARSAL_FTW_ACTIONRETVAL);

    if (resultSys == 0)
    {
        resultSys = rmdir(localPath);

        if (resultSys != 0)
        {
            result = ARUTILS_ERROR_SYSTEM;
        }
    }
    else
    {
        if (errno == ENOENT)
        {
            result = ARUTILS_ERROR_FILE_NOT_FOUND;
        }
        else
        {
            result = ARUTILS_ERROR_SYSTEM;
        }
    }

    return result;
}

eARUTILS_ERROR ARUTILS_FileSystem_GetFreeSpace(const char *localPath, double *freeSpace)
{
    eARUTILS_ERROR result = ARUTILS_OK;
    struct statfs statfsData;
    double freeBytes = 0.f;

    ARSAL_PRINT(ARSAL_PRINT_DEBUG, ARUTILS_FILE_SYSTEM_TAG, "%s", localPath ? localPath : "null");

    int resultSys = statfs(localPath, &statfsData);

    if (resultSys != 0)
    {
        result = ARUTILS_ERROR_SYSTEM;
    }
    else
    {
        freeBytes = ((double)statfsData.f_bavail) * ((double)statfsData.f_bsize);
    }

    *freeSpace = freeBytes;
    return result;
}
