/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

 /**
 * @addtogroup Input
 * @{
 *
 * @brief Provides driver interfaces for the input service.
 *
 * These driver interfaces can be used to open and close input device files, get input events, query device information,
 * register callback functions, and control the feature status.
 *
 * @since 1.0
 * @version 1.0
 */

 /**
 * @file input_type.h
 *
 * @brief Declares types of input devices as well as the structure and enumeration types used by driver interfaces.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef INPUT_TYPES_H
#define INPUT_TYPES_H

#include <stdint.h>
#include <stdbool.h>
#include <sys/time.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_INPUT_DEV_NUM 32
#define MAX_NODE_PATH_LEN 64
#define CHIP_INFO_LEN 10
#define CHIP_NAME_LEN 10
#define VENDOR_NAME_LEN 10
#define SELF_TEST_RESULT_LEN 20
#define DEV_MANAGER_SERVICE_NAME "hdf_input_host"

/**
 * @brief Enumerates return values.
 */
enum RetStatus {
    INPUT_SUCCESS        = 0,     /**< Success */
    INPUT_FAILURE        = -1,    /**< Failure */
    INPUT_INVALID_PARAM  = -2,    /**< Invalid parameter */
    INPUT_NOMEM          = -3,    /**< Insufficient memory */
    INPUT_NULL_PTR       = -4,    /**< Null pointer */
    INPUT_TIMEOUT        = -5,    /**< Execution timed out */
    INPUT_UNSUPPORTED    = -6,    /**< Unsupported feature */
};

/**
 * @brief Enumerates input device types.
 */
enum InputDevType {
    INDEV_TYPE_TOUCH,               /**< Touchscreen */
    INDEV_TYPE_KEY,                 /**< Physical key */
    INDEV_TYPE_BUTTON,              /**< Virtual button */
    INDEV_TYPE_CROWN,               /**< Watch crown */
    INDEV_TYPE_ENCODER,             /**< Customized type of a specific function or event */
    INDEV_TYPE_HID_BEGIN_POS = 33,  /* HID type start position */
    INDEV_TYPE_MOUSE,               /**< Mouse */
    INDEV_TYPE_KEYBOARD,            /**< Keyboard */
    INDEV_TYPE_UNKNOWN,             /**< Unknown input device type */
};

/**
 * @brief Enumerates power statuses.
 */
enum PowerStatus {
    INPUT_RESUME,                  /**< Resume status */
    INPUT_SUSPEND,                 /**< Suspend status */
    INPUT_LOW_POWER,               /**< Low-power status */
    INPUT_POWER_STATUS_UNKNOWN,    /**< Unknown power status */
};

/**
 * @brief Enumerates types of capacitance tests.
 */
enum CapacitanceTest {
    BASE_TEST,             /**< Basic capacitance test */
    FULL_TEST,             /**< Full capacitance self-test */
    MMI_TEST,              /**< Man-Machine Interface (MMI) capacitance test */
    RUNNING_TEST,          /**< Running capacitance test */
    TEST_TYPE_UNKNOWN,     /**< Unknown test type */
};

/**
 * @brief Describes the input event data package.
 */
typedef struct {
    uint32_t type;          /**< Type of the input event */
    uint32_t code;          /**< Specific code item of the input event */
    int32_t value;          /**< Value of the input event code item */
    uint64_t timestamp;     /**< Timestamp of the input event */
} EventPackage;

typedef struct {
    uint32_t devIndex;
    uint32_t devType;
    uint32_t status;
} HotPlugEvent;

typedef struct {
    uint32_t devIndex;
    uint32_t devType;
} DevDesc;

/**
 * @brief Describes the input event callback registered by the input service.
 */
typedef struct {
    /**
     * @brief Reports input event data by the registered callback.
     *
     * @param pkgs describes the input event data package.
     * @param count Indicates the number of input event data packets.
     * @param devIndex Indicates the index of an input device.
     * @since 1.0
     * @version 1.0
     */
    void (*ReportEventPkgCallback)(const EventPackage **pkgs, uint32_t count, uint32_t devIndex);

    /**
     * @brief Reports hot plug event data by the registered callback.
     *
     * @param event Indicates the pointer to the hot plug event data reported by the input driver.
     * @since 1.0
     * @version 1.0
     */
    void (*ReportHotPlugEventCallback)(const HotPlugEvent *event);
} InputReportEventCb;

/**
 * @brief Describes basic device information of the input device.
 */
typedef struct {
    uint32_t devIndex;                   /**< Device index */
    int32_t fd;                          /**< File descriptor of the device */
    void *service;                       /**< Service of the device */
    void *listener;                      /**< Event listener of the device */
    uint32_t devType;                    /**< Device type */
    uint32_t powerStatus;                /**< Power status */
    char chipInfo[CHIP_INFO_LEN];        /**< Driver chip information */
    char vendorName[VENDOR_NAME_LEN];    /**< Module vendor name */
    char chipName[CHIP_NAME_LEN];        /**< Driver chip name */
    char devNodePath[MAX_NODE_PATH_LEN]; /**< Device file path */
    uint32_t solutionX;                  /**< Resolution in the X axis */
    uint32_t solutionY;                  /**< Resolution in the Y axis */
    InputReportEventCb *callback;        /**< Callback {@link InputReportEventCb} for reporting data */
} DeviceInfo;

/**
 * @brief Describes the extra commands.
 */
typedef struct {
    const char *cmdCode;     /**< Command code */
    const char *cmdValue;    /**< Data transmitted in the command */
} InputExtraCmd;

#ifdef __cplusplus
}
#endif
#endif
/** @} */
