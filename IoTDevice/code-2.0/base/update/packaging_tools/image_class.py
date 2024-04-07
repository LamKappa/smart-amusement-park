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
import bisect
import copy
import os
import struct
import tempfile
from hashlib import sha256

from log_exception import UPDATE_LOGGER
from blocks_manager import BlocksManager
from utils import SPARSE_IMAGE_MAGIC
from utils import HEADER_INFO_FORMAT
from utils import CHUNK_INFO_FORMAT
from utils import HEADER_INFO_LEN
from utils import CHUNK_INFO_LEN
from utils import EXTEND_VALUE
from utils import FILE_MAP_ZERO_KEY
from utils import FILE_MAP_NONZERO_KEY
from utils import FILE_MAP_COPY_KEY
from utils import MAX_BLOCKS_PER_GROUP
from utils import CHUNK_TYPE_RAW
from utils import CHUNK_TYPE_FILL
from utils import CHUNK_TYPE_DONT_CARE
from utils import CHUNK_TYPE_CRC32


class FullUpdateImage:
    """
    Full image processing class
    """

    def __init__(self, target_package_images_dir, full_img_list, verse_script,
                 no_zip=False):
        self.__target_package_images_dir = target_package_images_dir
        self.__full_img_list = full_img_list
        self.__verse_script = verse_script
        self.__no_zip = no_zip

    def update_full_image(self):
        """
        Processing of the full image
        :return full_image_content_len_list: full image content length list
        :return full_image_file_obj_list: full image temporary file list
        """
        full_image_file_obj_list = []
        full_image_content_len_list = []
        for each_name in self.__full_img_list:
            full_image_content = self.get_full_image_content(each_name)
            if full_image_content is False:
                UPDATE_LOGGER.print_log(
                    "Get full image content failed!",
                    log_type=UPDATE_LOGGER.ERROR_LOG)
                return False, False
            each_img = tempfile.NamedTemporaryFile(
                prefix="full_image%s" % each_name, mode='wb')
            each_img.write(full_image_content)
            full_image_content_len_list.append(len(full_image_content))
            full_image_file_obj_list.append(each_img)
            UPDATE_LOGGER.print_log(
                "Image %s full processing completed" % each_name)
            if not self.__no_zip:
                # No zip mode (no script command)
                if is_sparse_image(each_img.name):
                    sparse_image_write_cmd = \
                        self.__verse_script.sparse_image_write(each_name)
                    cmd = '%s_WRITE_FLAG%s' % (
                        each_name, sparse_image_write_cmd)
                else:
                    raw_image_write_cmd = \
                        self.__verse_script.raw_image_write(each_name)
                    cmd = '%s_WRITE_FLAG%s' % (
                        each_name, raw_image_write_cmd)
                self.__verse_script.add_command(
                    cmd=cmd)

        UPDATE_LOGGER.print_log(
            "All full image processing completed! image count: %d" %
            len(self.__full_img_list))
        return full_image_content_len_list, full_image_file_obj_list

    def get_full_image_content(self, each_name):
        """
        Obtain the full image content.
        :param each_name: image name
        :return content: full image content if available; false otehrwise
        """
        each_image_path = os.path.join(self.__target_package_images_dir,
                                       '%s.img' % each_name)
        if not os.path.exists(each_image_path):
            UPDATE_LOGGER.print_log(
                "The %s.img file is missing from the target package, "
                "the component: %s cannot be full update processed. "
                "path: %s" %
                (each_name, each_name, each_image_path),
                UPDATE_LOGGER.ERROR_LOG)
            return False
        with open(each_image_path, 'rb') as f_r:
            content = f_r.read()
        return content


def is_sparse_image(img_path):
    """
    Check whether the image is a sparse image.
    :param img_path: image path
    :return:
    """
    with open(img_path, 'rb') as f_r:
        image_content = f_r.read(HEADER_INFO_LEN)
        header_info = struct.unpack(HEADER_INFO_FORMAT, image_content)
        *_, is_sparse = SparseImage.image_header_info_check(header_info)
    return is_sparse


class SparseImage:
    """
    Sparse image class
    """
    def __init__(self, image_path, map_path):
        """
        Initialize the sparse image.
        :param image_path: img file path
        :param map_path: map file path
        """
        self.image_flag = True
        self.image_path = image_path
        self.offset_value_list = []
        self.care_block_range = None
        self.extended_range = None
        self.reserved_blocks = BlocksManager("0")
        self.file_map = []
        self.offset_index = []
        self.block_size = None
        self.total_blocks = None
        self.parse_sparse_image_file(image_path, map_path)

    def parse_sparse_image_file(self, image_path, map_path):
        """
        Parse the .img file.
        :param image_path: img file path
        :param map_path: map file path
        """
        with open(image_path, 'rb') as f_r:
            image_content = f_r.read(HEADER_INFO_LEN)
            header_info = struct.unpack(HEADER_INFO_FORMAT, image_content)

            block_size, chunk_header_info_size, header_info_size, magic_info, \
                total_blocks, total_chunks, self.image_flag = \
                self.image_header_info_check(header_info)
            self.block_size = block_size
            self.total_blocks = total_blocks

            if self.image_flag is False:
                UPDATE_LOGGER.print_log(
                    "This image is not a sparse image! path: %s" % image_path,
                    UPDATE_LOGGER.ERROR_LOG)
                return

            UPDATE_LOGGER.print_log("Sparse head info parsing completed!")

            pos_value = 0
            care_value_list, offset_value_list = [], []

            for _ in range(total_chunks):
                chunk_info_content = f_r.read(CHUNK_INFO_LEN)
                chunk_info = struct.unpack(
                    CHUNK_INFO_FORMAT, chunk_info_content)
                pos_value = self.parse_chunk_info(
                    block_size, care_value_list, chunk_info, f_r,
                    offset_value_list, pos_value)
                if pos_value is False:
                    raise RuntimeError
            self.care_block_range = BlocksManager(care_value_list)
            self.offset_index = [i[0] for i in offset_value_list]
            self.offset_value_list = offset_value_list
            extended_range = \
                self.care_block_range.extend_value_to_blocks(EXTEND_VALUE)
            all_blocks = BlocksManager(range_data=(0, total_blocks))
            self.extended_range = \
                extended_range.get_intersect_with_other(all_blocks).\
                get_subtract_with_other(self.care_block_range)
            self.parse_block_map_file(map_path, f_r)

    @staticmethod
    def parse_chunk_info(*args):
        """
        Parse the chunk information.
        :return pos_value: pos
        """
        block_size, care_value_list, chunk_info, f_r,\
            offset_value_list, pos_value = args
        chunk_type = chunk_info[0]
        # Chunk quantity
        chunk_size = chunk_info[2]
        total_size = chunk_info[3]
        data_size = total_size - 12

        # Chunk type, which can be CHUNK_TYPE_RAW, CHUNK_TYPE_FILL,
        # CHUNK_TYPE_DONT_CARE, or CHUNK_TYPE_CRC32.
        if chunk_type == CHUNK_TYPE_RAW:
            if data_size != chunk_size * block_size:
                UPDATE_LOGGER.print_log(
                    "chunk_size * block_size: %u and "
                    "data size: %u is not equal!" %
                    (data_size, chunk_size * block_size),
                    UPDATE_LOGGER.ERROR_LOG)
                return False
            else:
                temp_value = pos_value + chunk_size
                care_value_list.append(pos_value)
                care_value_list.append(temp_value)
                offset_value_list.append(
                    (pos_value, chunk_size, f_r.tell(), None))
                pos_value = temp_value
                f_r.seek(data_size, os.SEEK_CUR)

        elif chunk_type == CHUNK_TYPE_FILL:
            temp_value = pos_value + chunk_size
            fill_data = f_r.read(4)
            care_value_list.append(pos_value)
            care_value_list.append(temp_value)
            offset_value_list.append((pos_value, chunk_size, None, fill_data))
            pos_value = temp_value

        elif chunk_type == CHUNK_TYPE_DONT_CARE:
            if data_size != 0:
                UPDATE_LOGGER.print_log(
                    "CHUNK_TYPE_DONT_CARE chunk data_size"
                    " must be 0, data size: (%u)" %
                    data_size, UPDATE_LOGGER.ERROR_LOG)
                return False
            else:
                pos_value += chunk_size

        elif chunk_type == CHUNK_TYPE_CRC32:
            UPDATE_LOGGER.print_log(
                "Not supported chunk type CHUNK_TYPE_CRC32!",
                UPDATE_LOGGER.ERROR_LOG)
            return False

        else:
            UPDATE_LOGGER.print_log(
                "Not supported chunk type 0x%04X !" %
                chunk_type, UPDATE_LOGGER.ERROR_LOG)
            return False
        return pos_value

    def parse_block_map_file(self, map_path, image_file_r):
        """
        Parses the map file for blocks where files are contained in the image.
        :param map_path: map file path
        :param image_file_r: file reading object
        :return:
        """
        remain_range = self.care_block_range
        temp_file_map = {}

        with open(map_path, 'r') as f_r:
            # Read the .map file and process each line.
            for each_line in f_r.readlines():
                each_map_path, ranges_value = each_line.split(None, 1)
                each_range = BlocksManager(ranges_value)
                temp_file_map[each_map_path] = each_range
                # each_range is contained in the remain range.
                if each_range.size() != each_range.\
                        get_intersect_with_other(remain_range).size():
                    raise RuntimeError
                # After the processing is complete,
                # remove each_range from remain_range.
                remain_range = remain_range.get_subtract_with_other(each_range)
        reserved_blocks = self.reserved_blocks
        # Remove reserved blocks from all blocks.
        remain_range = remain_range.get_subtract_with_other(reserved_blocks)

        # Divide all blocks into zero_blocks
        # (if there are many) and nonzero_blocks.
        zero_blocks_list = []
        nonzero_blocks_list = []
        nonzero_groups_list = []
        default_zero_block = ('\0' * self.block_size).encode()

        nonzero_blocks_list, nonzero_groups_list, zero_blocks_list = \
            self.apply_remain_range(
                default_zero_block, image_file_r, nonzero_blocks_list,
                nonzero_groups_list, remain_range, zero_blocks_list)

        temp_file_map = self.get_file_map(
            nonzero_blocks_list, nonzero_groups_list,
            reserved_blocks, temp_file_map, zero_blocks_list)
        self.file_map = temp_file_map

    def apply_remain_range(self, *args):
        """
        Implement traversal processing of remain_range.
        """
        default_zero_block, image_file_r,\
            nonzero_blocks_list, nonzero_groups_list,\
            remain_range, zero_blocks_list = args
        for start_value, end_value in remain_range:
            for each_value in range(start_value, end_value):
                # bisect 二分查找，b在self.offset_index中的位置
                idx = bisect.bisect_right(self.offset_index, each_value) - 1
                chunk_start, _, file_pos, fill_data = \
                    self.offset_value_list[idx]
                data = self.get_file_data(self.block_size, chunk_start,
                                          default_zero_block, each_value,
                                          file_pos, fill_data, image_file_r)

                zero_blocks_list, nonzero_blocks_list, nonzero_groups_list = \
                    self.get_zero_nonzero_blocks_list(
                        data, default_zero_block, each_value,
                        nonzero_blocks_list, nonzero_groups_list,
                        zero_blocks_list)
        return nonzero_blocks_list, nonzero_groups_list, zero_blocks_list

    @staticmethod
    def get_file_map(*args):
        """
        Obtain the file map.
        nonzero_blocks_list nonzero blocks list,
        nonzero_groups_list nonzero groups list,
        reserved_blocks reserved blocks ,
        temp_file_map temporary file map,
        zero_blocks_list zero block list
        :return temp_file_map file map
        """
        nonzero_blocks_list, nonzero_groups_list,\
            reserved_blocks, temp_file_map, zero_blocks_list = args
        if nonzero_blocks_list:
            nonzero_groups_list.append(nonzero_blocks_list)
        if zero_blocks_list:
            temp_file_map[FILE_MAP_ZERO_KEY] = \
                BlocksManager(range_data=zero_blocks_list)
        if nonzero_groups_list:
            for i, blocks in enumerate(nonzero_groups_list):
                temp_file_map["%s-%d" % (FILE_MAP_NONZERO_KEY, i)] = \
                    BlocksManager(range_data=blocks)
        if reserved_blocks:
            temp_file_map[FILE_MAP_COPY_KEY] = reserved_blocks
        return temp_file_map

    @staticmethod
    def get_zero_nonzero_blocks_list(*args):
        """
        Get zero_blocks_list, nonzero_blocks_list, and nonzero_groups_list.
        data: block data,
        default_zero_block: default to zero block,
        each_value: each value,
        nonzero_blocks_list: nonzero_blocks_list,
        nonzero_groups_list: nonzero_groups_list,
        zero_blocks_list: zero_blocks_list,
        :return new_zero_blocks_list: new zero blocks list,
        :return new_nonzero_blocks_list: new nonzero blocks list,
        :return new_nonzero_groups_list: new nonzero groups list.
        """
        data, default_zero_block, each_value,\
            nonzero_blocks_list, nonzero_groups_list,\
            zero_blocks_list = args
        # Check whether the data block is equal to the default zero_blocks.
        if data == default_zero_block:
            zero_blocks_list.append(each_value)
            zero_blocks_list.append(each_value + 1)
        else:
            nonzero_blocks_list.append(each_value)
            nonzero_blocks_list.append(each_value + 1)
            # The number of nonzero_blocks is greater than
            # or equal to the upper limit.
            if len(nonzero_blocks_list) >= MAX_BLOCKS_PER_GROUP:
                nonzero_groups_list.append(nonzero_blocks_list)
                nonzero_blocks_list = []
        new_zero_blocks_list, new_nonzero_blocks_list, \
            new_nonzero_groups_list = copy.copy(zero_blocks_list), \
            copy.copy(nonzero_blocks_list), \
            copy.copy(nonzero_groups_list)
        return new_zero_blocks_list, new_nonzero_blocks_list, \
            new_nonzero_groups_list

    @staticmethod
    def get_file_data(*args):
        """
        Get the file data.
        block_size: blocksize,
        chunk_start: the start position of chunk,
        default_zero_block: default to zero blocks,
        each_value: each_value,
        file_pos: file position,
        fill_data: data,
        image_file_r: read file object,
        :return data: Get the file data.
        """
        block_size, chunk_start, default_zero_block, each_value,\
            file_pos, fill_data, image_file_r = args
        if file_pos is not None:
            file_pos += (each_value - chunk_start) * block_size
            image_file_r.seek(file_pos, os.SEEK_SET)
            data = image_file_r.read(block_size)
        else:
            if fill_data == default_zero_block[:4]:
                data = default_zero_block
            else:
                data = None
        return data

    def range_sha256(self, ranges):
        hash_obj = sha256()
        for data in self.__get_blocks_set_data(ranges):
            hash_obj.update(data)
        return hash_obj.hexdigest()

    def write_range_data_2_fd(self, ranges, file_obj):
        for data in self.__get_blocks_set_data(ranges):
            file_obj.write(data)

    def get_ranges(self, ranges):
        return [each_data for each_data in self.__get_blocks_set_data(ranges)]

    def __get_blocks_set_data(self, blocks_set_data):
        """
        Get the range data.
        """
        with open(self.image_path, 'rb') as f_r:
            for start, end in blocks_set_data:
                diff_value = end - start
                idx = bisect.bisect_right(self.offset_index, start) - 1
                chunk_start, chunk_len, file_pos, fill_data = \
                    self.offset_value_list[idx]

                remain = chunk_len - (start - chunk_start)
                this_read = min(remain, diff_value)
                if file_pos is not None:
                    pos = file_pos + ((start - chunk_start) * self.block_size)
                    f_r.seek(pos, os.SEEK_SET)
                    yield f_r.read(this_read * self.block_size)
                else:
                    yield fill_data * (this_read * (self.block_size >> 2))
                diff_value -= this_read

                while diff_value > 0:
                    idx += 1
                    chunk_start, chunk_len, file_pos, fill_data = \
                        self.offset_value_list[idx]
                    this_read = min(chunk_len, diff_value)
                    if file_pos is not None:
                        f_r.seek(file_pos, os.SEEK_SET)
                        yield f_r.read(this_read * self.block_size)
                    else:
                        yield fill_data * (this_read * (self.block_size >> 2))
                    diff_value -= this_read

    @staticmethod
    def image_header_info_check(header_info):
        """
        Check for new messages of the header_info image.
        :param header_info: header_info
        :return:
        """
        image_flag = True
        # Sparse mirroring header ID. The magic value is fixed to 0xED26FF3A.
        magic_info = header_info[0]
        # major version number
        major_version = header_info[1]
        # minor version number
        minor_version = header_info[2]
        # Length of the header information.
        # The value is fixed to 28 characters.
        header_info_size = header_info[3]
        # Header information size of the chunk.
        # The length is fixed to 12 characters.
        chunk_header_info_size = header_info[4]
        # Number of bytes of a block. The default size is 4096.
        block_size = header_info[5]
        # Total number of blocks contained in the current image
        # (number of blocks in a non-sparse image)
        total_blocks = header_info[6]
        # Total number of chunks contained in the current image
        total_chunks = header_info[7]
        if magic_info != SPARSE_IMAGE_MAGIC:
            UPDATE_LOGGER.print_log(
                "SparseImage head Magic should be 0xED26FF3A!",
                UPDATE_LOGGER.WARNING_LOG)
            image_flag = False
        if major_version != 1 or minor_version != 0:
            UPDATE_LOGGER.print_log(
                "SparseImage Only supported major version with "
                "minor version 1.0!",
                UPDATE_LOGGER.WARNING_LOG)
            image_flag = False
        if header_info_size != 28:
            UPDATE_LOGGER.print_log(
                "SparseImage header info size must be 28! size: %u." %
                header_info_size, UPDATE_LOGGER.WARNING_LOG)
            image_flag = False
        if chunk_header_info_size != 12:
            UPDATE_LOGGER.print_log(
                "SparseImage Chunk header size mast to be 12! size: %u." %
                chunk_header_info_size, UPDATE_LOGGER.WARNING_LOG)
            image_flag = False
        return block_size, chunk_header_info_size, header_info_size, \
            magic_info, total_blocks, total_chunks, image_flag
