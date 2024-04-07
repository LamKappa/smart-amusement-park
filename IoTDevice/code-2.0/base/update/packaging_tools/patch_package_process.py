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

import multiprocessing
import subprocess
import tempfile
from ctypes import pointer

from log_exception import UPDATE_LOGGER
from blocks_manager import BlocksManager
from transfers_manager import ActionType
from update_package import PkgHeader
from update_package import PkgComponent
from utils import OPTIONS_MANAGER
from utils import DIFF_EXE_PATH
from utils import get_lib_api

NEW_DAT = "new.dat"
PATCH_DAT = "patch.dat"
TRANSFER_LIST = "transfer.list"


class PatchProcess:
    def __init__(self, partition, tgt_sparse_image, src_sparse_image,
                 actions_list):
        self.actions_list = actions_list
        self.worker_threads = multiprocessing.cpu_count() // 2
        self.partition = partition
        self.tgt_sparse_img_obj, self.src_sparse_img_obj = \
            tgt_sparse_image, src_sparse_image
        self.version = 1
        self.touched_src_ranges = BlocksManager()
        self.touched_src_sha256 = None
        self.package_patch_zip = PackagePatchZip(partition)

    def patch_process(self):
        """
        Generate patches through calculation.
        """
        UPDATE_LOGGER.print_log("Patch Process!")

        new_dat_file_obj, patch_dat_file_obj, transfer_list_file_obj = \
            self.package_patch_zip.get_file_obj()

        stashes = {}
        total_blocks_count = 0
        stashed_blocks = 0
        max_stashed_blocks = 0
        transfer_content = ["%d\n" % self.version, "TOTAL_MARK\n",
                            "0\n", "MAX_STASH_MARK\n"]

        diff_offset = 0
        for each_action in self.actions_list:
            max_stashed_blocks, stashed_blocks = self.add_stash_command(
                each_action, max_stashed_blocks, stashed_blocks, stashes,
                transfer_content)

            free_commands_list, free_size, src_str_list = \
                self.add_free_command(each_action, stashes)

            src_str = " ".join(src_str_list)
            tgt_size = each_action.tgt_block_set.size()

            if each_action.type_str == ActionType.ZERO:
                total_blocks_count = \
                    self.apply_zero_type(each_action, total_blocks_count,
                                         transfer_content)
            elif each_action.type_str == ActionType.NEW:
                total_blocks_count = \
                    self.apply_new_type(each_action, new_dat_file_obj,
                                        tgt_size, total_blocks_count,
                                        transfer_content)
            elif each_action.type_str == ActionType.DIFFERENT:
                max_stashed_blocks, total_blocks_count, diff_offset = \
                    self.apply_diff_style(
                        diff_offset, each_action, max_stashed_blocks,
                        patch_dat_file_obj, src_str, stashed_blocks, tgt_size,
                        total_blocks_count, transfer_content)
            else:
                UPDATE_LOGGER.print_log("Unknown action type: %s!" %
                                        each_action.type_str)
                raise RuntimeError
            if free_commands_list:
                transfer_content.append("".join(free_commands_list))
                stashed_blocks -= free_size

        self.after_for_process(max_stashed_blocks, total_blocks_count,
                               transfer_content, transfer_list_file_obj)

    def apply_new_type(self, each_action, new_dat_file_obj, tgt_size,
                       total_blocks_count, transfer_content):
        self.tgt_sparse_img_obj.write_range_data_2_fd(
            each_action.tgt_block_set, new_dat_file_obj)
        UPDATE_LOGGER.print_log("%7s %s %s" % (
            each_action.type_str, each_action.tgt_name,
            str(each_action.tgt_block_set)))
        temp_size = self.write_split_transfers(
            transfer_content,
            each_action.type_str, each_action.tgt_block_set)
        if tgt_size != temp_size:
            raise RuntimeError
        total_blocks_count += temp_size
        return total_blocks_count

    def apply_zero_type(self, each_action, total_blocks_count,
                        transfer_content):
        UPDATE_LOGGER.print_log("%7s %s %s" % (
            each_action.type_str, each_action.tgt_name,
            str(each_action.tgt_block_set)))
        to_zero = \
            each_action.tgt_block_set.get_subtract_with_other(
                each_action.src_block_set)
        if self.write_split_transfers(transfer_content, each_action.type_str,
                                      to_zero) != to_zero.size():
            raise RuntimeError
        total_blocks_count += to_zero.size()
        return total_blocks_count

    def apply_diff_style(self, *args):
        """
        Process actions of the diff type.
        """
        diff_offset, each_action, max_stashed_blocks,\
            patch_dat_file_obj, src_str, stashed_blocks, tgt_size,\
            total_blocks_count, transfer_content = args
        if self.tgt_sparse_img_obj. \
                range_sha256(each_action.tgt_block_set) == \
                self.src_sparse_img_obj.\
                range_sha256(each_action.src_block_set):
            each_action.type_str = ActionType.MOVE
            UPDATE_LOGGER.print_log("%7s %s %s (from %s %s)" % (
                each_action.type_str, each_action.tgt_name,
                str(each_action.tgt_block_set),
                each_action.src_name,
                str(each_action.src_block_set)))

            max_stashed_blocks, total_blocks_count = \
                self.add_move_command(
                    each_action, max_stashed_blocks, src_str,
                    stashed_blocks, tgt_size, total_blocks_count,
                    transfer_content)
        else:
            do_img_diff, patch_value = self.compute_diff_patch(
                each_action, patch_dat_file_obj)

            if each_action.src_block_set.is_overlaps(
                    each_action.tgt_block_set):
                temp_stash_usage = \
                    stashed_blocks + each_action.src_block_set.size()
                if temp_stash_usage > max_stashed_blocks:
                    max_stashed_blocks = temp_stash_usage

            self.add_diff_command(diff_offset, do_img_diff,
                                  each_action, patch_value, src_str,
                                  transfer_content)

            diff_offset += len(patch_value)
            total_blocks_count += tgt_size
        return max_stashed_blocks, total_blocks_count, diff_offset

    def after_for_process(self, max_stashed_blocks, total_blocks_count,
                          transfer_content, transfer_list_file_obj):
        """
        Implement processing after cyclical actions_list processing.
        :param max_stashed_blocks: maximum number of stashed blocks in actions
        :param total_blocks_count: total number of blocks
        :param transfer_content: transfer content
        :param transfer_list_file_obj: transfer file object
        :return:
        """
        self.touched_src_sha256 = self.src_sparse_img_obj.range_sha256(
            self.touched_src_ranges)
        if self.tgt_sparse_img_obj.extended_range:
            if self.write_split_transfers(
                    transfer_content, ActionType.ZERO,
                    self.tgt_sparse_img_obj.extended_range) != \
                    self.tgt_sparse_img_obj.extended_range.size():
                raise RuntimeError
            total_blocks_count += self.tgt_sparse_img_obj.extended_range.size()
        all_tgt = BlocksManager(
            range_data=(0, self.tgt_sparse_img_obj.total_blocks))
        all_tgt_minus_extended = all_tgt.get_subtract_with_other(
            self.tgt_sparse_img_obj.extended_range)
        new_not_care = all_tgt_minus_extended.get_subtract_with_other(
            self.tgt_sparse_img_obj.care_block_range)
        self.add_erase_content(new_not_care, transfer_content)
        transfer_content = self.get_transfer_content(
            max_stashed_blocks, total_blocks_count, transfer_content)
        transfer_list_file_obj.write(transfer_content.encode())

    @staticmethod
    def get_transfer_content(max_stashed_blocks, total_blocks_count,
                             transfer_content):
        """
        Get the tranfer content.
        """
        transfer_content = ''.join(transfer_content)
        transfer_content = \
            transfer_content.replace("TOTAL_MARK", str(total_blocks_count))
        transfer_content = \
            transfer_content.replace("MAX_STASH_MARK", str(max_stashed_blocks))
        transfer_content = \
            transfer_content.replace("ActionType.MOVE", "move")
        transfer_content = \
            transfer_content.replace("ActionType.ZERO", "zero")
        transfer_content = \
            transfer_content.replace("ActionType.NEW", "new")
        return transfer_content

    def add_diff_command(self, *args):
        """
        Add the diff command.
        """
        diff_offset, do_img_diff, each_action,\
            patch_value, src_str, transfer_content = args
        self.touched_src_ranges = self.touched_src_ranges.get_union_with_other(
            each_action.src_block_set)
        diff_type = "imgdiff" if do_img_diff else "bsdiff"
        transfer_content.append("%s %d %d %s %s %s %s\n" % (
            diff_type,
            diff_offset, len(patch_value),
            self.src_sparse_img_obj.range_sha256(each_action.src_block_set),
            self.tgt_sparse_img_obj.range_sha256(each_action.tgt_block_set),
            each_action.tgt_block_set.to_string_raw(), src_str))

    def compute_diff_patch(self, each_action, patch_dat_file_obj):
        """
        Run the command to calculate the differential patch.
        """
        src_file_obj = \
            tempfile.NamedTemporaryFile(prefix="src-", mode='wb')
        self.src_sparse_img_obj.write_range_data_2_fd(
            each_action.src_block_set, src_file_obj)
        src_file_obj.seek(0)
        tgt_file_obj = tempfile.NamedTemporaryFile(
            prefix="tgt-", mode='wb')
        self.tgt_sparse_img_obj.write_range_data_2_fd(
            each_action.tgt_block_set, tgt_file_obj)
        tgt_file_obj.seek(0)
        OPTIONS_MANAGER.incremental_temp_file_obj_list.append(
            src_file_obj)
        OPTIONS_MANAGER.incremental_temp_file_obj_list.append(
            tgt_file_obj)
        do_img_diff = True if \
            each_action.tgt_name.split(".")[-1].lower() in \
            ("zip", "gz", "lz4", "hap") else False
        try:
            patch_value, do_img_diff = self.apply_compute_patch(
                src_file_obj.name, tgt_file_obj.name, do_img_diff)
        except ValueError:
            UPDATE_LOGGER.print_log("Patch process Failed!")
            UPDATE_LOGGER.print_log("%7s %s %s (from %s %s)" % (
                each_action.type_str, each_action.tgt_name,
                str(each_action.tgt_block_set),
                each_action.src_name,
                str(each_action.src_block_set)),
                                    UPDATE_LOGGER.ERROR_LOG)
            raise ValueError
        patch_dat_file_obj.write(patch_value)
        return do_img_diff, patch_value

    def add_move_command(self, *args):
        """
        Add the move command.
        """
        each_action, max_stashed_blocks, src_str,\
            stashed_blocks, tgt_size, total_blocks_count,\
            transfer_content = args
        src_block_set = each_action.src_block_set
        tgt_block_set = each_action.tgt_block_set
        if src_block_set != tgt_block_set:
            if src_block_set.is_overlaps(tgt_block_set):
                temp_stash_usage = stashed_blocks + \
                                   src_block_set.size()
                if temp_stash_usage > max_stashed_blocks:
                    max_stashed_blocks = temp_stash_usage

            self.touched_src_ranges = \
                self.touched_src_ranges.get_union_with_other(src_block_set)

            transfer_content.append(
                "{type_str} {tgt_hash} {tgt_string} {src_str}\n".
                format(type_str=each_action.type_str,
                       tgt_hash=self.tgt_sparse_img_obj.
                       range_sha256(each_action.tgt_block_set),
                       tgt_string=tgt_block_set.to_string_raw(),
                       src_str=src_str))
            total_blocks_count += tgt_size
        return max_stashed_blocks, total_blocks_count

    def add_free_command(self, each_action, stashes):
        """
        Add the free command.
        :param each_action: action object to be processed
        :param stashes: Stash dict
        :return: free_commands_list, free_size, src_str_list
        """
        free_commands_list = []
        free_size = 0
        src_blocks_size = each_action.src_block_set.size()
        src_str_list = [str(src_blocks_size)]
        un_stashed_src_ranges = each_action.src_block_set
        mapped_stashes = []
        for _, each_stash_before in each_action.use_stash:
            un_stashed_src_ranges = \
                un_stashed_src_ranges.get_subtract_with_other(
                    each_stash_before)
            src_range_sha = \
                self.src_sparse_img_obj.range_sha256(each_stash_before)
            each_stash_before = \
                each_action.src_block_set.get_map_within(each_stash_before)
            mapped_stashes.append(each_stash_before)
            if src_range_sha not in stashes:
                raise RuntimeError
            src_str_list.append(
                "%s:%s" % (src_range_sha, each_stash_before.to_string_raw()))
            stashes[src_range_sha] -= 1
            if stashes[src_range_sha] == 0:
                free_commands_list.append("free %s\n" % (src_range_sha,))
                free_size += each_stash_before.size()
                stashes.pop(src_range_sha)
        self.apply_stashed_range(each_action, mapped_stashes, src_blocks_size,
                                 src_str_list, un_stashed_src_ranges)
        return free_commands_list, free_size, src_str_list

    def apply_stashed_range(self, *args):
        each_action, mapped_stashes, src_blocks_size,\
            src_str_list, un_stashed_src_ranges = args
        if un_stashed_src_ranges.size() != 0:
            src_str_list.insert(1, un_stashed_src_ranges.to_string_raw())
            if each_action.use_stash:
                mapped_un_stashed = each_action.src_block_set.get_map_within(
                    un_stashed_src_ranges)
                src_str_list.insert(2, mapped_un_stashed.to_string_raw())
                mapped_stashes.append(mapped_un_stashed)
                self.check_partition(
                    BlocksManager(range_data=(0, src_blocks_size)),
                    mapped_stashes)
        else:
            src_str_list.insert(1, "-")
            self.check_partition(
                BlocksManager(range_data=(0, src_blocks_size)), mapped_stashes)

    def add_stash_command(self, each_action, max_stashed_blocks,
                          stashed_blocks, stashes, transfer_content):
        """
        Add the stash command.
        :param each_action: action object to be processed
        :param max_stashed_blocks: number of max stash blocks in all actions
        :param stashed_blocks: number of stash blocks
        :param stashes: Stash dict
        :param transfer_content: transfer content list
        :return: max_stashed_blocks, stashed_blocks
        """
        for _, each_stash_before in each_action.stash_before:
            src_range_sha = \
                self.src_sparse_img_obj.range_sha256(each_stash_before)
            if src_range_sha in stashes:
                stashes[src_range_sha] += 1
            else:
                stashes[src_range_sha] = 1
                stashed_blocks += each_stash_before.size()
                self.touched_src_ranges = \
                    self.touched_src_ranges.\
                    get_union_with_other(each_stash_before)
                transfer_content.append("stash %s %s\n" % (
                    src_range_sha, each_stash_before.to_string_raw()))
        if stashed_blocks > max_stashed_blocks:
            max_stashed_blocks = stashed_blocks
        return max_stashed_blocks, stashed_blocks

    def write_script(self, partition, script_check_cmd_list,
                     script_write_cmd_list, verse_script):
        """
        Add command content to the script.
        :param partition: image name
        :param script_check_cmd_list: incremental check command list
        :param script_write_cmd_list: incremental write command list
        :param verse_script: verse script object
        :return:
        """
        ranges_str = self.touched_src_ranges.to_string_raw()
        expected_sha = self.touched_src_sha256

        sha_check_cmd = verse_script.sha_check(
            ranges_str, expected_sha, partition)

        first_block_check_cmd = verse_script.first_block_check(partition)

        abort_cmd = verse_script.abort(partition)

        cmd = 'if ({sha_check_cmd} != 0 || ' \
              '{first_block_check_cmd} != 0)' \
              '{{\n    {abort_cmd}}}\n'.format(
                sha_check_cmd=sha_check_cmd,
                first_block_check_cmd=first_block_check_cmd,
                abort_cmd=abort_cmd)

        script_check_cmd_list.append(cmd)

        block_update_cmd = verse_script.block_update(partition)

        cmd = '%s_WRITE_FLAG%s' % (partition, block_update_cmd)
        script_write_cmd_list.append(cmd)

    def add_erase_content(self, new_not_care, transfer_content):
        """
        Add the erase command.
        :param new_not_care: blocks that don't need to be cared about
        :param transfer_content: transfer content list
        :return:
        """
        erase_first = new_not_care.\
            get_subtract_with_other(self.touched_src_ranges)
        if erase_first.size() != 0:
            transfer_content.insert(
                4, "erase %s\n" % (erase_first.to_string_raw(),))
        erase_last = new_not_care.get_subtract_with_other(erase_first)
        if erase_last.size() != 0:
            transfer_content.append(
                "erase %s\n" % (erase_last.to_string_raw(),))

    @staticmethod
    def check_partition(total, seq):
        so_far = BlocksManager()
        for i in seq:
            if so_far.is_overlaps(i):
                raise RuntimeError
            so_far = so_far.get_union_with_other(i)
        if so_far != total:
            raise RuntimeError

    @staticmethod
    def write_split_transfers(transfer_content, type_str, target_blocks):
        """
        Limit the size of operand in command 'new' and 'zero' to 1024 blocks.
        :param transfer_content: transfer content list
        :param type_str: type of the action to be processed.
        :param target_blocks: BlocksManager of the target blocks
        :return: total
        """
        if type_str not in (ActionType.NEW, ActionType.ZERO):
            raise RuntimeError
        blocks_limit = 1024
        total = 0
        while target_blocks.size() != 0:
            blocks_to_write = target_blocks.get_first_block_obj(blocks_limit)
            transfer_content.append(
                "%s %s\n" % (type_str, blocks_to_write.to_string_raw()))
            total += blocks_to_write.size()
            target_blocks = \
                target_blocks.get_subtract_with_other(blocks_to_write)
        return total

    @staticmethod
    def apply_compute_patch(src_file, tgt_file, imgdiff=False):
        """
        Add command content to the script.
        :param src_file: source file name
        :param tgt_file: target file name
        :param imgdiff: whether to execute imgdiff judgment
        :return:
        """
        patch_file_obj = \
            tempfile.NamedTemporaryFile(prefix="patch-", mode='wb')

        OPTIONS_MANAGER.incremental_temp_file_obj_list.append(
            patch_file_obj)
        cmd = [DIFF_EXE_PATH] if imgdiff else [DIFF_EXE_PATH, '-b', '1']

        cmd.extend(['-s', src_file, '-d', tgt_file, '-p', patch_file_obj.name])
        sub_p = subprocess.Popen(cmd, stdout=subprocess.PIPE,
                                 stderr=subprocess.STDOUT)
        output, _ = sub_p.communicate()

        if sub_p.returncode != 0:
            raise ValueError(output)

        with open(patch_file_obj.name, 'rb') as file_read:
            patch_content = file_read.read()
        return patch_content, imgdiff


class PackagePatchZip:
    """
    Compress the patch file generated by the
    differential calculation as *.zip file.
    """
    def __init__(self, partition):

        self.partition_new_dat_file_name = "%s.%s" % (partition, NEW_DAT)
        self.partition_patch_dat_file_name = "%s.%s" % (partition, PATCH_DAT)
        self.partition_transfer_file_name = "%s.%s" % (
            partition, TRANSFER_LIST)

        self.new_dat_file_obj = tempfile.NamedTemporaryFile(
            prefix="%s-" % NEW_DAT, mode='wb')
        self.patch_dat_file_obj = tempfile.NamedTemporaryFile(
            prefix="%s-" % PATCH_DAT, mode='wb')
        self.transfer_list_file_obj = tempfile.NamedTemporaryFile(
            prefix="%s-" % TRANSFER_LIST, mode='wb')

        OPTIONS_MANAGER.incremental_temp_file_obj_list.append(
            self.new_dat_file_obj)
        OPTIONS_MANAGER.incremental_temp_file_obj_list.append(
            self.patch_dat_file_obj)
        OPTIONS_MANAGER.incremental_temp_file_obj_list.append(
            self.transfer_list_file_obj)

        self.partition_file_obj = \
            tempfile.NamedTemporaryFile(prefix="partition_patch-")
        self.partition_head_list = PkgHeader()
        pkg_components = PkgComponent * 3
        self.partition_component_list = pkg_components()
        OPTIONS_MANAGER.incremental_image_file_obj_list.append(
            self.partition_file_obj)
        self.set_package_file_args()

    def get_file_obj(self):
        """
        Obtain file objects.
        """
        return self.new_dat_file_obj, self.patch_dat_file_obj, \
            self.transfer_list_file_obj

    def set_package_file_args(self):
        """
        Set Diff patch calculation and packaging parameters.
        """
        self.partition_head_list.digest_method = 0
        self.partition_head_list.sign_method = 0
        self.partition_head_list.pkg_type = 2
        self.partition_head_list.entry_count = 3
        self.partition_component_list[0].file_path = \
            self.new_dat_file_obj.name.encode("utf-8")
        self.partition_component_list[0].component_addr = \
            self.partition_new_dat_file_name.encode("utf-8")
        self.partition_component_list[1].file_path = \
            self.patch_dat_file_obj.name.encode("utf-8")
        self.partition_component_list[1].component_addr = \
            self.partition_patch_dat_file_name.encode("utf-8")

    def package_patch_zip(self):
        """
        Compress the partition diff patch calculation data as *.zip package.
        """
        self.partition_file_obj.seek(0)
        self.patch_dat_file_obj.seek(0)
        self.new_dat_file_obj.seek(0)
        self.transfer_list_file_obj.seek(0)
        self.partition_component_list[2].file_path = \
            self.transfer_list_file_obj.name.encode("utf-8")
        self.partition_component_list[2].component_addr = \
            self.partition_transfer_file_name.encode("utf-8")
        lib = get_lib_api()
        lib.CreatePackage(pointer(self.partition_head_list),
                          self.partition_component_list,
                          self.partition_file_obj.name.encode("utf-8"),
                          OPTIONS_MANAGER.private_key.encode("utf-8"))
