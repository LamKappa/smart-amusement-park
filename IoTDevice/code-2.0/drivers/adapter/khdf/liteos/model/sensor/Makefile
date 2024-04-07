# Copyright (c) 2020-2021 Huawei Device Co., Ltd. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without modification,
# are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice, this list of
#    conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright notice, this list
#    of conditions and the following disclaimer in the documentation and/or other materials
#    provided with the distribution.
#
# 3. Neither the name of the copyright holder nor the names of its contributors may be used
#    to endorse or promote products derived from this software without specific prior written
#    permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
# THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
# PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
# CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
# PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
# OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
# WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
# OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
# ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

include $(LITEOSTOPDIR)/../../drivers/adapter/khdf/liteos/lite.mk

MODULE_NAME := hdf_sensor_driver

FRAMEWORKS_SENSOR_ROOT = $(LITEOSTOPDIR)/../../drivers/framework/model/sensor/driver

LOCAL_INCLUDE := $(FRAMEWORKS_SENSOR_ROOT)/include \
                 $(FRAMEWORKS_SENSOR_ROOT)/common/include \
                 $(FRAMEWORKS_SENSOR_ROOT)/accel \
                 $(FRAMEWORKS_SENSOR_ROOT)/chipset/accel

LOCAL_SRCS += $(FRAMEWORKS_SENSOR_ROOT)/common/src/sensor_common.c \
              $(FRAMEWORKS_SENSOR_ROOT)/common/src/sensor_device_common.c \
              $(FRAMEWORKS_SENSOR_ROOT)/common/src/sensor_device_manager.c \
              $(FRAMEWORKS_SENSOR_ROOT)/common/src/sensor_parser.c

ifeq ($(LOSCFG_DRIVERS_HDF_SENSOR_ACCEL), y)
LOCAL_SRCS += $(FRAMEWORKS_SENSOR_ROOT)/accel/sensor_accel_driver.c \
              $(FRAMEWORKS_SENSOR_ROOT)/chipset/accel/accel_bmi160.c
endif

include $(HDF_DRIVER)
