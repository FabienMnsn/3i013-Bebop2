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
 * @file ARMAVLINK_Manager.c
 * @brief ARMavlink manager.
 * @date 11/13/2013
 * @author frederic.dhaeyer@parrot.com
 */
#include <stdlib.h>
#include <libARSAL/ARSAL_Print.h>
#include "ARMAVLINK_Manager.h"

/* ***************************************
 *
 *             define :
 *
 *****************************************/
#define ARMAVLINK_MANAGER_TAG    "ARMAVLINK_Manager"

ARMAVLINK_Manager_t* ARMAVLINK_Manager_New(eARMAVLINK_ERROR *error)
{
    ARMAVLINK_Manager_t *manager = NULL;
    eARMAVLINK_ERROR err = ARMAVLINK_OK;
    
    /* Check parameters */
    if(err == ARMAVLINK_OK)
    {
        /* Create the Manager */
        manager = malloc (sizeof (ARMAVLINK_Manager_t));
        if (manager == NULL)
        {
            err = ARMAVLINK_ERROR_ALLOC;
        }
    }
    
    if(err == ARMAVLINK_OK)
    {
        /* Initialize to default values */
        
    }

    /* delete the Manager if an error occurred */
    if (err != ARMAVLINK_OK)
    {
        ARSAL_PRINT (ARSAL_PRINT_ERROR, ARMAVLINK_MANAGER_TAG, "error: %s", ARMAVLINK_Error_ToString (err));
        ARMAVLINK_Manager_Delete (&manager);
    }
    
    /* return the error */
    if (error != NULL)
    {
        *error = err;
    }

    return manager;
}

void ARMAVLINK_Manager_Delete(ARMAVLINK_Manager_t **manager)
{
    ARMAVLINK_Manager_t *managerPtr = NULL;
    
    if (manager)
    {
        managerPtr = *manager;
        
        // Uninitialize here
        
        
        if (managerPtr)
        {
            free (managerPtr);
            managerPtr = NULL;
        }
        
        *manager = NULL;
    }
}
