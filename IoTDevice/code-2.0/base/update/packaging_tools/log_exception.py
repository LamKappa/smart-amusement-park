#!/usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2021 Huawei Device Co., Ltd.
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import logging
import sys


class UpdateToolLogger:
    """
    Global log class
    """
    INFO_LOG = 'INFO_LOG'
    WARNING_LOG = 'WARNING_LOG'
    ERROR_LOG = 'ERROR_LOG'
    LOG_TYPE = (INFO_LOG, WARNING_LOG, ERROR_LOG)

    def __init__(self, output_type='console'):
        self.__logger_obj = self.__get_logger_obj(output_type=output_type)

    @staticmethod
    def __get_logger_obj(output_type='console'):
        ota_logger = logging.getLogger(__name__)
        ota_logger.setLevel(level=logging.INFO)
        formatter = logging.Formatter(
            '%(asctime)s %(levelname)s : %(message)s',
            "%Y-%m-%d %H:%M:%S")
        if output_type == 'console':
            console_handler = logging.StreamHandler()
            console_handler.setLevel(logging.INFO)
            console_handler.setFormatter(formatter)
            ota_logger.addHandler(console_handler)
        elif output_type == 'file':
            file_handler = logging.FileHandler("UpdateToolLog.txt")
            file_handler.setLevel(logging.INFO)
            file_handler.setFormatter(formatter)
            ota_logger.addHandler(file_handler)
        return ota_logger

    def print_log(self, msg, log_type=INFO_LOG):
        """
        Print log information.
        :param msg: log information
        :param log_type: log type
        :return:
        """
        if log_type == self.LOG_TYPE[0]:
            self.__logger_obj.info(msg)
        elif log_type == self.LOG_TYPE[1]:
            self.__logger_obj.warning(msg)
        elif log_type == self.LOG_TYPE[2]:
            self.__logger_obj.error(msg)
        else:
            self.__logger_obj.error("Unknown log type! %s", log_type)
            return False
        return True

    def print_uncaught_exception_msg(self, msg, exc_info):
        """
        Print log when an uncaught exception occurs.
        :param msg: Uncaught exception
        :param exc_info: information about the uncaught exception
        """
        self.__logger_obj.error(msg, exc_info=exc_info)


UPDATE_LOGGER = UpdateToolLogger()


def handle_exception(exc_type, exc_value, exc_traceback):
    """
    Override global caught exceptions.
    :param exc_type: exception type
    :param exc_value: exception value
    :param exc_traceback: exception traceback
    :return:
    """
    if issubclass(exc_type, KeyboardInterrupt):
        sys.__excepthook__(exc_type, exc_value, exc_traceback)
        return
    UPDATE_LOGGER.print_uncaught_exception_msg(
        "Uncaught exception", exc_info=(exc_type, exc_value, exc_traceback))
    from utils import clear_resource
    clear_resource(err_clear=True)


sys.excepthook = handle_exception


class VendorExpandError(OSError):
    """
    Vendor extended exception class.
    Script interfaces are not completely overriden.
    """
    def __init__(self, script_class, func_name):
        super().__init__()
        self.script_class = script_class
        self.func_name = func_name

    def __str__(self, ):
        return ('%s Vendor expansion does not override function %s' %
                (self.script_class, self.func_name))
