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

import os
import shutil
import time
from dataclasses import dataclass

from xdevice import DeviceTestType
from xdevice import IDriver
from xdevice import Plugin
from xdevice import platform_logger

from core.config.config_manager import UserConfigManager

__all__ = ["LiteUnitTest"]


def get_level_para_string(level_string):
    level_list = list(set(level_string.split(",")))
    level_para_string = ""
    for item in level_list:
        if not item.isdigit():
            continue
        item = item.strip(" ")
        level_para_string += ("Level%s," % item)
    level_para_string = level_para_string.strip(",")
    return level_para_string


@dataclass
class GTestConst(object):
    exec_para_filter = "--gtest_filter"
    exec_para_level = "--gtest_testsize"


@Plugin(type=Plugin.DRIVER, id=DeviceTestType.lite_cpp_test)
class LiteUnitTest(IDriver):
    """
    lite gtest test driver for L1
    """
    config = None
    log = platform_logger("LiteUnitTest")
    nfs_dir = ""
    lite_device = None
    result = None

    def __check_failed__(self, msg):
        self.log.error("check failed {}".format(msg))
        return None

    def __check_environment__(self, device_options):
        pass

    def __check_config__(self, config):
        """
        1. check serial protocol
        2. login device
        3. NFS is available
        :param config: serial device
        :return:
        """
        self.log.error("Lite driver check config:{}".format(config))

    def __execute__(self, request):
        """

        1. select test case by subsystem, module, suite
        2. open test dir
        3、execute single test case, eg. ./test_demo
        :param request: contains test condition, sub_system
            module_name，test_suit，
        test_case，test_level，test_case_dir
        :return:
        """
        self.log.debug("Test suite FilePath: %s" %
                      request.root.source.source_file)
        self.lite_device = request.config.environment.devices[0]
        self.lite_device.connect()
        if not self._before_execute_test():
            self.log.error("open test dir failed")
            return
        self.log.debug("open test dir success")
        if self._execute_test(request) == "":
            self.log.error("execute test command failed")
            return
        self.log.info("execute test command success")
        if not self._after_execute_test(request):
            self.log.error("after execute test failed")
            return
        self.log.info("lite device execute request success")

    def _before_execute_test(self):
        """
        need copy test case to nfs dir
        :param request: nfs dir, test case path
        :return:
        """
        self.nfs_dir = \
            UserConfigManager().get_user_config("NFS").get("host_dir")
        if self.nfs_dir == "":
            self.log.error("no configure for nfs directory")
            return False
        _, status, _ = \
            self.lite_device.execute_command_with_timeout("cd /{}".format(
                UserConfigManager().get_user_config("NFS").get("board_dir")),
            case_type=DeviceTestType.lite_cpp_test)
        if not status:
            self.log.error("pre execute command failed")
            return False
        self.log.info("pre execute command success")
        return True

    def _execute_test(self, request):
        test_case = request.root.source.source_file
        self.config = request.config
        test_para = self._get_test_para(self.config.testcase,
                                       self.config.testlevel)
        case_name = os.path.basename(test_case)
        if os.path.exists(os.path.join(self.nfs_dir, case_name)):
            os.remove(os.path.join(self.nfs_dir, case_name))
        result_name = case_name + ".xml"
        result_file = os.path.join(self.nfs_dir, result_name)
        if os.path.exists(result_file):
            os.remove(result_file)
        shutil.copyfile(test_case, os.path.join(self.nfs_dir, case_name))
        self.lite_device.execute_command_with_timeout(
            "chmod 777 {}".format(case_name),
            case_type=DeviceTestType.lite_cpp_test)
        test_command = "./%s %s" % (case_name, test_para)
        case_result, status, _ = \
            self.lite_device.execute_command_with_timeout(
            test_command, case_type=DeviceTestType.lite_cpp_test)
        if status:
            self.log.info("test case result:\n %s" % case_result)
            return
        self.log.error("failed case: %s" % test_case)

    def _get_test_para(self, testcase, testlevel):
        if "" != testcase and "" == testlevel:
            test_para = "%s=%s" % (GTestConst.exec_para_filter, testcase)
        elif "" == testcase and "" != testlevel:
            level_para = get_level_para_string(testlevel)
            test_para = "%s=%s" % (GTestConst.exec_para_level, level_para)
        else:
            test_para = ""
        return test_para

    def _after_execute_test(self, request):
        """
        copy test result to result dir
        :param request:
        :return:
        """
        if request.config is None:
            self.log.error("test config is null")
            return False
        report_path = request.config.report_path
        test_result = os.path.join(report_path, "result")
        test_case = request.root.source.source_file
        case_name = os.path.basename(test_case)
        if not os.path.exists(test_result):
            os.mkdir(test_result)
        sub_system_module = test_case.split(
            "unittest" + os.sep)[1].split(os.sep + "bin")[0]
        if os.sep in sub_system_module:
            sub_system = sub_system_module.split(os.sep)[0]
            module_name = sub_system_module.split(os.sep)[1]
            subsystem_dir = os.path.join(test_result, sub_system)
            if not os.path.exists(subsystem_dir):
                os.mkdir(subsystem_dir)
            module_dir = os.path.join(subsystem_dir, module_name)
            if not os.path.exists(module_dir):
                os.mkdir(module_dir)
            test_result = module_dir
        else:
            if sub_system_module != "":
                test_result = os.path.join(test_result, sub_system_module)
                if not os.path.exists(test_result):
                    os.mkdir(test_result)
        result_name = case_name + ".xml"
        result_file = os.path.join(self.nfs_dir, result_name)
        if not self._check_xml_exist(result_name):
            self.log.error("result xml file %s not exist." % result_name)
        if not os.path.exists(result_file):
            self.log.error("file %s not exist." % result_file)
            return False
        file_name = os.path.basename(result_file)
        final_result = os.path.join(test_result, file_name)
        shutil.copyfile(result_file,
                        final_result)
        self.log.info("after execute test")
        self.lite_device.close()
        return True

    def _check_xml_exist(self, xml_file, timeout=10):
        ls_command = \
            "ls /%s" % \
            UserConfigManager().get_user_config("NFS").get("board_dir")
        start_time = time.time()
        while time.time()-start_time < timeout:
            result, _, _ = self.lite_device.execute_command_with_timeout(
                command=ls_command, case_type=DeviceTestType.cpp_test_lite,
                timeout=5, receiver=None)
            if xml_file in result:
                return True
            time.sleep(1)
        return False

    def show_help_info(self):
        """
        show help info.
        """
        self.log.info("this is test driver for cpp test")
        return None

    def show_driver_info(self):
        """
        show driver info.
        """
        self.log.info("this is test driver for cpp test")
        return None

    def __result__(self):
        pass
