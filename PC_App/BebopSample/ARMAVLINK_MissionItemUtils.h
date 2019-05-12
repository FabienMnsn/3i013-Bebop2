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
 * @file ARMAVLINK_MissionItemUtils.h
 * @brief ARMavlink library Utils
 * @date 14/05/2014
 * @author djavan.bertrand@parrot.com
 */
#ifndef _ARMAVLINK_MISSION_ITEM_UTILS_PRIVATE_H
#define _ARMAVLINK_MISSION_ITEM_UTILS_PRIVATE_H

#include <libARMavlink/ARMAVLINK_MissionItemUtils.h>
#include <libARMavlink/ARMAVLINK_Error.h>

/**
 * @brief Determine if two mission items are equals
 * @param[in] missionItem1 : one mission item to compare
 * @param[in] missionItem2 : one mission item to compare
 * @return 1 if the two mission items values are equals, 0 otherwise
 */
int ARMAVLINK_MissionItemUtils_MissionItemsAreEquals(const mavlink_mission_item_t *const missionItem1, const mavlink_mission_item_t *const missionItem2);

/**
 * @brief Copy a mavlink item in an other one
 * @param[out] missionItemCpy : the copy of the mission item. It should be allocated
 * @param[in] missionItem : the mission item to copy
 * @return ARMAVLINK_OK if operation went well, the enum description of the error otherwise
 */
eARMAVLINK_ERROR ARMAVLINK_MissionItemUtils_CopyMavlinkMissionItem(mavlink_mission_item_t * missionItemCpy, const mavlink_mission_item_t *const missionItem);


#endif
