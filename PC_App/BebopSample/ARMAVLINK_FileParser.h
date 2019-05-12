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
 * @file ARMAVLINK_FileParser.h
 * @brief Mavlink File Parser
 * @date 14/05/2014
 * @author djavan.bertrand@parrot.com
 */
#ifndef _ARMAVLINK_FILE_PARSER_PRIVATE_H
#define _ARMAVLINK_FILE_PARSER_PRIVATE_H

#include <libARMavlink/ARMAVLINK_FileParser.h>
#include <libARMavlink/ARMAVLINK_Error.h>
#include "ARMAVLINK_ListUtils.h"

/**
 * @brief ARMavlink file parser structure allows to manage the process of reading Mavlink files
 */
struct ARMAVLINK_FileParser_t
{
    char *mavlinkVersion;
};

/**
 * @brief Read the first line of the mavlink file
 * @param fileParser : Pointer on the file parser
 * @param[in|out] line : the first line
 * @warning : the line param will be modified by this function
 */
eARMAVLINK_ERROR ARMAVLINK_FileParser_ReadFirstLine(ARMAVLINK_FileParser_t *fileParser, char *line);

/**
 * @brief Read a mavlink command and add the command to the mission item list
 * @param fileParser : Pointer on the file parser
 * @param[in|out] line : the line
 * @param[out] missionItemList : the list to add the mission item
 * @warning : the line param will be modified by this function
 */
eARMAVLINK_ERROR ARMAVLINK_FileParser_ReadMavlinkCommand(ARMAVLINK_FileParser_t *fileParser, char *line, mission_item_list_t *missionItemList);

#endif
