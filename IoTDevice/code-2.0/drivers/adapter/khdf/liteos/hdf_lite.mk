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

ifeq ($(LOSCFG_DRIVERS_HDF), y)
    LITEOS_BASELIB += --whole-archive
    LITEOS_DRIVERS_HDF := $(LITEOSTOPDIR)/../../drivers/adapter/khdf/liteos
    LITEOS_SOURCE_ROOT := $(LITEOSTOPDIR)/../..
    LIB_SUBDIRS        += $(LITEOS_DRIVERS_HDF)
    HDF_FRAMEWORKS_PATH:= $(LITEOSTOPDIR)/../../drivers/framework
    LITEOS_BASELIB += -lhdf
    LITEOS_DRIVERS_HDF_INCLUDE += -I $(HDF_FRAMEWORKS_PATH)/core/common/include/manager
    LITEOS_DRIVERS_HDF_INCLUDE += -I $(HDF_FRAMEWORKS_PATH)/support/platform/include
    LITEOS_DRIVERS_HDF_INCLUDE += -I $(HDF_FRAMEWORKS_PATH)/include/platform
    LITEOS_DRIVERS_HDF_INCLUDE += -I $(HDF_FRAMEWORKS_PATH)/include/utils

# models
ifeq ($(LOSCFG_DRIVERS_HDF_WIFI), y)
    LITEOS_BASELIB += -lhdf_wifi_model
    LIB_SUBDIRS    +=  $(LITEOS_DRIVERS_HDF)/model/network/wifi
endif

ifeq ($(LOSCFG_DRIVERS_HDF_USB), y)
    LITEOS_DRIVERS_HDF_INCLUDE += -I $(LITEOS_DRIVERS_HDF)/model/bus/usb/include
    LITEOS_BASELIB += -lhdf_usb
    LIB_SUBDIRS    +=  $(LITEOS_DRIVERS_HDF)/model/bus/usb
endif

ifeq ($(LOSCFG_DRIVERS_HDF_DISP), y)
    LITEOS_BASELIB += -lhdf_display
    LIB_SUBDIRS    +=  $(LITEOS_DRIVERS_HDF)/model/display
endif

ifeq ($(LOSCFG_DRIVERS_HDF_INPUT), y)
    LITEOS_BASELIB += -lhdf_input_driver
    LIB_SUBDIRS    += $(LITEOS_DRIVERS_HDF)/model/input
endif

ifeq ($(LOSCFG_DRIVERS_HDF_SENSOR), y)
    LITEOS_BASELIB += -lhdf_sensor_driver
    LIB_SUBDIRS    += $(LITEOS_DRIVERS_HDF)/model/sensor
endif

HAVE_VENDOR_CONFIG := $(shell if [ -d $(LITEOS_SOURCE_ROOT)/vendor/$(patsubst "%",%,$(LOSCFG_DEVICE_COMPANY))/$(patsubst "%",%,$(LOSCFG_PRODUCT_NAME))/config ]; then echo y; else echo n; fi)
ifeq ($(LOSCFG_DRIVERS_HDF_TEST), y)
include $(LITEOS_DRIVERS_HDF)/test/test_lite.mk
# test
    LITEOS_BASELIB += -lhdf_test
    LIB_SUBDIRS    += $(LITEOS_DRIVERS_HDF)/test

    LITEOS_BASELIB += -lhdf_test_config
    LIB_SUBDIRS += $(LITEOS_SOURCE_ROOT)/vendor/$(LOSCFG_DEVICE_COMPANY)/$(LOSCFG_PRODUCT_NAME)/config/hdf_test
else
# config
    LITEOS_BASELIB += -lhdf_config
ifeq ($(HAVE_VENDOR_CONFIG), y)
    LIB_SUBDIRS += $(LITEOS_SOURCE_ROOT)/vendor/$(LOSCFG_DEVICE_COMPANY)/$(LOSCFG_PRODUCT_NAME)/config
else
    LIB_SUBDIRS += $(LITEOS_SOURCE_ROOT)/device/$(LOSCFG_DEVICE_COMPANY)/$(LOSCFG_PRODUCT_NAME)/config
endif
endif

# vendor lib
COMPANY_OF_SOC := $(subst $\",,$(LOSCFG_DEVICE_COMPANY))
include $(LITEOSTOPDIR)/../../device/$(COMPANY_OF_SOC)/drivers/lite.mk
    LITEOS_BASELIB += --no-whole-archive
endif

