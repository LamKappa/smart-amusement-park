#!/usr/bin/env python3
# coding=utf-8

#
# Copyright (c) 2020 Huawei Device Co., Ltd.
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

__all__ = [
    "SchedulerType",
    "ToolCommandType",
    "ConfigFileConst"
]


class SchedulerType(object):
    """
    SchedulerType enumeration
    """
    # default scheduler
    SCHEDULER = "Scheduler"
    COMBINATION = "Combination"

    @property
    def default_type(self):
        return SchedulerType.SCHEDULER

    @property
    def combination_type(self):
        return SchedulerType.COMBINATION


class ToolCommandType(object):
    TOOLCMD_KEY_HELP = "help"
    TOOLCMD_KEY_SHOW = "show"
    TOOLCMD_KEY_RUN = "run"
    TOOLCMD_KEY_QUIT = "quit"
    TOOLCMD_KEY_LIST = "list"

    @property
    def run_command(self):
        return ToolCommandType.TOOLCMD_KEY_RUN

    @property
    def help_command(self):
        return ToolCommandType.TOOLCMD_KEY_HELP


class ConfigFileConst(object):
    FRAMECONFIG_FILEPATH = "framework_config.xml"
    BUILDCONFIG_FILEPATH = "build_config.xml"
    USERCONFIG_FILEPATH = "user_config.xml"
    FILTERCONFIG_FILEPATH = "filter_config.xml"
    RESOURCECONFIG_FILEPATH = "harmony_test.xml"
    CASE_RESOURCE_FILEPATH = "ohos_test.xml"
    SCENECONFIG_FILEPATH = "scene_config.xml"

    @property
    def framework_config_file(self):
        return ConfigFileConst.FRAMECONFIG_FILEPATH

    @property
    def user_config_file(self):
        return ConfigFileConst.USERCONFIG_FILEPATH

