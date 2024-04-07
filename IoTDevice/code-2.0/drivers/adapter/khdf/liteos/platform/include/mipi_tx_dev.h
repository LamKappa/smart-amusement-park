/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef MIPI_TX_DEV_H
#define MIPI_TX_DEV_H

#define CMD_MAX_NUM    4
#define LANE_MAX_NUM   4
#define MIPI_TX_DISABLE_LANE_ID (-1)
#define MIPI_TX_SET_DATA_SIZE   800
#define MIPI_TX_GET_DATA_SIZE   160


typedef enum {
    OUTPUT_MODE_CSI            = 0x0, /* csi mode */
    OUTPUT_MODE_DSI_VIDEO      = 0x1, /* dsi video mode */
    OUTPUT_MODE_DSI_CMD        = 0x2, /* dsi command mode */
    OUTPUT_MODE_BUTT
} OutPutModeTag;

typedef enum {
    BURST_MODE                      = 0x0,
    NON_BURST_MODE_SYNC_PULSES      = 0x1,
    NON_BURST_MODE_SYNC_EVENTS      = 0x2,
    VIDEO_DATA_MODE_BUTT
} VideoModeTag;

typedef enum {
    OUT_FORMAT_RGB_16_BIT          = 0x0,
    OUT_FORMAT_RGB_18_BIT          = 0x1,
    OUT_FORMAT_RGB_24_BIT          = 0x2,
    OUT_FORMAT_YUV420_8_BIT_NORMAL = 0x3,
    OUT_FORMAT_YUV420_8_BIT_LEGACY = 0x4,
    OUT_FORMAT_YUV422_8_BIT        = 0x5,
    OUT_FORMAT_BUTT
} OutputFormatTag;

typedef struct {
    unsigned short  vidPktSize;
    unsigned short  vidHsaPixels;
    unsigned short  vidHbpPixels;
    unsigned short  vidHlinePixels;
    unsigned short  vidVsaLines;
    unsigned short  vidVbpLines;
    unsigned short  vidVfpLines;
    unsigned short  vidActiveLines;
    unsigned short  edpiCmdSize;
} SyncInfoTag;

typedef struct {
    unsigned int    devno;                /* device number */
    short           laneId[LANE_MAX_NUM]; /* lane_id: -1 - disable */
    OutPutModeTag   outputMode;           /* output mode: CSI/DSI_VIDEO/DSI_CMD */
    VideoModeTag    videoMode;
    OutputFormatTag outputFormat;
    SyncInfoTag     syncInfo;
    unsigned int    phyDataRate;          /* mbps */
    unsigned int    pixelClk;             /* KHz */
} ComboDevCfgTag;

typedef struct {
    unsigned int    devno;                /* device number */
    unsigned short  dataType;
    unsigned short  cmdSize;
    unsigned char   *cmd;
} CmdInfoTag;

typedef struct {
    unsigned int    devno;                 /* device number */
    unsigned short  dataType;              /* DSI data type */
    unsigned short  dataParam;             /* data param,low 8 bit:1st param.high 8 bit:2st param, set 0 if not use */
    unsigned short  getDataSize;           /* read data size */
    unsigned char   *getData;              /* read data memery address, should  malloc by user */
} GetCmdInfoTag;

#endif
