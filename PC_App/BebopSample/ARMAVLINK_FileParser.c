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
 * @file ARMAVLINK_FileParser.c
 * @brief Mavlink File Parser
 * @date 14/05/2014
 * @author djavan.bertrand@parrot.com
 */
#include <stdlib.h>
#include <stdio.h>
#include <libARSAL/ARSAL_Print.h>
#include "ARMAVLINK_FileParser.h"
#include "ARMAVLINK_MissionItemUtils.h"
#include <string.h>

/* ***************************************
 *
 *             define :
 *
 *****************************************/
#define ARMAVLINK_FILE_PARSER_TAG                                "ARMAVLINK_FileParser"

#define ARMAVLINK_FILE_PARSER_FILE_HEADER                        "QGC WPL 120"
#define ARMAVLINK_FILE_PARSER_MAX_CHAR_IN_LINE                   255
#define ARMAVLINK_FILE_PARSER_QGC_WORD                           "QGC"
#define ARMAVLINK_FILE_PARSER_WPL_WORD                           "WPL"
/* ***************************************
 *
 *             functions :
 *
 *****************************************/
ARMAVLINK_FileParser_t* ARMAVLINK_FileParser_New(eARMAVLINK_ERROR *error)
{
    ARMAVLINK_FileParser_t *fileParser = NULL;
    if (!error)
    {
        goto finish;
    }
    *error = ARMAVLINK_OK;

    /* Create the file parser */
    fileParser = malloc (sizeof (ARMAVLINK_FileParser_t));
    if (fileParser == NULL)
    {
        *error = ARMAVLINK_ERROR_ALLOC;
        ARSAL_PRINT (ARSAL_PRINT_ERROR, ARMAVLINK_FILE_PARSER_TAG, "error: %s", ARMAVLINK_Error_ToString (*error));
        goto finish;
    }
    fileParser->mavlinkVersion = NULL;

finish:
    return fileParser;
}

void ARMAVLINK_FileParser_Delete(ARMAVLINK_FileParser_t **fileParser)
{
    if (fileParser == NULL || *fileParser == NULL)
    {
        return;
    }
    
    ARMAVLINK_FileParser_t *fileParserPtr = NULL;
    
    fileParserPtr = *fileParser;
    
    // Uninitialize here
    if (fileParserPtr->mavlinkVersion)
    {
        free (fileParserPtr->mavlinkVersion);
        fileParserPtr->mavlinkVersion = NULL;
    }
    
    free (fileParserPtr);
    fileParserPtr = NULL;
    
    *fileParser = NULL;
}

eARMAVLINK_ERROR ARMAVLINK_FileParser_Parse(ARMAVLINK_FileParser_t *fileParser, const char *const filePath, mission_item_list_t *missionItemList)
{
    eARMAVLINK_ERROR error = ARMAVLINK_OK;

    FILE *file;
    char line[ARMAVLINK_FILE_PARSER_MAX_CHAR_IN_LINE];

    // check params
    if (missionItemList == NULL || filePath == NULL || fileParser == NULL)
    {
        error = ARMAVLINK_ERROR_BAD_PARAMETER;
        goto finish;
    }

    // try to open the file
    file = fopen(filePath,"rb");
    if (file == NULL)
    {
        error = ARMAVLINK_ERROR_FILE_PARSER_FILE_NOT_FOUND;
        goto finish;
    }

    // read the file
    // read the first line (the qgc description)
    if ((ARMAVLINK_OK == error) &&
        (fgets(line, ARMAVLINK_FILE_PARSER_MAX_CHAR_IN_LINE, file) != NULL))
    {
        error = ARMAVLINK_FileParser_ReadFirstLine(fileParser, line);
    }
    
    // read the rest of the file
    while ((ARMAVLINK_OK == error) &&
           (fgets(line, ARMAVLINK_FILE_PARSER_MAX_CHAR_IN_LINE, file) != NULL))
    {
        error = ARMAVLINK_FileParser_ReadMavlinkCommand(fileParser, line, missionItemList);
    }

    if (file != NULL)
    {
        fclose(file);
    }

finish:
    return error;
}

eARMAVLINK_ERROR ARMAVLINK_FileParser_ReadFirstLine(ARMAVLINK_FileParser_t *fileParser, char *line)
{
    eARMAVLINK_ERROR error = ARMAVLINK_OK;
    char qgc_word[ARMAVLINK_FILE_PARSER_MAX_CHAR_IN_LINE];
    char wpl_word[ARMAVLINK_FILE_PARSER_MAX_CHAR_IN_LINE];
    char filename_word[ARMAVLINK_FILE_PARSER_MAX_CHAR_IN_LINE];
    if (sscanf(line, "%s %s %s", qgc_word, wpl_word, filename_word) != 3)
    {
        error = ARMAVLINK_ERROR_FILE_PARSER_WORD_NOT_EXPTECTED;
        goto finish;
    }
    qgc_word[ARMAVLINK_FILE_PARSER_MAX_CHAR_IN_LINE-1] = '\0';
    wpl_word[ARMAVLINK_FILE_PARSER_MAX_CHAR_IN_LINE-1] = '\0';
    filename_word[ARMAVLINK_FILE_PARSER_MAX_CHAR_IN_LINE-1] = '\0';

    fileParser->mavlinkVersion = malloc(sizeof(char) * strlen(filename_word));
    strcpy(fileParser->mavlinkVersion, filename_word);

finish:
    return error;
}

eARMAVLINK_ERROR ARMAVLINK_FileParser_ReadMavlinkCommand(ARMAVLINK_FileParser_t *fileParser, char *line, mission_item_list_t *missionItemList)
{
    float param1;
    float param2;
    float param3;
    float param4;
    float longitude;
    float latitude;
    float altitude;
    int seq;
    int command;
    int frame;
    int current;
    int autocontinue;

    mavlink_mission_item_t missionItem;

    eARMAVLINK_ERROR error = ARMAVLINK_OK;

    // <INDEX> <CURRENT WP> <COORD FRAME> <COMMAND> <PARAM1> <PARAM2> <PARAM3> <PARAM4> <PARAM5/X/LONGITUDE> <PARAM6/Y/LATITUDE> <PARAM7/Z/ALTITUDE> <AUTOCONTINUE>
    // where each space is a tabulation
    if (sscanf (line, "%i\t%i\t%i\t%i\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%i\n",
                            &seq,
                            &current,
                            &frame,
                            &command,
                            &param1,
                            &param2,
                            &param3,
                            &param4,
                            &latitude,
                            &longitude,
                            &altitude,
                            &autocontinue) != 12)
    {
        error = ARMAVLINK_ERROR_FILE_PARSER_WORD_NOT_EXPTECTED;
        goto finish;
    }

    error = ARMAVLINK_MissionItemUtils_CreateMavlinkMissionItemWithAllParams(&missionItem, param1, param2, param3, param4,
                                                                              latitude, longitude, altitude, command,
                                                                              seq, frame, current, autocontinue);
    if (ARMAVLINK_OK != error)
    {
        goto finish;
    }

    ARMAVLINK_ListUtils_MissionItemListAdd(missionItemList, &missionItem);

finish:
    return error;
}

