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
"""
Description: create actions_list and transfer package
"""

import os
import re
from collections import OrderedDict
from enum import Enum

from blocks_manager import BlocksManager
from log_exception import UPDATE_LOGGER
from utils import FILE_MAP_ZERO_KEY
from utils import FILE_MAP_COPY_KEY

VERSION_NAME_RE = r"[0-9]+"
REPLACE_CONTENT = "#"


class ActionType(Enum):
    NEW = 0
    ZERO = 1
    DIFFERENT = 2
    MOVE = 3


class ActionInfo:
    def __init__(self, type_str, tgt_name, src_name, tgt_block_set,
                 src_block_set):
        self.type_str = type_str
        self.tgt_name = tgt_name
        self.src_name = src_name
        self.tgt_block_set = tgt_block_set
        if src_block_set is not None:
            self.src_block_set = src_block_set
        else:
            self.src_block_set = BlocksManager()
        self.child = OrderedDict()
        self.parent = OrderedDict()
        self.stash_before = []
        self.use_stash = []

    def get_max_block_number(self):
        if self.src_block_set and self.src_block_set.size() != 0:
            return max(self.src_block_set.range_data)
        else:
            return 0

    def net_stash_change(self):
        return (sum(sr.size() for (_, sr) in self.stash_before) -
                sum(sr.size() for (_, sr) in self.use_stash))


class TransfersManager(object):
    def __init__(self, partition, tgt_sparse_img_obj, src_sparse_img_obj,
                 disable_img_diff=False):
        self.tgt_sparse_img_obj = tgt_sparse_img_obj
        self.src_sparse_img_obj = src_sparse_img_obj
        self.partition = partition
        self.disable_img_diff = disable_img_diff

        self.action_file_list = []

    @staticmethod
    def simplify_file_name(file_name):
        base_name = os.path.basename(file_name)
        no_version_name = re.sub(VERSION_NAME_RE, REPLACE_CONTENT, base_name)
        return base_name, no_version_name

    def arrange_source_file(self):
        base_names = {}
        version_patterns = {}
        for file_name in self.src_sparse_img_obj.file_map.keys():
            base_name, no_version_name = self.simplify_file_name(file_name)
            base_names[base_name] = file_name
            version_patterns[no_version_name] = file_name
        return base_names, version_patterns

    def find_process_needs(self):
        """
        generate action_list
        """
        src_base_names, src_version_patterns = self.arrange_source_file()
        max_size = -1

        for tgt_file_name, tgt_blocks in \
                self.tgt_sparse_img_obj.file_map.items():
            if FILE_MAP_ZERO_KEY == tgt_file_name:
                UPDATE_LOGGER.print_log("Apply ZERO type!")
                self.action_file_list.append(
                    ActionInfo(
                        ActionType.ZERO, tgt_file_name, FILE_MAP_ZERO_KEY,
                        tgt_blocks, self.src_sparse_img_obj.
                        file_map.get(FILE_MAP_ZERO_KEY, None)))
                continue
            if FILE_MAP_COPY_KEY == tgt_file_name:
                UPDATE_LOGGER.print_log("Apply COPY type!")
                self.action_file_list.append(
                    ActionInfo(ActionType.NEW, tgt_file_name,
                               None, tgt_blocks, None))
                continue
            if tgt_file_name in self.src_sparse_img_obj.file_map:
                UPDATE_LOGGER.print_log("Apply DIFF type!")
                action_info = ActionInfo(
                    ActionType.DIFFERENT, tgt_file_name, tgt_file_name,
                    tgt_blocks,
                    self.src_sparse_img_obj.file_map[tgt_file_name])
                max_size = action_info.get_max_block_number() \
                    if action_info.get_max_block_number() > \
                    max_size else max_size
                self.action_file_list.append(action_info)
                continue
            src_file_name = self.get_file_name(
                src_base_names, src_version_patterns, tgt_file_name)
            if src_file_name:
                action_info = ActionInfo(
                    ActionType.DIFFERENT, tgt_file_name, src_file_name,
                    tgt_blocks,
                    self.src_sparse_img_obj.file_map[src_file_name])
                max_size = action_info.get_max_block_number() if \
                    action_info.get_max_block_number() > max_size else max_size
                self.action_file_list.append(action_info)
                continue
            self.action_file_list.append(
                ActionInfo(ActionType.NEW, tgt_file_name,
                           None, tgt_blocks, None))

        return max_size

    def get_file_name(self, src_base_names, src_version_patterns,
                      tgt_file_name):
        tgt_base_name, tgt_version_patterns = \
            self.simplify_file_name(tgt_file_name)
        has_diff_name = True if tgt_base_name in src_base_names else False
        has_diff_version = \
            True if tgt_version_patterns in src_version_patterns else False
        src_file_name = \
            src_base_names[tgt_base_name] if has_diff_name else \
            src_version_patterns[tgt_version_patterns] if \
            has_diff_version else None
        return src_file_name

    def get_action_list(self):
        return self.action_file_list
