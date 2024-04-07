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

PWD := $(abspath $(shell pwd))
UNAME := $(shell uname)
ifneq ($(findstring $(UNAME),Linux),)
HCGEN_PATH := linux-x86/bin/hc-gen
else
HCGEN_PATH := win-x86/bin/hc-gen.exe
endif

HAVE_VENDOR_CONFIG := $(shell if [ -d $(LITEOS_SOURCE_ROOT)/vendor/$(patsubst "%",%,$(LOSCFG_DEVICE_COMPANY))/$(patsubst "%",%,$(LOSCFG_PRODUCT_NAME))/config ]; then echo y; else echo n; fi)

ifeq ($(LOCAL_HCS_ROOT),)
ifeq ($(HAVE_VENDOR_CONFIG), y)
LOCAL_HCS_ROOT := $(abspath $(LITEOSTOPDIR)/../../vendor/$(patsubst "%",%,$(LOSCFG_DEVICE_COMPANY))/$(patsubst "%",%,$(LOSCFG_PRODUCT_NAME))/config)
else
LOCAL_HCS_ROOT := $(abspath $(LITEOSTOPDIR)/../../device/$(patsubst "%",%,$(LOSCFG_DEVICE_COMPANY))/$(patsubst "%",%,$(LOSCFG_PRODUCT_NAME))/config)
endif
endif

HC_GEN := hc-gen
BUILD_IN_HC_GEN := $(LITEOSTOPDIR)/../../prebuilts/build-tools/$(HCGEN_PATH)
ifneq ($(wildcard $(BUILD_IN_HC_GEN)),)
HC_GEN := $(BUILD_IN_HC_GEN)
endif

SOURCE_ROOT := $(abspath $(LITEOSTOPDIR)/../../)
HDF_CONFIG_DIR := $(LOCAL_HCS_ROOT)
OBJOUT := $(BUILD)$(dir $(subst $(SOURCE_ROOT),,$(shell pwd)))$(MODULE_NAME)
LOCAL_CFLAGS += $(LITEOS_GCOV_OPTS)
LOCAL_CFLAGS += $(addprefix -I ,$(LOCAL_INCLUDE))

HCS_SRCS:= $(addprefix $(HDF_CONFIG_DIR)/,$(LOCAL_HCS_SRCS))
CONFIG_OUT_DIR := $(OBJOUT)/config/
CONFIG_GEN_SRCS := $(addsuffix .c,$(addprefix $(CONFIG_OUT_DIR),$(basename $(LOCAL_HCS_SRCS))))
CONFIG_OUT_SUBDIRS := $(dir $(CONFIG_GEN_SRCS))
DEPENDS_CONFIG_SRCS :=  $(addsuffix .c,$(addprefix $(CONFIG_OUT_DIR),$(basename $(LOCAL_DEPENDS_HCS_SRCS))))
DEPENDS_CONFIG_OUT_SUBDIRS :=  $(dir $(DEPENDS_CONFIG_SRCS))
LOCAL_INCLUDE += $(CONFIG_OUT_SUBDIRS) $(DEPENDS_CONFIG_OUT_SUBDIRS)

PLATFORM_HCS_SRC := $(wildcard $(LOCAL_PLATFORM_HCS_SRC))
CONFIG_GEN_HEX_SRC :=  $(addsuffix _hex.c,$(addprefix $(CONFIG_OUT_DIR),$(basename $(PLATFORM_HCS_SRC))))

LOCAL_CFG_OBJS := $(patsubst %.c, %.o,$(CONFIG_GEN_SRCS))
LOCAL_CFG_OBJS += $(patsubst %.c, %.o,$(CONFIG_GEN_HEX_SRC))

LOCAL_CSRCS := $(filter %.c,$(LOCAL_SRCS))
LOCAL_CPPSRCS := $(filter %.cpp,$(LOCAL_SRCS))
LOCAL_COBJS := $(patsubst %.c,$(OBJOUT)/%.o,$(LOCAL_CSRCS))
LOCAL_CPPOBJS := $(patsubst %.cpp,$(OBJOUT)/%.o,$(LOCAL_CPPSRCS))
LOCAL_OBJS := $(LOCAL_CFG_OBJS) $(LOCAL_COBJS) $(LOCAL_CPPOBJS)

HCB_FLAGS := -b -i -a

ifeq ($(LOCAL_SO), y)
LIBSO := $(OUT)/lib/lib$(MODULE_NAME).so
LIBA := $(OUT)/lib/lib$(MODULE_NAME).a
else
LIBSO :=
LIBA := $(OUT)/lib/lib$(MODULE_NAME).a
endif
LIB := $(LIBA) $(LIBSO)

all: $(LIB)

$(CONFIG_GEN_HEX_SRC):  $(CONFIG_OUT_DIR)%_hex.c: $(HDF_CONFIG_DIR)/%.hcs
	$(HIDE)echo gen hdf built-in config
	$(HIDE)if [ ! -d $(dir $@) ]; then mkdir -p $(dir $@); fi
	$(HIDE)$(HC_GEN) $(HCB_FLAGS) -o  $(subst _hex.c,,$(@)) $<

$(CONFIG_GEN_SRCS): $(CONFIG_OUT_DIR)%.c: $(HDF_CONFIG_DIR)/%.hcs
	$(HIDE)echo gen hdf driver config
	$(HIDE)if [ ! -d $(dir $@) ]; then mkdir -p $(dir $@); fi
	$(HIDE)$(HC_GEN) -t -o $@ $<

$(DEPENDS_CONFIG_SRCS): $(CONFIG_OUT_DIR)%.c: $(HDF_CONFIG_DIR)/%.hcs
	$(HIDE)if [ ! -d $(dir $@) ]; then mkdir -p $(dir $@); fi
	$(HIDE)$(HC_GEN) -t -o $@ $<
	$(HIDE)rm $@

$(LOCAL_CFG_OBJS): %.o: %.c
	$(HIDE)$(CC) $(LITEOS_CFLAGS) $(LOCAL_FLAGS) $(LOCAL_CFLAGS) -c $< -o $@
	$(HIDE)rm $<

$(LOCAL_COBJS): $(OBJOUT)/%.o: %.c | $(LOCAL_CFG_OBJS) $(DEPENDS_CONFIG_SRCS)
	$(HIDE)$(OBJ_MKDIR)
	$(HIDE)$(CC) $(LITEOS_CFLAGS) $(LOCAL_FLAGS) $(LOCAL_CFLAGS) -c $< -o $@

$(LOCAL_CPPOBJS): $(OBJOUT)/%.o: %.cpp
	$(HIDE)$(OBJ_MKDIR)
	$(HIDE)$(GPP) $(LITEOS_CXXFLAGS) $(LOCAL_FLAGS) $(LOCAL_CPPFLAGS) -c $< -o $@

$(LIBA): $(LOCAL_OBJS)
ifeq ($(OS), Linux)
	$(HIDE)$(AR) $(ARFLAGS) $@ $(LOCAL_OBJS)
else
ifeq ($(LOCAL_MODULES),)
	$(HIDE)$(AR) $(ARFLAGS) $@ $(LOCAL_OBJS)
else
	$(HIDE)for i in $(LOCAL_MODULES); do \
		pushd $(OBJOUT)/$$i 1>/dev/null; \
		$(AR) $(ARFLAGS) $@ *.o;\
		popd 1>/dev/null;\
	done
endif
endif

ifeq ($(LOCAL_SO), y)
$(LIBSO): $(LOCAL_CFG_OBJS) $(LOCAL_ALL_OBJS)
	$(HIDE)$(CC) $(LITEOS_CFLAGS) -fPIC -shared $^ -o $@
endif

clean:
	$(HIDE)$(RM) $(LIB) $(OBJOUT) $(LOCAL_GCH) *.bak *~

.PHONY: all clean

