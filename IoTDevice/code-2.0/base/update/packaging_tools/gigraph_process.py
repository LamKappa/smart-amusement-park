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
from collections import OrderedDict

from log_exception import UPDATE_LOGGER

# 50% of the data partition, in KB x 1024.
DATA_SIZE = 1374024 * 1024


class GigraphProcess(object):
    def __init__(self, actions_list, src_sparse_image, tgt_sparse_image):
        self.actions_list = actions_list
        if len(self.actions_list) == 0:
            raise RuntimeError
        self.size_of_source_list = 0
        self.src_sparse_img_obj = src_sparse_image
        self.tgt_sparse_img_obj = tgt_sparse_image
        self.vertices = len(self.actions_list)
        self.data_size = DATA_SIZE

        self.generate_digraph()
        self.stash_process()

    def generate_digraph(self):
        """
        Start correlation lookup.
        """
        source_ranges = []
        source_ranges = \
            self.get_source_ranges(self.actions_list, source_ranges)

        self.get_intersections_dict(source_ranges)
        # Start ordering.
        topo_logical = TopoLogical(self)
        action_stack = topo_logical.stack()
        new_action_list = []
        for action in action_stack:
            action.order = len(new_action_list)
            new_action_list.append(action)
        self.actions_list = new_action_list

    def get_intersections_dict(self, source_ranges):
        """
        Get the intersections_dict.
        :param source_ranges: source blocks
        :return:
        """
        for each_action in self.actions_list:
            intersections = OrderedDict()
            for start_value, end_value in each_action.tgt_block_set:
                for i in range(start_value, end_value):
                    if i >= len(source_ranges):
                        break
                    if source_ranges[i] is not None:
                        for j in source_ranges[i]:
                            intersections[j] = None

            self.update_goes_before_and_after(each_action, intersections)

    @staticmethod
    def update_goes_before_and_after(each_action, intersections):
        """
        Update "goes before" and "goes after".
        :param each_action: action to be processed
        :param intersections: intersections dict
        :return:
        """
        for each_intersection in intersections:
            if each_action is each_intersection:
                continue

            intersect_range = \
                each_action.tgt_block_set.get_intersect_with_other(
                    each_intersection.src_block_set)
            if intersect_range:
                if each_intersection.src_name == "__ZERO":
                    size = 0
                else:
                    size = intersect_range.size()
                each_intersection.child[each_action] = size
                each_action.parent[each_intersection] = size

    @staticmethod
    def get_source_ranges(transfers, source_ranges):
        """
        Update "goes before" and "goes after".
        :param transfers: actions list
        :param source_ranges: source blocks
        :return:
        """
        for each_action in transfers:
            for start_value, end_value in each_action.src_block_set:
                if end_value > len(source_ranges):
                    source_ranges.extend(
                        [None] * (end_value - len(source_ranges)))
                for i in range(start_value, end_value):
                    if source_ranges[i] is None:
                        source_ranges[i] = \
                            OrderedDict.fromkeys([each_action])
                    else:
                        source_ranges[i][each_action] = None
        return source_ranges

    def stash_process(self):
        """
        Stash processing
        """
        UPDATE_LOGGER.print_log("Reversing backward edges...")
        stash_raw_id = 0
        for each_action in self.actions_list:
            each_child_dict = each_action.child.copy()
            for each_before in each_child_dict:
                if each_action.order >= each_before.order:
                    intersect_block_set = \
                        each_action.src_block_set.get_intersect_with_other(
                            each_before.tgt_block_set)

                    each_before.stash_before.append(
                        (stash_raw_id, intersect_block_set))
                    each_action.use_stash.append(
                        (stash_raw_id, intersect_block_set))
                    stash_raw_id += 1
                    each_action.child.pop(each_before)
                    each_before.parent.pop(each_action)
                    each_action.parent[each_before] = None
                    each_before.child[each_action] = None
        UPDATE_LOGGER.print_log("Reversing backward edges completed!")


class DirectedCycle(object):
    def __init__(self, graph):
        self.graph = graph
        self.marked = [False for _ in range(self.graph.vertices)]
        self.has_cycle = False
        self.ontrack = [False for _ in range(self.graph.vertices)]


class DepthFirstOrder:
    def __init__(self, graph):
        self.graph = graph
        self.marked = {}
        self.stack = []

        for each_action in self.graph.actions_list:
            self.marked[each_action] = False

    def dfs(self):
        def dfs(index):
            self.marked[index] = True
            for each_child in index.child:
                if not self.marked[each_child]:
                    dfs(each_child)
            self.stack.insert(0, index)

        for each_action in self.graph.actions_list:
            if not self.marked[each_action]:
                dfs(each_action)
        return self.stack

    def sort_vertices(self):
        return self.dfs()


class TopoLogical(object):
    def __init__(self, graph):
        self.order = None
        self.cycle = DirectedCycle(graph)
        if not self.cycle.has_cycle:
            dfo = DepthFirstOrder(graph)
            self.order = dfo.sort_vertices()

    def stack(self):
        return self.order
