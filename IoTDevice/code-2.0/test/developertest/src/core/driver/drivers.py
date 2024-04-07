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
import time
import platform
from dataclasses import dataclass


from xdevice import DeviceTestType
from xdevice import DeviceLabelType
from xdevice import ExecuteTerminate
from xdevice import DeviceError

from xdevice import IDriver
from xdevice import platform_logger
from xdevice import Plugin
from core.utils import get_decode
from core.config.resource_manager import ResourceManager


__all__ = [
    "CppTestDriver",
    "disable_keyguard",
    "GTestConst"]

LOG = platform_logger("Drivers")
DEFAULT_TEST_PATH = "/%s/%s/" % ("data", "test")

TIME_OUT = 900 * 1000


##############################################################################
##############################################################################


class DisplayOutputReceiver:
    def __init__(self):
        self.output = ""
        self.unfinished_line = ""

    def _process_output(self, output, end_mark="\n"):
        content = output
        if self.unfinished_line:
            content = "".join((self.unfinished_line, content))
            self.unfinished_line = ""
        lines = content.split(end_mark)
        if content.endswith(end_mark):
            return lines[:-1]
        else:
            self.unfinished_line = lines[-1]
            return lines[:-1]

    def __read__(self, output):
        self.output = "%s%s" % (self.output, output)
        lines = self._process_output(output)
        for line in lines:
            line = line.strip()
            if line:
                LOG.info(get_decode(line))

    def __error__(self, message):
        pass

    def __done__(self, result_code="", message=""):
        pass


@dataclass
class GTestConst(object):
    exec_para_filter = "--gtest_filter"
    exec_para_level = "--gtest_testsize"


def get_device_log_file(report_path, serial=None, log_name="device_log"):
    from xdevice import Variables
    log_path = os.path.join(report_path, Variables.report_vars.log_dir)
    os.makedirs(log_path, exist_ok=True)

    serial = serial or time.time_ns()
    device_file_name = "{}_{}.log".format(log_name, serial)
    device_log_file = os.path.join(log_path, device_file_name)
    return device_log_file


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


def get_result_savepath(testsuit_path, result_rootpath):
    findkey = os.sep + "tests" + os.sep
    filedir, _ = os.path.split(testsuit_path)
    pos = filedir.find(findkey)
    if -1 != pos:
        subpath = filedir[pos + len(findkey):]
        pos1 = subpath.find(os.sep)
        if -1 != pos1:
            subpath = subpath[pos1 + len(os.sep):]
            result_path = os.path.join(result_rootpath, "result", subpath)
        else:
            result_path = os.path.join(result_rootpath, "result")
    else:
        result_path = os.path.join(result_rootpath, "result")

    if not os.path.exists(result_path):
        os.makedirs(result_path)

    LOG.info("result_savepath = " + result_path)
    return result_path


# all testsuit common Unavailable test result xml
def _create_empty_result_file(filepath, filename, error_message):
    error_message = str(error_message)
    error_message = error_message.replace("\"", "")
    error_message = error_message.replace("<", "")
    error_message = error_message.replace(">", "")
    error_message = error_message.replace("&", "")
    if filename.endswith(".hap"):
        filename = filename.split(".")[0]
    if not os.path.exists(filepath):
        with open(filepath, "w", encoding='utf-8') as file_desc:
            time_stamp = time.strftime("%Y-%m-%d %H:%M:%S",
                                       time.localtime())
            file_desc.write('<?xml version="1.0" encoding="UTF-8"?>\n')
            file_desc.write(
                '<testsuites tests="0" failures="0" '
                'disabled="0" errors="0" timestamp="%s" '
                'time="0" name="AllTests">\n' % time_stamp)
            file_desc.write(
                '  <testsuite name="%s" tests="0" failures="0" '
                'disabled="0" errors="0" time="0.0" '
                'unavailable="1" message="%s">\n' %
                (filename, error_message))
            file_desc.write('  </testsuite>\n')
            file_desc.write('</testsuites>\n')
    return


def _unlock_screen(device):
    device.execute_shell_command("svc power stayon true")
    time.sleep(1)


def _unlock_device(device):
    device.execute_shell_command("input keyevent 82")
    time.sleep(1)
    device.execute_shell_command("wm dismiss-keyguard")
    time.sleep(1)


def _lock_screen(device):
    device.execute_shell_command("svc power stayon false")
    time.sleep(1)


def disable_keyguard(device):
    _unlock_screen(device)
    _unlock_device(device)


def _sleep_according_to_result(result):
    if result:
        time.sleep(1)


##############################################################################
##############################################################################

class ResultManager(object):
    def __init__(self, testsuit_path, config):
        self.testsuite_path = testsuit_path
        self.config = config
        self.result_rootpath = self.config.report_path
        self.device = self.config.device
        if testsuit_path.endswith(".hap"):
            self.device_testpath = self.config.test_hap_out_path
        else:
            self.device_testpath = self.config.target_test_path
        self.testsuite_name = os.path.basename(self.testsuite_path)
        self.is_coverage = False

    def set_is_coverage(self, is_coverage):
        self.is_coverage = is_coverage

    def get_test_results(self, error_message=""):
        # Get test result files
        filepath = self.obtain_test_result_file()
        if not os.path.exists(filepath):
            _create_empty_result_file(filepath, self.testsuite_name,
                                      error_message)
        if "benchmark" == self.config.testtype[0]:
            if self.device.is_directory(
                    os.path.join(self.device_testpath, "benchmark")):
                self._obtain_benchmark_result()
        # Get coverage data files
        if self.is_coverage:
            self.obtain_coverage_data()

        return filepath

    def _obtain_benchmark_result(self):
        benchmark_root_dir = os.path.abspath(
            os.path.join(self.result_rootpath, "benchmark"))
        benchmark_dir = os.path.abspath(
            os.path.join(benchmark_root_dir,
                         self.get_result_sub_save_path(),
                         self.testsuite_name))

        if not os.path.exists(benchmark_dir):
            os.makedirs(benchmark_dir)

        print("benchmark_dir =%s", benchmark_dir)
        if not self.device.pull_file(
                os.path.join(self.device_testpath, "benchmark"),
                benchmark_dir):
            os.rmdir(benchmark_dir)
        return benchmark_dir

    def get_result_sub_save_path(self):
        find_key = os.sep + "tests" + os.sep
        file_dir, _ = os.path.split(self.testsuite_path)
        pos = file_dir.find(find_key)
        subpath = ""
        if -1 != pos:
            subpath = file_dir[pos + len(find_key):]
            pos1 = subpath.find(os.sep)
            if -1 != pos1:
                subpath = subpath[pos1 + len(os.sep):]
        print("subpath = " + subpath)
        return subpath

    def obtain_test_result_file(self):
        result_save_path = get_result_savepath(self.testsuite_path,
            self.result_rootpath)
        result_file_path = os.path.join(result_save_path,
            "%s.xml" % self.testsuite_name)

        result_josn_file_path = os.path.join(result_save_path,
            "%s.json" % self.testsuite_name)

        if self.testsuite_path.endswith('.hap'):
            remote_result_file = os.path.join(self.device_testpath,
                "testcase_result.xml")
            remote_json_result_file = os.path.join(self.device_testpath,
                "%s.json" % self.testsuite_name)
        else:
            remote_result_file = os.path.join(self.device_testpath,
                "%s.xml" % self.testsuite_name)
            remote_json_result_file = os.path.join(self.device_testpath,
                "%s.json" % self.testsuite_name)

        if self.device.is_file_exist(remote_result_file):
            self.device.pull_file(remote_result_file, result_file_path)
        elif self.device.is_file_exist(remote_json_result_file):
            self.device.pull_file(remote_json_result_file,
                                  result_josn_file_path)
            result_file_path = result_josn_file_path
        else:
            LOG.error("%s not exist", remote_result_file)

        return result_file_path

    def make_empty_result_file(self, error_message=""):
        result_savepath = get_result_savepath(self.testsuite_path,
            self.result_rootpath)
        result_filepath = os.path.join(result_savepath, "%s.xml" %
            self.testsuite_name)
        if not os.path.exists(result_filepath):
            _create_empty_result_file(result_filepath,
                self.testsuite_name, error_message)

    def is_exist_target_in_device(self, path, target):
        if platform.system() == "Windows":
            command = '\"ls -l %s | grep %s\"' % (path, target)
        else:
            command = "ls -l %s | grep %s" % (path, target)

        check_result = False
        stdout_info = self.device.execute_shell_command(command)
        if stdout_info != "" and stdout_info.find(target) != -1:
            check_result = True
        return check_result

    def obtain_coverage_data(self):
        cov_root_dir = os.path.abspath(os.path.join(
            self.result_rootpath,
            "..",
            "coverage",
            "data",
            "exec"))

        target_name = "obj"
        cxx_cov_path = os.path.abspath(os.path.join(
            self.result_rootpath,
            "..",
            "coverage",
            "data",
            "cxx",
            self.testsuite_name))

        if self.is_exist_target_in_device(DEFAULT_TEST_PATH, target_name):
            if not os.path.exists(cxx_cov_path):
                os.makedirs(cxx_cov_path)
            src_file = os.path.join(DEFAULT_TEST_PATH, target_name)
            self.device.pull_file(src_file, cxx_cov_path, is_create=True)


##############################################################################
##############################################################################

@Plugin(type=Plugin.DRIVER, id=DeviceTestType.cpp_test)
class CppTestDriver(IDriver):
    """
    CppTest is a Test that runs a native test package on given device.
    """
    # test driver config
    config = None
    result = ""

    def __check_environment__(self, device_options):
        if len(device_options) == 1 and device_options[0].label is None:
            return True
        if len(device_options) != 1 or \
                device_options[0].label != DeviceLabelType.phone:
            return False
        return True

    def __check_config__(self, config):
        pass

    def __result__(self):
        return self.result if os.path.exists(self.result) else ""

    def __execute__(self, request):
        try:
            self.config = request.config
            self.config.target_test_path = DEFAULT_TEST_PATH
            self.config.device = request.config.environment.devices[0]

            suite_file = request.root.source.source_file
            LOG.debug("Testsuite FilePath: %s" % suite_file)

            if not suite_file:
                LOG.error("test source '%s' not exists" %
                          request.root.source.source_string)
                return

            if not self.config.device:
                result = ResultManager(suite_file, self.config)
                result.set_is_coverage(False)
                result.make_empty_result_file(
                    "No test device is found. ")
                return

            serial = request.config.device.__get_serial__()
            device_log_file = get_device_log_file(
                request.config.report_path,
                serial)

            with open(device_log_file, "a", encoding="UTF-8") as file_pipe:
                self.config.device.start_catch_device_log(file_pipe)
                self._init_gtest()
                self._run_gtest(suite_file)
        finally:
            self.config.device.stop_catch_device_log()

    def _init_gtest(self):
        self.config.device.hdc_command("remount")
        self.config.device.execute_shell_command(
            "rm -rf %s" % self.config.target_test_path)
        self.config.device.execute_shell_command(
            "mkdir -p %s" % self.config.target_test_path)

    def _run_gtest(self, suite_file):
        from xdevice import Variables
        filename = os.path.basename(suite_file)
        test_para = self._get_test_para(self.config.testcase,
                                        self.config.testlevel,
                                        self.config.testtype,
                                        self.config.target_test_path,
                                        filename)
        is_coverage_test = True if self.config.coverage else False

        # push testsuite file
        self.config.device.push_file(suite_file, self.config.target_test_path)

        # push resource files
        resource_manager = ResourceManager()
        resource_data_dic, resource_dir = \
            resource_manager.get_resource_data_dic(suite_file)
        resource_manager.process_preparer_data(resource_data_dic, resource_dir,
                                               self.config.device)

        # execute testcase
        if not self.config.coverage:
            command = "cd %s; rm -rf %s.xml; chmod +x *; ./%s %s" % (
                self.config.target_test_path,
                filename,
                filename,
                test_para)
        else:
            coverage_outpath = self.config.coverage_outpath
            strip_num = len(coverage_outpath.split(os.sep)) - 1
            command = "cd %s; rm -rf %s.xml; chmod +x *; GCOV_PREFIX=. " \
                "GCOV_PREFIX_STRIP=%s ./%s %s" % \
                (self.config.target_test_path,
                 filename,
                 str(strip_num),
                 filename,
                 test_para)

        result = ResultManager(suite_file, self.config)
        result.set_is_coverage(is_coverage_test)

        try:
            # get result
            display_receiver = DisplayOutputReceiver()
            self.config.device.execute_shell_command(
                command,
                receiver=display_receiver,
                timeout=TIME_OUT,
                retry=0)
            return_message = display_receiver.output
        except (ExecuteTerminate, DeviceError) as exception:
            return_message = str(exception.args)

        self.result = result.get_test_results(return_message)
        resource_manager.process_cleaner_data(resource_data_dic,
            resource_dir,
            self.config.device)

    @staticmethod
    def _get_test_para(testcase,
                       testlevel,
                       testtype,
                       target_test_path,
                       filename):
        if "benchmark" == testtype[0]:
            test_para = (" --benchmark_out_format=json"
                         " --benchmark_out=%s%s.json") % (
                            target_test_path, filename)
            return test_para

        if "" != testcase and "" == testlevel:
            test_para = "%s=%s" % (GTestConst.exec_para_filter, testcase)
        elif "" == testcase and "" != testlevel:
            level_para = get_level_para_string(testlevel)
            test_para = "%s=%s" % (GTestConst.exec_para_level, level_para)
        else:
            test_para = ""
        return test_para


##############################################################################
##############################################################################
