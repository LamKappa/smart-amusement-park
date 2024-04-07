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

import optparse
import subprocess

if __name__ == '__main__':
    print("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^")
    parser_obj = optparse.OptionParser()
    parser_obj.add_option("--scriptname",
                      help="generate yacc script name",
                      action="store_true", default=True)
    parser_obj.add_option("--output",
                      help="yacc output path",
                      action="store_true", default="")
    (option_list, parse_params) = parser_obj.parse_args()

    if len(parse_params) < 1:
        parser_obj.error("yacc param error.")

    gen_script_name = parse_params[0]
    output_path = parse_params[1]
    parse_scripts = subprocess.check_call(
        [gen_script_name], stdout=subprocess.PIPE, cwd=output_path)
    print("result:", parse_scripts)
