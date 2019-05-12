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
 * @file ARMAVLINK_ListUtils.h
 * @brief ARMavlink library List Utils
 * @date 14/05/2014
 * @author djavan.bertrand@parrot.com
 */
#ifndef _ARMAVLINK_LIST_UTILS_PRIVATE_H
#define _ARMAVLINK_LIST_UTILS_PRIVATE_H

#include <libARMavlink/ARMAVLINK_ListUtils.h>
#include <libARMavlink/ARMAVLINK_Error.h>
#include <mavlink/parrot/mavlink.h>

/**
 * @brief Add a mission item in the list
 * @param list : a pointer on the list
 * @param[in] missionItem : a pointer on the mission item to add
 * @retval the index of the place in the list where the item has been added
 * @warning this function copies the point. Also, if the memory allocated for the list is full, it reallocates memory to add more points
 */
uint16_t ARMAVLINK_ListUtils_MissionItemListAdd(mission_item_list_t *list, const mavlink_mission_item_t *const missionItem);

/**
 * @brief delete a point in the list. Replace all following mission items to their indexes - 1
 * @param list : the pointer on the list
 * @param[in] index : index of the mission item to delete
 * @return ARMAVLINK_OK if the deletion went well, the enum description of the error
 */
eARMAVLINK_ERROR ARMAVLINK_ListUtils_MissionItemListDeleteMissionItem(mission_item_list_t *list, const uint16_t index);

/**
 * @brief insert a point in the list
 * @param list : the pointer on the list
 * @param[in] missionItem : the mission item to insert
 * @param[in] index : index of where to insert the mission item
 * @return ARMAVLINK_OK if the insertion went well, the enum description of the error
 * @warning this function copies the point. Also, if the memory allocated for the list is full, it reallocates memory to add more points
 */
eARMAVLINK_ERROR ARMAVLINK_ListUtils_MissionItemListInsertMissionItem(mission_item_list_t *list, const mavlink_mission_item_t *const missionItem, const uint16_t index);

/**
 * @brief replace a mission item in the list with another
 * @param list :the pointer on the list
 * @param[in] missionItem : the new mission item which will replace the old one
 * @param[in] index : index of mission item to replace
 * @return ARMAVLINK_OK if the replacement went well, the enum description of the error
 * @warning this function copies the item.
 */
eARMAVLINK_ERROR ARMAVLINK_ListUtils_MissionItemListReplaceMissionItem(mission_item_list_t *list, const mavlink_mission_item_t *const missionItem, const uint16_t index);

#endif
