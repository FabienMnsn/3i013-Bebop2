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
 * @file ARMAVLINK_MissionItemUtils.c
 * @brief ARMavlink library Utils about mission items
 * @date 14/05/2014
 * @author djavan.bertrand@parrot.com
 */
#include <stdlib.h>
#include "ARMAVLINK_MissionItemUtils.h"
#include <libARSAL/ARSAL_Print.h>

#define ARMAVLINK_MISSION_ITEM_UTILS_TAG    "ARMAVLINK_MissionItemUtil"

// default mission item values
#define ARMAVLINK_MISSION_ITEM_UTILS_EMPTY_PARAM                        0
#define ARMAVLINK_MISSION_ITEM_UTILS_DEFAULT_SEQ                        0
#define ARMAVLINK_MISSION_ITEM_UTILS_DEFAULT_CURRENT                    0
#define ARMAVLINK_MISSION_ITEM_UTILS_DEFAULT_TARGET_SYSTEM              1
#define ARMAVLINK_MISSION_ITEM_UTILS_DEFAULT_TARGET_COMPONENT           1
#define ARMAVLINK_MISSION_ITEM_UTILS_DEFAULT_AUTOCONTINUE               1
#define ARMAVLINK_MISSION_ITEM_UTILS_DEFAULT_POSITION_REFENTIAL MAV_FRAME_GLOBAL_RELATIVE_ALT // in common.h


// default MAV_CMD_NAV_WAYPOINT values
#define ARMAVLINK_MISSION_ITEM_UTILS_WAYPOINT_DEFAULT_RADIUS            5.0f    // in meters
#define ARMAVLINK_MISSION_ITEM_UTILS_WAYPOINT_DEFAULT_TIME              0.0f    // in ms
#define ARMAVLINK_MISSION_ITEM_UTILS_WAYPOINT_DEFAULT_ORBIT             0.0f

int ARMAVLINK_MissionItemUtils_MissionItemsAreEquals(const mavlink_mission_item_t *const missionItem1, const mavlink_mission_item_t *const missionItem2)
{
    int equal = 0;
    
    if ((missionItem1->param1 == missionItem2->param1) &&
        (missionItem1->param2 == missionItem2->param2) &&
        (missionItem1->param3 == missionItem2->param3) &&
        (missionItem1->param4 == missionItem2->param4) &&
        (missionItem1->x == missionItem2->x) &&
        (missionItem1->y == missionItem2->y) &&
        (missionItem1->z == missionItem2->z) &&
        (missionItem1->seq == missionItem2->seq) &&
        (missionItem1->command == missionItem2->command) &&
        (missionItem1->target_system == missionItem2->target_system) &&
        (missionItem1->target_component == missionItem2->target_component) &&
        (missionItem1->frame == missionItem2->frame) &&
        (missionItem1->current == missionItem2->current) &&
        (missionItem1->autocontinue == missionItem2->autocontinue))
    {
        equal = 1;
    }
    
    return equal;
    
}

eARMAVLINK_ERROR ARMAVLINK_MissionItemUtils_CopyMavlinkMissionItem(mavlink_mission_item_t * missionItemCpy, const mavlink_mission_item_t *const missionItem)
{
    eARMAVLINK_ERROR error = ARMAVLINK_OK;
    
    if ((missionItem == NULL) || (missionItemCpy == NULL))
    {
        error = ARMAVLINK_ERROR_BAD_PARAMETER;
    }
    
    if (error == ARMAVLINK_OK)
    {
        memcpy(missionItemCpy, missionItem, sizeof(mavlink_mission_item_t));
    }
    
    return error;
}

eARMAVLINK_ERROR ARMAVLINK_MissionItemUtils_CreateMavlinkMissionItemWithAllParams(mavlink_mission_item_t* missionItem, float param1, float param2, float param3, float param4,  float latitude, float longitude, float altitude, int command, int seq, int frame, int current, int autocontinue)
{
    eARMAVLINK_ERROR error = ARMAVLINK_OK;
    
    if (missionItem != NULL)
    {
        missionItem->param1            = param1;
        missionItem->param2            = param2;
        missionItem->param3            = param3;
        missionItem->param4            = param4;
        missionItem->x                 = latitude;
        missionItem->y                 = longitude;
        missionItem->z                 = altitude;
        missionItem->seq               = seq;
        missionItem->command           = command;
        missionItem->target_system     = ARMAVLINK_MISSION_ITEM_UTILS_DEFAULT_TARGET_SYSTEM,
        missionItem->target_component  = ARMAVLINK_MISSION_ITEM_UTILS_DEFAULT_TARGET_COMPONENT;
        missionItem->frame             = frame;
        missionItem->current           = current;
        missionItem->autocontinue      = autocontinue;
    }
    else
    {
        error = ARMAVLINK_ERROR_BAD_PARAMETER;
    }
    
    return error;
}

/* ******************* Specific commands ***************** */

/* MAV_CMD_NAV_WAYPOINT */
eARMAVLINK_ERROR ARMAVLINK_MissionItemUtils_CreateMavlinkNavWaypointMissionItem(mavlink_mission_item_t* missionItem,float latitude, float longitude, float altitude, float yaw)
{
    return ARMAVLINK_MissionItemUtils_CreateMavlinkMissionItemWithAllParams(
            missionItem,
            ARMAVLINK_MISSION_ITEM_UTILS_WAYPOINT_DEFAULT_TIME,
            ARMAVLINK_MISSION_ITEM_UTILS_WAYPOINT_DEFAULT_RADIUS,
            ARMAVLINK_MISSION_ITEM_UTILS_WAYPOINT_DEFAULT_ORBIT,
            yaw,
            latitude,
            longitude,
            altitude,
            MAV_CMD_NAV_WAYPOINT,
            ARMAVLINK_MISSION_ITEM_UTILS_DEFAULT_SEQ,
            ARMAVLINK_MISSION_ITEM_UTILS_DEFAULT_POSITION_REFENTIAL,
            ARMAVLINK_MISSION_ITEM_UTILS_DEFAULT_CURRENT,
            ARMAVLINK_MISSION_ITEM_UTILS_DEFAULT_AUTOCONTINUE
    );
}

eARMAVLINK_ERROR ARMAVLINK_MissionItemUtils_CreateMavlinkNavWaypointMissionItemWithRadius(mavlink_mission_item_t* missionItem,  float latitude, float longitude, float altitude, float radius)
{
    return ARMAVLINK_MissionItemUtils_CreateMavlinkMissionItemWithAllParams(
            missionItem,
            ARMAVLINK_MISSION_ITEM_UTILS_WAYPOINT_DEFAULT_TIME,
            ARMAVLINK_MISSION_ITEM_UTILS_WAYPOINT_DEFAULT_RADIUS,
            radius,
            ARMAVLINK_MISSION_ITEM_UTILS_EMPTY_PARAM,
            latitude,
            longitude,
            altitude,
            MAV_CMD_NAV_WAYPOINT,
            ARMAVLINK_MISSION_ITEM_UTILS_DEFAULT_SEQ,
            ARMAVLINK_MISSION_ITEM_UTILS_DEFAULT_POSITION_REFENTIAL,
            ARMAVLINK_MISSION_ITEM_UTILS_DEFAULT_CURRENT,
            ARMAVLINK_MISSION_ITEM_UTILS_DEFAULT_AUTOCONTINUE
    );
}

/* MAV_CMD_NAV_LAND */
eARMAVLINK_ERROR ARMAVLINK_MissionItemUtils_CreateMavlinkLandMissionItem(mavlink_mission_item_t* missionItem,  float latitude, float longitude, float altitude, float yaw)
{
    return ARMAVLINK_MissionItemUtils_CreateMavlinkMissionItemWithAllParams(
            missionItem,
            ARMAVLINK_MISSION_ITEM_UTILS_EMPTY_PARAM,
            ARMAVLINK_MISSION_ITEM_UTILS_EMPTY_PARAM,
            ARMAVLINK_MISSION_ITEM_UTILS_EMPTY_PARAM,
            yaw,
            latitude,
            longitude,
            altitude,
            MAV_CMD_NAV_LAND,
            ARMAVLINK_MISSION_ITEM_UTILS_DEFAULT_SEQ,
            ARMAVLINK_MISSION_ITEM_UTILS_DEFAULT_POSITION_REFENTIAL,
            ARMAVLINK_MISSION_ITEM_UTILS_DEFAULT_CURRENT,
            ARMAVLINK_MISSION_ITEM_UTILS_DEFAULT_AUTOCONTINUE
    );
}

/* MAV_CMD_NAV_TAKEOFF */
eARMAVLINK_ERROR ARMAVLINK_MissionItemUtils_CreateMavlinkTakeoffMissionItem(mavlink_mission_item_t* missionItem,  float latitude, float longitude, float altitude, float yaw, float pitch)
{
    return ARMAVLINK_MissionItemUtils_CreateMavlinkMissionItemWithAllParams(
            missionItem,
            pitch,
            ARMAVLINK_MISSION_ITEM_UTILS_EMPTY_PARAM,
            ARMAVLINK_MISSION_ITEM_UTILS_EMPTY_PARAM,
            yaw,
            latitude,
            longitude,
            altitude,
            MAV_CMD_NAV_TAKEOFF,
            ARMAVLINK_MISSION_ITEM_UTILS_DEFAULT_SEQ,
            ARMAVLINK_MISSION_ITEM_UTILS_DEFAULT_POSITION_REFENTIAL,
            ARMAVLINK_MISSION_ITEM_UTILS_DEFAULT_CURRENT,
            ARMAVLINK_MISSION_ITEM_UTILS_DEFAULT_AUTOCONTINUE
    );
}

/* MAV_CMD_NAV_DO_CHANGE_SPEED */
eARMAVLINK_ERROR ARMAVLINK_MissionItemUtils_CreateMavlinkChangeSpeedMissionItem(mavlink_mission_item_t* missionItem, int groundSpeed, float speed, float throttle)
{
    eARMAVLINK_ERROR error = ARMAVLINK_OK;
    if ((groundSpeed != 0) && (groundSpeed != 1))
    {
        error = ARMAVLINK_ERROR_BAD_PARAMETER;
    }
    
    if (error == ARMAVLINK_OK)
    {
        error = ARMAVLINK_MissionItemUtils_CreateMavlinkMissionItemWithAllParams(
                missionItem,
                groundSpeed,
                speed,
                throttle,
                ARMAVLINK_MISSION_ITEM_UTILS_EMPTY_PARAM,
                ARMAVLINK_MISSION_ITEM_UTILS_EMPTY_PARAM,
                ARMAVLINK_MISSION_ITEM_UTILS_EMPTY_PARAM,
                ARMAVLINK_MISSION_ITEM_UTILS_EMPTY_PARAM,
                MAV_CMD_DO_CHANGE_SPEED,
                ARMAVLINK_MISSION_ITEM_UTILS_DEFAULT_SEQ,
                ARMAVLINK_MISSION_ITEM_UTILS_DEFAULT_POSITION_REFENTIAL,
                ARMAVLINK_MISSION_ITEM_UTILS_DEFAULT_CURRENT,
                ARMAVLINK_MISSION_ITEM_UTILS_DEFAULT_AUTOCONTINUE
        );
    }
    
    return error;
}

/* MAV_CMD_VIDEO_START_CAPTURE */
eARMAVLINK_ERROR ARMAVLINK_MissionItemUtils_CreateMavlinkVideoStartCapture(mavlink_mission_item_t* missionItem, int cameraId, float framesPerSeconds, float resolution)
{
    return ARMAVLINK_MissionItemUtils_CreateMavlinkMissionItemWithAllParams(
            missionItem,
            cameraId,
            framesPerSeconds,
            resolution,
            ARMAVLINK_MISSION_ITEM_UTILS_EMPTY_PARAM,
            ARMAVLINK_MISSION_ITEM_UTILS_EMPTY_PARAM,
            ARMAVLINK_MISSION_ITEM_UTILS_EMPTY_PARAM,
            ARMAVLINK_MISSION_ITEM_UTILS_EMPTY_PARAM,
            MAV_CMD_VIDEO_START_CAPTURE,
            ARMAVLINK_MISSION_ITEM_UTILS_DEFAULT_SEQ,
            ARMAVLINK_MISSION_ITEM_UTILS_DEFAULT_POSITION_REFENTIAL,
            ARMAVLINK_MISSION_ITEM_UTILS_DEFAULT_CURRENT,
            ARMAVLINK_MISSION_ITEM_UTILS_DEFAULT_AUTOCONTINUE
    );
}

/* MAV_CMD_VIDEO_STOP_CAPTURE */
eARMAVLINK_ERROR ARMAVLINK_MissionItemUtils_CreateMavlinkVideoStopCapture(mavlink_mission_item_t* missionItem)
{
    return ARMAVLINK_MissionItemUtils_CreateMavlinkMissionItemWithAllParams(
            missionItem,
            ARMAVLINK_MISSION_ITEM_UTILS_EMPTY_PARAM,
            ARMAVLINK_MISSION_ITEM_UTILS_EMPTY_PARAM,
            ARMAVLINK_MISSION_ITEM_UTILS_EMPTY_PARAM,
            ARMAVLINK_MISSION_ITEM_UTILS_EMPTY_PARAM,
            ARMAVLINK_MISSION_ITEM_UTILS_EMPTY_PARAM,
            ARMAVLINK_MISSION_ITEM_UTILS_EMPTY_PARAM,
            ARMAVLINK_MISSION_ITEM_UTILS_EMPTY_PARAM,
            MAV_CMD_VIDEO_STOP_CAPTURE,
            ARMAVLINK_MISSION_ITEM_UTILS_DEFAULT_SEQ,
            ARMAVLINK_MISSION_ITEM_UTILS_DEFAULT_POSITION_REFENTIAL,
            ARMAVLINK_MISSION_ITEM_UTILS_DEFAULT_CURRENT,
            ARMAVLINK_MISSION_ITEM_UTILS_DEFAULT_AUTOCONTINUE
    );
}

/* MAV_CMD_IMAGE_START_CAPTURE */
eARMAVLINK_ERROR ARMAVLINK_MissionItemUtils_CreateMavlinkImageStartCapture(mavlink_mission_item_t* missionItem,float period,float imagesCount,float resolution)
{
    return ARMAVLINK_MissionItemUtils_CreateMavlinkMissionItemWithAllParams(
            missionItem,
            period,
            imagesCount,
            resolution,
            ARMAVLINK_MISSION_ITEM_UTILS_EMPTY_PARAM,
            ARMAVLINK_MISSION_ITEM_UTILS_EMPTY_PARAM,
            ARMAVLINK_MISSION_ITEM_UTILS_EMPTY_PARAM,
            ARMAVLINK_MISSION_ITEM_UTILS_EMPTY_PARAM,
            MAV_CMD_IMAGE_START_CAPTURE,
            ARMAVLINK_MISSION_ITEM_UTILS_DEFAULT_SEQ,
            ARMAVLINK_MISSION_ITEM_UTILS_DEFAULT_POSITION_REFENTIAL,
            ARMAVLINK_MISSION_ITEM_UTILS_DEFAULT_CURRENT,
            ARMAVLINK_MISSION_ITEM_UTILS_DEFAULT_AUTOCONTINUE
    );
}

/* MAV_CMD_IMAGE_STOP_CAPTURE */
eARMAVLINK_ERROR ARMAVLINK_MissionItemUtils_CreateMavlinkImageStopCapture(mavlink_mission_item_t* missionItem)
{
    return ARMAVLINK_MissionItemUtils_CreateMavlinkMissionItemWithAllParams(
            missionItem,
            ARMAVLINK_MISSION_ITEM_UTILS_EMPTY_PARAM,
            ARMAVLINK_MISSION_ITEM_UTILS_EMPTY_PARAM,
            ARMAVLINK_MISSION_ITEM_UTILS_EMPTY_PARAM,
            ARMAVLINK_MISSION_ITEM_UTILS_EMPTY_PARAM,
            ARMAVLINK_MISSION_ITEM_UTILS_EMPTY_PARAM,
            ARMAVLINK_MISSION_ITEM_UTILS_EMPTY_PARAM,
            ARMAVLINK_MISSION_ITEM_UTILS_EMPTY_PARAM,
            MAV_CMD_IMAGE_STOP_CAPTURE,
            ARMAVLINK_MISSION_ITEM_UTILS_DEFAULT_SEQ,
            ARMAVLINK_MISSION_ITEM_UTILS_DEFAULT_POSITION_REFENTIAL,
            ARMAVLINK_MISSION_ITEM_UTILS_DEFAULT_CURRENT,
            ARMAVLINK_MISSION_ITEM_UTILS_DEFAULT_AUTOCONTINUE
    );
}

/* MAV_CMD_CONDITION_DELAY */
eARMAVLINK_ERROR ARMAVLINK_MissionItemUtils_CreateMavlinkDelay(mavlink_mission_item_t* missionItem,float duration)
{
    return ARMAVLINK_MissionItemUtils_CreateMavlinkMissionItemWithAllParams(
            missionItem,
            duration,
            ARMAVLINK_MISSION_ITEM_UTILS_EMPTY_PARAM,
            ARMAVLINK_MISSION_ITEM_UTILS_EMPTY_PARAM,
            ARMAVLINK_MISSION_ITEM_UTILS_EMPTY_PARAM,
            ARMAVLINK_MISSION_ITEM_UTILS_EMPTY_PARAM,
            ARMAVLINK_MISSION_ITEM_UTILS_EMPTY_PARAM,
            ARMAVLINK_MISSION_ITEM_UTILS_EMPTY_PARAM,
            MAV_CMD_CONDITION_DELAY,
            ARMAVLINK_MISSION_ITEM_UTILS_DEFAULT_SEQ,
            ARMAVLINK_MISSION_ITEM_UTILS_DEFAULT_POSITION_REFENTIAL,
            ARMAVLINK_MISSION_ITEM_UTILS_DEFAULT_CURRENT,
            ARMAVLINK_MISSION_ITEM_UTILS_DEFAULT_AUTOCONTINUE
    );
}

/* MAV_CMD_PANORAMA_CREATE */
eARMAVLINK_ERROR ARMAVLINK_MissionItemUtils_CreateMavlinkCreatePanorama(mavlink_mission_item_t* missionItem,float horizontalAngle,float verticalAngle,float horizontalRotationSpeed,float verticalRotationSpeed)
{
    return ARMAVLINK_MissionItemUtils_CreateMavlinkMissionItemWithAllParams(
            missionItem,
            horizontalAngle,
            verticalAngle,
            horizontalRotationSpeed,
            verticalRotationSpeed,
            ARMAVLINK_MISSION_ITEM_UTILS_EMPTY_PARAM,
            ARMAVLINK_MISSION_ITEM_UTILS_EMPTY_PARAM,
            ARMAVLINK_MISSION_ITEM_UTILS_EMPTY_PARAM,
            MAV_CMD_PANORAMA_CREATE,
            ARMAVLINK_MISSION_ITEM_UTILS_DEFAULT_SEQ,
            ARMAVLINK_MISSION_ITEM_UTILS_DEFAULT_POSITION_REFENTIAL,
            ARMAVLINK_MISSION_ITEM_UTILS_DEFAULT_CURRENT,
            ARMAVLINK_MISSION_ITEM_UTILS_DEFAULT_AUTOCONTINUE
    );
}

eARMAVLINK_ERROR ARMAVLINK_MissionItemUtils_CreateMavlinkSetROI(mavlink_mission_item_t* missionItem, MAV_ROI mode, int missionIndex, int roiIndex, float latitude, float longitude, float altitude)
{
    return ARMAVLINK_MissionItemUtils_CreateMavlinkMissionItemWithAllParams(
            missionItem,
            mode,
            missionIndex,
            roiIndex,
            ARMAVLINK_MISSION_ITEM_UTILS_EMPTY_PARAM,
            latitude,
            longitude,
            altitude,
            MAV_CMD_DO_SET_ROI,
            ARMAVLINK_MISSION_ITEM_UTILS_DEFAULT_SEQ,
            ARMAVLINK_MISSION_ITEM_UTILS_DEFAULT_POSITION_REFENTIAL,
            ARMAVLINK_MISSION_ITEM_UTILS_DEFAULT_CURRENT,
            ARMAVLINK_MISSION_ITEM_UTILS_DEFAULT_AUTOCONTINUE
    );
}

eARMAVLINK_ERROR ARMAVLINK_MissionItemUtils_CreateMavlinkSetViewMode(mavlink_mission_item_t* missionItem, MAV_VIEW_MODE_TYPE type, int roiIndex)
{
    return ARMAVLINK_MissionItemUtils_CreateMavlinkMissionItemWithAllParams(
            missionItem,
            type,
            roiIndex,
            ARMAVLINK_MISSION_ITEM_UTILS_EMPTY_PARAM,
            ARMAVLINK_MISSION_ITEM_UTILS_EMPTY_PARAM,
            ARMAVLINK_MISSION_ITEM_UTILS_EMPTY_PARAM,
            ARMAVLINK_MISSION_ITEM_UTILS_EMPTY_PARAM,
            ARMAVLINK_MISSION_ITEM_UTILS_EMPTY_PARAM,
            MAV_CMD_SET_VIEW_MODE,
            ARMAVLINK_MISSION_ITEM_UTILS_DEFAULT_SEQ,
            ARMAVLINK_MISSION_ITEM_UTILS_DEFAULT_POSITION_REFENTIAL,
            ARMAVLINK_MISSION_ITEM_UTILS_DEFAULT_CURRENT,
            ARMAVLINK_MISSION_ITEM_UTILS_DEFAULT_AUTOCONTINUE
    );
}

eARMAVLINK_ERROR ARMAVLINK_MissionItemUtils_CreateMavlinkSetPictureMode(mavlink_mission_item_t* missionItem,
        MAV_STILL_CAPTURE_MODE_TYPE type, float interval) {
    return ARMAVLINK_MissionItemUtils_CreateMavlinkMissionItemWithAllParams(
            missionItem,
            type,
            interval,
            ARMAVLINK_MISSION_ITEM_UTILS_EMPTY_PARAM,
            ARMAVLINK_MISSION_ITEM_UTILS_EMPTY_PARAM,
            ARMAVLINK_MISSION_ITEM_UTILS_EMPTY_PARAM,
            ARMAVLINK_MISSION_ITEM_UTILS_EMPTY_PARAM,
            ARMAVLINK_MISSION_ITEM_UTILS_EMPTY_PARAM,
            MAV_CMD_SET_STILL_CAPTURE_MODE,
            ARMAVLINK_MISSION_ITEM_UTILS_DEFAULT_SEQ,
            ARMAVLINK_MISSION_ITEM_UTILS_DEFAULT_POSITION_REFENTIAL,
            ARMAVLINK_MISSION_ITEM_UTILS_DEFAULT_CURRENT,
            ARMAVLINK_MISSION_ITEM_UTILS_DEFAULT_AUTOCONTINUE
    );
}

eARMAVLINK_ERROR ARMAVLINK_MissionItemUtils_CreateMavlinkSetPhotoSensors(mavlink_mission_item_t* missionItem,
        uint32_t sensorsBitfield) {
    return ARMAVLINK_MissionItemUtils_CreateMavlinkMissionItemWithAllParams(
            missionItem,
            sensorsBitfield,
            ARMAVLINK_MISSION_ITEM_UTILS_EMPTY_PARAM,
            ARMAVLINK_MISSION_ITEM_UTILS_EMPTY_PARAM,
            ARMAVLINK_MISSION_ITEM_UTILS_EMPTY_PARAM,
            ARMAVLINK_MISSION_ITEM_UTILS_EMPTY_PARAM,
            ARMAVLINK_MISSION_ITEM_UTILS_EMPTY_PARAM,
            ARMAVLINK_MISSION_ITEM_UTILS_EMPTY_PARAM,
            MAV_CMD_SET_PHOTO_SENSORS,
            ARMAVLINK_MISSION_ITEM_UTILS_DEFAULT_SEQ,
            ARMAVLINK_MISSION_ITEM_UTILS_DEFAULT_POSITION_REFENTIAL,
            ARMAVLINK_MISSION_ITEM_UTILS_DEFAULT_CURRENT,
            ARMAVLINK_MISSION_ITEM_UTILS_DEFAULT_AUTOCONTINUE
    );
}
