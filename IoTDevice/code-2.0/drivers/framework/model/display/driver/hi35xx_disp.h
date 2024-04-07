/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HI35XX_DISP_H
#define HI35XX_DISP_H
#include "hdf_base.h"
#include "lcd_abs_if.h"

/* output interface type */
#define INTF_LCD_6BIT               (0x01L << 9)
#define INTF_LCD_8BIT               (0x01L << 10)
#define INTF_LCD_16BIT              (0x01L << 11)
#define INTF_LCD_18BIT              (0x01L << 12)
#define INTF_LCD_24BIT              (0x01L << 13)
#define INTF_MIPI                   (0x01L << 14)

#define IO_CFG1_BASE                0x111F0000
#define IO_CFG2_BASE                0x112F0000
#define IO_CFG_SIZE                 0x10000

#define PWM_DEV0                    0
#define PWM_DEV1                    1

struct PanelData *GetPanelData(int32_t index);
struct PanelInfo *GetPanelInfo(int32_t index);
#endif /* HI35XX_DISP_H */
