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

import heapq
import itertools
import operator


class BlocksManager(object):
    """
    blocks manager
    """

    def __init__(self, range_data=None):
        self.monotonic = False
        if isinstance(range_data, str):
            self.__parse_data_text(range_data)
        elif range_data:
            if len(range_data) % 2 != 0:
                raise RuntimeError
            self.range_data = tuple(self.__remove_repeated_pairs(range_data))
            self.monotonic = all(
                x < y for x, y in zip(self.range_data, self.range_data[1:]))
        else:
            self.range_data = ()

    def __iter__(self):
        for i in range(0, len(self.range_data), 2):
            yield self.range_data[i:i + 2]

    def __eq__(self, other):
        return self.range_data == other.range_data

    def __ne__(self, other):
        return self.range_data != other.range_data

    def __parse_data_text(self, text):
        """
        Parse data from text content.
        """
        data = []
        monotonic = True
        last = -1
        for split_content in text.split():
            if "-" in split_content:
                start_value, end_value = \
                    (int(n) for n in split_content.split("-"))
                data.append(start_value)
                data.append(end_value + 1)
                if last <= start_value <= end_value:
                    last = end_value
                else:
                    monotonic = False
            else:
                int_content = int(split_content)
                data.append(int_content)
                data.append(int_content + 1)
                if last <= int_content:
                    last = int_content + 1
                else:
                    monotonic = False
        data.sort()
        self.range_data = tuple(self.__remove_repeated_pairs(data))
        self.monotonic = monotonic

    @staticmethod
    def __remove_repeated_pairs(source):
        """
        Remove repeated blocks.
        """
        new = None
        for num in source:
            if num == new:
                new = None
            else:
                if new is not None:
                    yield new
                new = num
        if new is not None:
            yield new

    def to_string_raw(self):
        if len(self.range_data) == 0:
            raise RuntimeError
        return "".join([str(len(self.range_data)), ",", ",".join(
            str(i) for i in self.range_data)])

    def get_union_with_other(self, other):
        """
        Obtain the intersection.
        """
        range_a = self.get_subtract_with_other(other)
        range_b = other.get_subtract_with_other(self)
        range_c = self.get_intersect_with_other(other)
        range_e, range_f, range_g = \
            list(range_a.range_data), list(range_b.range_data), list(
                range_c.range_data)
        range_d = []
        range_d.extend(range_e)
        range_d.extend(range_f)
        range_d.extend(range_g)
        range_d.sort()
        return BlocksManager(range_data=range_d)

    def get_intersect_with_other(self, other):
        """
        Obtain the intersection.
        """
        other_data, data, new_data = list(self.range_data), list(
            other.range_data), []
        for i in range(len(data) // 2):
            for j in range(len(other_data) // 2):
                data_list1 = [data[i * 2], data[i * 2 + 1], other_data[j * 2],
                              other_data[j * 2 + 1]]
                data_list2 = [other_data[j * 2], other_data[j * 2 + 1],
                              data[i * 2], data[i * 2 + 1]]
                sort_list = [data[i * 2], data[i * 2 + 1], other_data[j * 2],
                             other_data[j * 2 + 1]]
                sort_list.sort()
                if operator.ne(sort_list, data_list1) and \
                        operator.ne(sort_list, data_list2):
                    new_data.append(sort_list[1])
                    new_data.append(sort_list[2])
        return BlocksManager(range_data=new_data)

    def get_subtract_with_other(self, other):
        """
        Obtain the difference set.
        """
        intersect_ran = self.get_intersect_with_other(other)
        data, intersect_data = list(self.range_data), list(
            intersect_ran.range_data)
        new_data = data + intersect_data
        new_data.sort()
        return BlocksManager(range_data=new_data)

    def is_overlaps(self, other):
        """
        Determine whether there is non-empty overlap.
        """
        intersect_range = self.get_intersect_with_other(other)
        if intersect_range.size():
            return True
        return False

    def size(self):
        """
        Obtain the self size.
        """
        total = 0
        data = list(self.range_data)
        for i in range(len(data) // 2):
            total += data[i * 2 + 1] - data[i * 2]
        return total

    def get_map_within(self, other):
        """
        When other is a subset of self,
        obtain the continuous range starting from 0.
        :param other:
        :return:
        """
        out = []
        offset = 0
        start = None
        for be_num, af_num in \
                heapq.merge(zip(self.range_data, itertools.cycle((-5, +5))),
                            zip(other.range_data, itertools.cycle((-1, +1)))):
            if af_num == -5:
                start = be_num
            elif af_num == +5:
                offset += be_num - start
                start = None
            else:
                out.append(offset + be_num - start)
        return BlocksManager(range_data=out)

    def extend_value_to_blocks(self, value):
        """
        Extend self
        :param value:
        :return:
        """
        data = list(self.range_data)
        remove_data = []
        for i in range(len(data) // 2):
            data[i * 2 + 1] = data[i * 2 + 1] + value
            data[i * 2] = max(0, data[i * 2] - value)
        for i in range(len(data) // 2 - 1):
            sign_1 = data[i * 2 + 1]
            sign_2 = data[(i + 1) * 2]
            if sign_1 >= sign_2:
                remove_data.append(sign_2)
                remove_data.append(sign_1)
        for j in remove_data:
            data.remove(j)
        return BlocksManager(data)

    def get_first_block_obj(self, value):
        """
        Return the first range pair containing the value.
        :param value:
        :return:
        """
        if self.size() <= value:
            return self
        data = list(self.range_data)
        be_value, af_value = 0, 1
        for i in range(len(data) // 2):
            be_value += data[i * 2 + 1] - data[i * 2]
            if be_value > value:
                data[i * 2 + 1] = data[i * 2 + 1] - be_value + value
                break
            else:
                af_value += 1
        return BlocksManager(range_data=data[:af_value * 2])
