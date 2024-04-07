/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "gpio_if.h"
#include "hdf_device_desc.h"
#include "hdf_log.h"
#include "lcd_abs_if.h"
#include "mipi_dsi_if.h"
#include "osal.h"
#include "pwm_if.h"

#define RESET_GPIO                5
#define MIPI_DSI0                 0
#define BLK_PWM1                  1
#define PWM_MAX_PERIOD            100000
/* backlight setting */
#define MIN_LEVEL                 0
#define MAX_LEVEL                 255
#define DEFAULT_LEVEL             100

#define WIDTH                     480
#define HEIGHT                    960
#define HORIZONTAL_BACK_PORCH     20
#define HORIZONTAL_FRONT_PORCH    20
#define HORIZONTAL_SYNC_WIDTH     10
#define VERTICAL_BACK_PORCH       14
#define VERTICAL_FRONT_PORCH      16
#define VERTICAL_SYNC_WIDTH       2
#define FRAME_RATE                60

/* panel on command payload */
static uint8_t g_payLoad0[] = { 0xF0, 0x5A, 0x5A };
static uint8_t g_payLoad1[] = { 0xF1, 0xA5, 0xA5 };
static uint8_t g_payLoad2[] = { 0xB3, 0x03, 0x03, 0x03, 0x07, 0x05, 0x0D, 0x0F, 0x11, 0x13, 0x09, 0x0B };
static uint8_t g_payLoad3[] = { 0xB4, 0x03, 0x03, 0x03, 0x06, 0x04, 0x0C, 0x0E, 0x10, 0x12, 0x08, 0x0A };
static uint8_t g_payLoad4[] = { 0xB0, 0x54, 0x32, 0x23, 0x45, 0x44, 0x44, 0x44, 0x44, 0x60, 0x00, 0x60, 0x1C };
static uint8_t g_payLoad5[] = { 0xB1, 0x32, 0x84, 0x02, 0x87, 0x12, 0x00, 0x50, 0x1C };
static uint8_t g_payLoad6[] = { 0xB2, 0x73, 0x09, 0x08 };
static uint8_t g_payLoad7[] = { 0xB6, 0x5C, 0x5C, 0x05 };
static uint8_t g_payLoad8[] = { 0xB8, 0x23, 0x41, 0x32, 0x30, 0x03 };
static uint8_t g_payLoad9[] = { 0xBC, 0xD2, 0x0E, 0x63, 0x63, 0x5A, 0x32, 0x22, 0x14, 0x22, 0x03 };
static uint8_t g_payLoad10[] = { 0xb7, 0x41 };
static uint8_t g_payLoad11[] = { 0xC1, 0x0c, 0x10, 0x04, 0x0c, 0x10, 0x04 };
static uint8_t g_payLoad12[] = { 0xC2, 0x10, 0xE0 };
static uint8_t g_payLoad13[] = { 0xC3, 0x22, 0x11 };
static uint8_t g_payLoad14[] = { 0xD0, 0x07, 0xFF };
static uint8_t g_payLoad15[] = { 0xD2, 0x63, 0x0B, 0x08, 0x88 };
static uint8_t g_payLoad16[] = { 0xC6, 0x08, 0x15, 0xFF, 0x10, 0x16, 0x80, 0x60 };
static uint8_t g_payLoad17[] = { 0xc7, 0x04 };
static uint8_t g_payLoad18[] = {
    0xC8, 0x7C, 0x50, 0x3B, 0x2C, 0x25, 0x16, 0x1C, 0x08, 0x27, 0x2B, 0x2F, 0x52, 0x43, 0x4C, 0x40,
    0x3D, 0x30, 0x1E, 0x06, 0x7C, 0x50, 0x3B, 0x2C, 0x25, 0x16, 0x1C, 0x08, 0x27, 0x2B, 0x2F, 0x52,
    0x43, 0x4C, 0x40, 0x3D, 0x30, 0x1E, 0x06
};
static uint8_t g_payLoad19[] = { 0x11 };
static uint8_t g_payLoad20[] = { 0x29 };

struct DsiCmdDesc g_OnCmd[] = {
    { 0x29, 0, sizeof(g_payLoad0), g_payLoad0 },
    { 0x29, 0, sizeof(g_payLoad1), g_payLoad1 },
    { 0x29, 0, sizeof(g_payLoad2), g_payLoad2 },
    { 0x29, 0, sizeof(g_payLoad3), g_payLoad3 },
    { 0x29, 0, sizeof(g_payLoad4), g_payLoad4 },
    { 0x29, 0, sizeof(g_payLoad5), g_payLoad5 },
    { 0x29, 0, sizeof(g_payLoad6), g_payLoad6 },
    { 0x29, 0, sizeof(g_payLoad7), g_payLoad7 },
    { 0x29, 0, sizeof(g_payLoad8), g_payLoad8 },
    { 0x29, 0, sizeof(g_payLoad9), g_payLoad9 },
    { 0x23, 0, sizeof(g_payLoad10), g_payLoad10 },
    { 0x29, 0, sizeof(g_payLoad11), g_payLoad11 },
    { 0x29, 0, sizeof(g_payLoad12), g_payLoad12 },
    { 0x29, 0, sizeof(g_payLoad13), g_payLoad13 },
    { 0x29, 0, sizeof(g_payLoad14), g_payLoad14 },
    { 0x29, 0, sizeof(g_payLoad15), g_payLoad15 },
    { 0x29, 0, sizeof(g_payLoad16), g_payLoad16 },
    { 0x23, 0, sizeof(g_payLoad17), g_payLoad17 },
    { 0x29, 1, sizeof(g_payLoad18), g_payLoad18 },
    { 0x05, 120, sizeof(g_payLoad19), g_payLoad19 },
    { 0x05, 120, sizeof(g_payLoad20), g_payLoad20 },
};

/* panel off command payload */
static uint8_t g_offPayLoad0[] = { 0x28 };
static uint8_t g_offPayLoad1[] = { 0x10 };
struct DsiCmdDesc g_offCmd[] = {
    { 0x05, 20, sizeof(g_offPayLoad0), g_offPayLoad0 },
    { 0x05, 120, sizeof(g_offPayLoad1), g_offPayLoad1 },
};

static DevHandle g_mipiHandle = NULL;
static DevHandle g_pwmHandle = NULL;

static int32_t LcdResetOn(void)
{
    int32_t ret;

    ret = GpioSetDir(RESET_GPIO, GPIO_DIR_OUT);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("GpioSetDir failed, ret:%d", ret);
        return HDF_FAILURE;
    }
    ret = GpioWrite(RESET_GPIO, GPIO_VAL_HIGH);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("GpioWrite failed, ret:%d", ret);
        return HDF_FAILURE;
    }
    /* delay 20ms */
    OsalMSleep(20);
    return HDF_SUCCESS;
}

static int32_t LcdResetOff(void)
{
    int32_t ret;

    ret = GpioSetDir(RESET_GPIO, GPIO_DIR_OUT);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("GpioSetDir failed, ret:%d", ret);
        return HDF_FAILURE;
    }
    ret = GpioWrite(RESET_GPIO, GPIO_VAL_LOW);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("GpioWrite failed, ret:%d", ret);
        return HDF_FAILURE;
    }
    /* delay 20ms */
    OsalMSleep(20);
    return HDF_SUCCESS;
}

static int32_t PwmCfg(void)
{
    g_pwmHandle = PwmOpen(BLK_PWM1);
    if (g_pwmHandle == NULL) {
        HDF_LOGE("%s: PwmOpen failed", __func__);
        return HDF_FAILURE;
    }
    /* pwm config */
    int32_t ret;
    struct PwmConfig config;
    ret = PwmGetConfig(g_pwmHandle, &config);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: PwmGetConfig fail, ret %d", __func__, ret);
        return HDF_FAILURE;
    }
    config.duty = 1;
    config.period = PWM_MAX_PERIOD;
    config.status = 1;
    ret = PwmSetConfig(g_pwmHandle, &config);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: PwmSetConfig fail, ret %d", __func__, ret);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

static int32_t Icn9700Init(void)
{
    g_mipiHandle = MipiDsiOpen(MIPI_DSI0);
    if (g_mipiHandle == NULL) {
        HDF_LOGE("%s: MipiDsiOpen failed", __func__);
        return HDF_FAILURE;
    }
    if (PwmCfg() != HDF_SUCCESS) {
        HDF_LOGE("%s: PwmCfg failed", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

static int32_t Icn9700On(void)
{
    int32_t ret;

    /* lcd reset power on */
    ret = LcdResetOn();
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: LcdResetOn failed", __func__);
        return HDF_FAILURE;
    }
    if (g_mipiHandle == NULL) {
        HDF_LOGE("%s: g_mipiHandle is null", __func__);
        return HDF_FAILURE;
    }
    /* send mipi init code */
    int32_t count = sizeof(g_OnCmd) / sizeof(g_OnCmd[0]);
    int32_t i;
    for (i = 0; i < count; i++) {
        ret = MipiDsiTx(g_mipiHandle, &(g_OnCmd[i]));
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%s: MipiDsiTx failed", __func__);
            return HDF_FAILURE;
        }
    }
    /* set mipi to hs mode */
    MipiDsiSetHsMode(g_mipiHandle);
    return HDF_SUCCESS;
}

static int32_t Icn9700Off(void)
{
    int32_t ret;

    if (g_mipiHandle == NULL) {
        HDF_LOGE("%s: g_mipiHandle is null", __func__);
        return HDF_FAILURE;
    }
    /* send mipi init code */
    int32_t count = sizeof(g_offCmd) / sizeof(g_offCmd[0]);
    int32_t i;
    for (i = 0; i < count; i++) {
        ret = MipiDsiTx(g_mipiHandle, &(g_offCmd[i]));
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%s: MipiDsiTx failed", __func__);
            return HDF_FAILURE;
        }
    }
    /* set mipi to lp mode */
    MipiDsiSetLpMode(g_mipiHandle);
    /* lcd reset power off */
    ret = LcdResetOff();
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: LcdResetOff failed", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

static int32_t Icn9700SetBacklight(uint32_t level)
{
    int32_t ret;
    uint32_t duty;
    duty = (level * PWM_MAX_PERIOD) / MAX_LEVEL;
    ret = PwmSetDuty(g_pwmHandle, duty);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: PwmSetDutyCycle failed, ret %d", __func__, ret);
        return HDF_FAILURE;
    }
    static uint32_t lastLevel = 0;
    if (level != 0 && lastLevel == 0) {
        ret = PwmEnable(g_pwmHandle);
    } else if (level == 0 && lastLevel != 0) {
        ret = PwmDisable(g_pwmHandle);
    }
    lastLevel = level;
    return ret;
}

static struct PanelInfo g_panelInfo = {
    .width = WIDTH,                     /* width */
    .height = HEIGHT,                   /* height */
    .hbp = HORIZONTAL_BACK_PORCH,       /* horizontal back porch */
    .hfp = HORIZONTAL_FRONT_PORCH,      /* horizontal front porch */
    .hsw = HORIZONTAL_SYNC_WIDTH,       /* horizontal sync width */
    .vbp = VERTICAL_BACK_PORCH,         /* vertical back porch */
    .vfp = VERTICAL_FRONT_PORCH,        /* vertical front porch */
    .vsw = VERTICAL_SYNC_WIDTH,         /* vertical sync width */
    .frameRate = FRAME_RATE,            /* frame rate */
    .intfType = MIPI_DSI,               /* panel interface type */
    .intfSync = OUTPUT_USER,            /* output timing type */
    /* mipi config info */
    .mipi = { DSI_2_LANES, DSI_VIDEO_MODE, VIDEO_BURST_MODE, FORMAT_RGB_24_BIT },
    /* backlight config info */
    .blk = { BLK_PWM, MIN_LEVEL, MAX_LEVEL, DEFAULT_LEVEL },
    .pwm = { BLK_PWM1, PWM_MAX_PERIOD },
};

static struct PanelStatus g_panelStatus = {
    .powerStatus = POWER_STATUS_OFF,
    .currLevel = MIN_LEVEL,
};

static struct PanelData g_panelData = {
    .info = &g_panelInfo,
    .status = &g_panelStatus,
    .init = Icn9700Init,
    .on = Icn9700On,
    .off = Icn9700Off,
    .setBacklight = Icn9700SetBacklight,
};

int32_t Icn9700EntryInit(struct HdfDeviceObject *object)
{
    if (object == NULL) {
        HDF_LOGE("%s: param is null", __func__);
        return HDF_FAILURE;
    }
    if (PanelDataRegister(&g_panelData) != HDF_SUCCESS) {
        HDF_LOGE("%s: PanelDataRegister failed", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGI("%s: exit succ", __func__);
    return HDF_SUCCESS;
}

struct HdfDriverEntry g_icn9700DevEntry = {
    .moduleVersion = 1,
    .moduleName = "LCD_ICN9700",
    .Init = Icn9700EntryInit,
};

HDF_INIT(g_icn9700DevEntry);
