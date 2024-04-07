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

from script_generator import Script
from utils import SCRIPT_KEY_LIST


def create_vendor_script_class():
    """
    Obtain the extended script objects of the vendor. By default,
    the return value is [None] * len(SCRIPT_KEY_LIST).
    SCRIPT_KEY_LIST is the stage list in Opera mode.
    If needed, rewrite this function to create a Vendor{Opera}Script object
    class and return the object. Sample code is as follows:
    prelude_script = VendorPreludeScript()
    verse_script = VendorVerseScript()
    refrain_script = VendorRefrainScript()
    ending_script = VendorEndingScript()
    opera_obj_list = [prelude_script, verse_script,
                    refrain_script, ending_script]
    :return opera_obj_list: {Opera}script object list
    """
    opera_obj_list = [None] * len(SCRIPT_KEY_LIST)
    return opera_obj_list


class VendorPreludeScript(Script):
    def __init__(self):
        super().__init__()


class VendorVerseScript(Script):
    def __init__(self):
        super().__init__()


class VendorRefrainScript(Script):
    def __init__(self):
        super().__init__()


class VendorEndingScript(Script):
    def __init__(self):
        super().__init__()
