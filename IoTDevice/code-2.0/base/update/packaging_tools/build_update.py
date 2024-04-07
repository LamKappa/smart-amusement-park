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
The tool for making updater package.

positional arguments:
  target_package        Target package file path.
  update_package        Update package file path.

optional arguments:
  -h, --help            show this help message and exit
  -s SOURCE_PACKAGE, --source_package SOURCE_PACKAGE
                        Source package file path.
  -nz, --no_zip         No zip mode,
                        which means to output the update package without zip.
  -pf PARTITION_FILE, --partition_file PARTITION_FILE
                        Variable partition mode, Partition list file path.
  -sa {ECC,RSA}, --signing_algorithm {ECC,RSA}
                        The signing algorithms
                        supported by the tool include ['ECC', 'RSA'].
  -ha {sha256,sha384}, --hash_algorithm {sha256,sha384}
                        The hash algorithms
                        supported by the tool include ['sha256', 'sha384'].
  -pk PRIVATE_KEY, --private_key PRIVATE_KEY
                        Private key file path.

"""
import copy
import filecmp
import os
import argparse
import re
import subprocess

import xmltodict

from gigraph_process import GigraphProcess
from image_class import FullUpdateImage
from image_class import SparseImage
from patch_package_process import PatchProcess
from transfers_manager import TransfersManager
from log_exception import UPDATE_LOGGER
from script_generator import PreludeScript
from script_generator import VerseScript
from script_generator import RefrainScript
from script_generator import EndingScript
from update_package import build_update_package
from utils import OPTIONS_MANAGER
from utils import UPDATER_CONFIG
from utils import parse_partition_file_xml
from utils import unzip_package
from utils import clear_resource
from utils import PRODUCT
from utils import XML_FILE_PATH
from utils import get_update_info
from utils import SCRIPT_KEY_LIST
from utils import PER_BLOCK_SIZE
from vendor_script import create_vendor_script_class


def type_check(arg):
    """
    Argument check, which is used to check whether the specified arg is a file.
    :param arg: the arg to check
    :return:  Check result, which is False if the arg is invalid.
    """
    if arg is not None and not os.path.exists(arg):
        UPDATE_LOGGER.print_log(
            "FileNotFoundError, path: %s" % arg, UPDATE_LOGGER.ERROR_LOG)
        return False
    return arg


def private_key_check(arg):
    """
    Argument check, which is used to check whether
    the specified arg is a private_key.
    :param arg:  The arg to check.
    :return: Check result, which is False if the arg is invalid.
    """
    if arg != "ON_SERVER" and not os.path.isfile(arg):
        UPDATE_LOGGER.print_log(
            "FileNotFoundError, path: %s" % arg, UPDATE_LOGGER.ERROR_LOG)
        return False
    return arg


def check_update_package(arg):
    """
    Argument check, which is used to check whether
    the update package path exists.
    :param arg: The arg to check.
    :return: Check result
    """
    make_dir_path = None
    if os.path.exists(arg):
        if os.path.isfile(arg):
            UPDATE_LOGGER.print_log(
                "Update package must be a dir path, not a file path. "
                "path: %s" % arg, UPDATE_LOGGER.ERROR_LOG)
            return False
    else:
        try:
            UPDATE_LOGGER.print_log(
                "Update package path does  not exist. The dir will be created!"
                "path: %s" % arg, UPDATE_LOGGER.WARNING_LOG)
            os.makedirs(arg)
            make_dir_path = arg
        except OSError:
            UPDATE_LOGGER.print_log(
                "Make update package path dir failed! "
                "path: %s" % arg, UPDATE_LOGGER.ERROR_LOG)
            return False
    if make_dir_path is not None:
        OPTIONS_MANAGER.make_dir_path = make_dir_path
    return arg


def create_entrance_args():
    """
    Arguments for the tool to create an update package
    :return source_package : source version package
            target_package : target version package
            update_package : update package output path
            no_zip : whether to enable the update package zip function.
            partition_file : partition table XML file
            signing_algorithm : signature algorithm (ECC and RSA (default))
            private_key : path of the private key file
    """
    description = "Tool for creating update package."
    parser = argparse.ArgumentParser(description=description)
    parser.add_argument("-s", "--source_package", type=type_check,
                        default=None, help="Source package file path.")
    parser.add_argument("target_package", type=type_check,
                        help="Target package file path.")
    parser.add_argument("update_package", type=check_update_package,
                        help="Update package file path.")
    parser.add_argument("-nz", "--no_zip", action='store_true',
                        help="No zip mode, Output update package without zip.")
    parser.add_argument("-pf", "--partition_file", default=None,
                        help="Variable partition mode, "
                             "Partition list file path.")
    parser.add_argument("-sa", "--signing_algorithm", default='RSA',
                        choices=['ECC', 'RSA'],
                        help="The signing algorithm "
                             "supported by the tool include ['ECC', 'RSA'].")
    parser.add_argument("-ha", "--hash_algorithm", default='sha256',
                        choices=['sha256', 'sha384'],
                        help="The hash algorithm "
                             "supported by the tool include "
                             "['sha256', 'sha384'].")
    parser.add_argument("-pk", "--private_key", type=private_key_check,
                        default=None, help="Private key file path.")
    args = parser.parse_args()
    source_package = args.source_package
    OPTIONS_MANAGER.source_package = source_package
    target_package = args.target_package
    OPTIONS_MANAGER.target_package = target_package
    update_package = args.update_package
    OPTIONS_MANAGER.update_package = update_package
    no_zip = args.no_zip
    OPTIONS_MANAGER.no_zip = no_zip
    partition_file = args.partition_file
    OPTIONS_MANAGER.partition_file = partition_file
    signing_algorithm = args.signing_algorithm
    OPTIONS_MANAGER.signing_algorithm = signing_algorithm
    hash_algorithm = args.hash_algorithm
    OPTIONS_MANAGER.hash_algorithm = hash_algorithm
    private_key = args.private_key
    OPTIONS_MANAGER.private_key = private_key
    return source_package, target_package, update_package, no_zip, \
        partition_file, signing_algorithm, hash_algorithm, private_key


def get_script_obj(script_obj=None):
    """
    获取Opera script对象
    :return:
    """
    script_obj_list = create_vendor_script_class()
    if script_obj_list == [None] * len(SCRIPT_KEY_LIST) and script_obj is None:
        prelude_script = PreludeScript()
        verse_script = VerseScript()
        refrain_script = RefrainScript()
        ending_script = EndingScript()
    else:
        if script_obj_list == [None] * len(SCRIPT_KEY_LIST):
            script_obj_list = script_obj
        UPDATE_LOGGER.print_log(
            "Get vendor extension object completed!"
            "The vendor extension script will be generated.")
        prelude_script = script_obj_list[0]
        verse_script = script_obj_list[1]
        refrain_script = script_obj_list[2]
        ending_script = script_obj_list[3]
    return prelude_script, verse_script, refrain_script, ending_script


def check_incremental_args(no_zip, partition_file, source_package):
    """
    When the incremental list is not empty, incremental processing is required.
    In this case, check related arguments.
    :param no_zip:
    :param partition_file:
    :param source_package:
    :return:
    """
    if source_package is None:
        UPDATE_LOGGER.print_log(
            "The source package is missing, "
            "cannot be incrementally processed!",
            UPDATE_LOGGER.ERROR_LOG)
        clear_resource(err_clear=True)
        return False
    if no_zip:
        UPDATE_LOGGER.print_log(
            "No ZIP mode, cannot be incrementally processed!",
            UPDATE_LOGGER.ERROR_LOG)
        clear_resource(err_clear=True)
        return False
    if partition_file is not None:
        UPDATE_LOGGER.print_log(
            "Partition file is not None, "
            "cannot be incrementally processed!",
            UPDATE_LOGGER.ERROR_LOG)
        clear_resource(err_clear=True)
        return False

    OPTIONS_MANAGER.source_package_temp_obj, \
        OPTIONS_MANAGER.source_package_dir = \
        unzip_package(source_package, origin='source')
    xml_path = ''
    if OPTIONS_MANAGER.source_package_dir is not False:
        xml_path = os.path.join(OPTIONS_MANAGER.source_package_dir,
                                UPDATER_CONFIG, XML_FILE_PATH)
    if OPTIONS_MANAGER.source_package_dir is False:
        OPTIONS_MANAGER.source_package_temp_obj = None
        OPTIONS_MANAGER.source_package_dir = None
    if os.path.exists(xml_path):
        with open(xml_path, 'r') as xml_file:
            xml_str = xml_file.read()
    else:
        UPDATE_LOGGER.print_log("XML file does not exist! xml path: %s" %
                                xml_path, UPDATE_LOGGER.ERROR_LOG)
        return False
    xml_content_dict = xmltodict.parse(xml_str, encoding='utf-8')
    package_dict = xml_content_dict.get('package', {})
    head_dict = package_dict.get('head', {}).get('info')
    OPTIONS_MANAGER.source_package_version = head_dict.get("@softVersion")
    if check_package_version(OPTIONS_MANAGER.target_package_version,
                             OPTIONS_MANAGER.source_package_version) is False:
        clear_resource(err_clear=True)
        return False
    return True


def check_userdata_image():
    """
    Check the userdata image. Updating this image is prohibited.
    :return:
    """
    if 'userdata' in OPTIONS_MANAGER.full_img_list or \
            'userdata' in OPTIONS_MANAGER.incremental_img_list:
        UPDATE_LOGGER.print_log(
            "userdata image does not participate in update!"
            "Please check xml config, path: %s!" %
            os.path.join(OPTIONS_MANAGER.target_package_config_dir,
                         XML_FILE_PATH),
            UPDATE_LOGGER.ERROR_LOG)
        clear_resource(err_clear=True)
        return False
    return True


def check_images_list():
    """
    Check full_img_list and incremental_img_list.
    If their lengths are 0, an error will be logged.
    :return:
    """
    if len(OPTIONS_MANAGER.full_img_list) == 0 and \
            len(OPTIONS_MANAGER.incremental_img_list) == 0:
        UPDATE_LOGGER.print_log(
            "The image list is empty!"
            "Please check xml config, path: %s!" %
            os.path.join(OPTIONS_MANAGER.target_package_config_dir,
                         XML_FILE_PATH),
            UPDATE_LOGGER.ERROR_LOG)
        clear_resource(err_clear=True)
        return False
    return True


def check_target_package_path(target_package):
    """
    Check the target_package path.
    :param target_package: target package path
    :return:
    """
    if os.path.isdir(target_package):
        OPTIONS_MANAGER.target_package_dir = target_package
        temp_dir_list = os.listdir(target_package)
        if UPDATER_CONFIG in temp_dir_list:
            OPTIONS_MANAGER.target_package_config_dir = \
                os.path.join(target_package, UPDATER_CONFIG)
        else:
            UPDATE_LOGGER.print_log(
                "Exception's target package path! path: %s" %
                target_package, UPDATE_LOGGER.ERROR_LOG)
            return False
    elif target_package.endswith('.zip'):
        # Decompress the target package.
        tmp_dir_obj, unzip_dir = unzip_package(target_package)
        if tmp_dir_obj is False or unzip_dir is False:
            clear_resource(err_clear=True)
            return False
        OPTIONS_MANAGER.target_package_dir = unzip_dir
        OPTIONS_MANAGER.target_package_temp_obj = tmp_dir_obj
        OPTIONS_MANAGER.target_package_config_dir = \
            os.path.join(unzip_dir, UPDATER_CONFIG)
    else:
        UPDATE_LOGGER.print_log(
            "Input Update Package type exception! path: %s" %
            target_package, UPDATE_LOGGER.ERROR_LOG)
        clear_resource(err_clear=True)
        return False
    return True


def check_miss_private_key(private_key):
    """
    Check private key.
    :param private_key:
    :return:
    """
    if private_key is None:
        UPDATE_LOGGER.print_log(
            "Private key is None, update package cannot be signed! "
            "Please specify the signature private key by -pk.",
            UPDATE_LOGGER.ERROR_LOG)
        clear_resource(err_clear=True)
        return False
    return True


def check_package_version(target_ver, source_ver):
    """
    target_ver: target version
    source_ver: source version
    return:
    """
    try:
        target_num = ''.join(target_ver.split(' ')[-1].split('.')[1:3])
        source_num = ''.join(source_ver.split(' ')[-1].split('.')[1:3])
        if int(target_num) <= int(source_num):
            UPDATE_LOGGER.print_log(
                'Target package version %s <= Source package version!'
                'Unable to make updater package!',
                UPDATE_LOGGER.ERROR_LOG)
            return False
    except ValueError:
        UPDATE_LOGGER.print_log('your package version number is not compliant.'
                                'Please check your package version number!',
                                UPDATE_LOGGER.ERROR_LOG)
        return False
    return True


def increment_image_processing(
        verse_script, incremental_img_list, source_package_dir,
        target_package_dir):
    """
    Incremental image processing
    :param verse_script: verse script
    :param incremental_img_list: incremental image list
    :param source_package_dir: source package path
    :param target_package_dir: target package path
    :return:
    """
    script_check_cmd_list = []
    script_write_cmd_list = []
    patch_process = None
    for each_img in incremental_img_list:
        each_src_image_path = \
            os.path.join(source_package_dir,
                         '%s.img' % each_img)
        each_src_map_path = \
            os.path.join(source_package_dir,
                         '%s.map' % each_img)
        each_tgt_image_path = \
            os.path.join(target_package_dir,
                         '%s.img' % each_img)
        each_tgt_map_path = \
            os.path.join(target_package_dir,
                         '%s.map' % each_img)
        if not os.path.exists(each_src_image_path):
            UPDATE_LOGGER.print_log(
                "The source %s.img file is missing from the source package, "
                "the component: %s cannot be incrementally processed. "
                "path: %s!" %
                (each_img, each_img,
                 os.path.join(source_package_dir, UPDATER_CONFIG,
                              XML_FILE_PATH)),
                UPDATE_LOGGER.ERROR_LOG)
            clear_resource(err_clear=True)
            return False

        check_make_map_path(each_img)
        cmd = ["e2fsdroid", "-B", each_src_map_path,
               "-a", "/%s" % each_img, each_src_image_path]
        sub_p = subprocess.Popen(
            cmd, shell=False, stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT)
        sub_p.wait()

        if not os.path.exists(each_tgt_image_path):
            UPDATE_LOGGER.print_log(
                "The target %s.img file is missing from the target package, "
                "the component: %s cannot be incrementally processed. "
                "Please check xml config, path: %s!" %
                (each_img, each_img,
                 os.path.join(target_package_dir, UPDATER_CONFIG,
                              XML_FILE_PATH)),
                UPDATE_LOGGER.ERROR_LOG)
            clear_resource(err_clear=True)
            return False

        cmd = ["e2fsdroid", "-B", each_tgt_map_path,
               "-a", "/%s" % each_img, each_tgt_image_path]
        sub_p = subprocess.Popen(
            cmd, shell=False, stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT)
        sub_p.wait()

        if filecmp.cmp(each_src_image_path, each_tgt_image_path):
            UPDATE_LOGGER.print_log(
                "Source Image is the same as Target Image!"
                "src image path: %s, tgt image path: %s" %
                (each_src_image_path, each_tgt_image_path),
                UPDATE_LOGGER.ERROR_LOG)
            clear_resource(err_clear=True)
            return False

        src_sparse_image = SparseImage(each_src_image_path, each_src_map_path)
        tgt_sparse_image = SparseImage(each_tgt_image_path, each_tgt_map_path)

        transfers_manager = TransfersManager(
            each_img, tgt_sparse_image, src_sparse_image)
        transfers_manager.find_process_needs()
        actions_list = transfers_manager.get_action_list()

        graph_process = GigraphProcess(actions_list, src_sparse_image,
                                       tgt_sparse_image)
        actions_list = copy.deepcopy(graph_process.actions_list)
        patch_process = PatchProcess(each_img, tgt_sparse_image,
                                     src_sparse_image,
                                     actions_list)
        patch_process.patch_process()
        patch_process.package_patch_zip.package_patch_zip()
        patch_process.write_script(each_img, script_check_cmd_list,
                                   script_write_cmd_list, verse_script)
    if not check_patch_file(patch_process):
        UPDATE_LOGGER.print_log(
            'Verify the incremental result failed!',
            UPDATE_LOGGER.ERROR_LOG)
        raise RuntimeError
    UPDATE_LOGGER.print_log(
            'Verify the incremental result successfully!',
            UPDATE_LOGGER.INFO_LOG)

    verse_script.add_command(
        "\n# ---- start incremental check here ----\n")
    for each_check_cmd in script_check_cmd_list:
        verse_script.add_command(each_check_cmd)
    verse_script.add_command(
        "\n# ---- start incremental write here ----\n")
    for each_write_cmd in script_write_cmd_list:
        verse_script.add_command(each_write_cmd)
    return True


def check_patch_file(patch_process):
    new_dat_file_obj, patch_dat_file_obj, transfer_list_file_obj = \
        patch_process.package_patch_zip.get_file_obj()
    with open(transfer_list_file_obj.name) as f_t:
        num = 0
        diff_str = None
        diff_num = 0
        for line in f_t:
            if 'new' in line:
                num_list = line.split('\n')[0].split(',')
                child_num = (int(num_list[-1]) - int(num_list[-2]))
                num += child_num
            if 'diff' in line:
                diff_str = line
        if diff_str:
            diff_list = diff_str.split('\n')[0].split(' ')
            diff_num = int(diff_list[1]) + int(diff_list[2])
    check_flag = \
        (os.path.getsize(new_dat_file_obj.name) == num * PER_BLOCK_SIZE) and \
        (os.path.getsize(patch_dat_file_obj.name) == diff_num)
    return check_flag


def check_make_map_path(each_img):
    """
    If env does not exist, the command for map generation does not exist
    in the environment variable, and False will be returned.
    """
    try:
        cmd = ["e2fsdroid", " -h"]
        subprocess.Popen(cmd, shell=False, stdout=subprocess.PIPE,
                         stderr=subprocess.STDOUT)
    except FileNotFoundError:
        UPDATE_LOGGER.print_log(
            "Command not found, need check the env! "
            "Make %s.map failed!" % each_img,
            UPDATE_LOGGER.ERROR_LOG)
        clear_resource(err_clear=True)
        raise RuntimeError
    return True


def main():
    """
    Entry function.
    """
    OPTIONS_MANAGER.product = PRODUCT

    source_package, target_package, update_package, no_zip, \
        partition_file, signing_algorithm, hash_algorithm, private_key = \
        create_entrance_args()
    if source_package is False or private_key is False or \
            target_package is False or update_package is False:
        return

    if check_miss_private_key(private_key) is False:
        clear_resource(err_clear=True)
        return

    if check_target_package_path(target_package) is False:
        clear_resource(err_clear=True)
        return

    if get_update_info() is False:
        clear_resource(err_clear=True)
        return

    if check_images_list() is False:
        clear_resource(err_clear=True)
        return

    if check_userdata_image() is False:
        clear_resource(err_clear=True)
        return

    # Create a Script object.
    prelude_script, verse_script, refrain_script, ending_script = \
        get_script_obj()

    # Create partition.
    if partition_file is not None:
        verse_script.add_command("\n# ---- do updater partitions ----\n")
        updater_partitions_cmd = verse_script.updater_partitions()
        verse_script.add_command(updater_partitions_cmd)

        partition_file_obj, partitions_list = \
            parse_partition_file_xml(partition_file)
        if partition_file_obj is False:
            clear_resource(err_clear=True)
            return False
        OPTIONS_MANAGER.partition_file_obj = partition_file_obj
        OPTIONS_MANAGER.full_img_list = partitions_list
        OPTIONS_MANAGER.two_step = False

    # Upgrade the updater image.
    if OPTIONS_MANAGER.two_step:
        get_status_cmd = verse_script.get_status()
        set_status_0_cmd = verse_script.set_status('0')
        set_status_1_cmd = verse_script.set_status('1')
        reboot_now_cmd = verse_script.reboot_now()
        create_updater_script_command = \
            '\n# ---- do updater partitions ----\n\n' \
            'if ({get_status_cmd} == 0){{\nUPDATER_WRITE_FLAG\n' \
            '    {set_status_1_cmd}    {reboot_now_cmd}}}\n' \
            'else{{    \nALL_WRITE_FLAG\n    {set_status_0_cmd}}}'.format(
                get_status_cmd=get_status_cmd,
                set_status_1_cmd=set_status_1_cmd,
                set_status_0_cmd=set_status_0_cmd,
                reboot_now_cmd=reboot_now_cmd)
        verse_script.add_command(create_updater_script_command)

    if len(OPTIONS_MANAGER.incremental_img_list) != 0:
        if check_incremental_args(no_zip, partition_file, source_package)\
                is False:
            clear_resource(err_clear=True)
            return
        if increment_image_processing(
                verse_script, OPTIONS_MANAGER.incremental_img_list,
                OPTIONS_MANAGER.source_package_dir,
                OPTIONS_MANAGER.target_package_dir) is False:
            clear_resource(err_clear=True)
            return

    # Full processing
    if len(OPTIONS_MANAGER.full_img_list) != 0:
        verse_script.add_command("\n# ---- full image ----\n")
        full_image_content_len_list, full_image_file_obj_list = \
            FullUpdateImage(OPTIONS_MANAGER.target_package_dir,
                            OPTIONS_MANAGER.full_img_list, verse_script,
                            no_zip=OPTIONS_MANAGER.no_zip).\
            update_full_image()
        if full_image_content_len_list is False or \
                full_image_file_obj_list is False:
            clear_resource(err_clear=True)
            return
        OPTIONS_MANAGER.full_image_content_len_list, \
            OPTIONS_MANAGER.full_image_file_obj_list = \
            full_image_content_len_list, full_image_file_obj_list

    # Generate the update package.
    build_re = build_update_package(no_zip, update_package,
                                    prelude_script, verse_script,
                                    refrain_script, ending_script)
    if build_re is False:
        clear_resource(err_clear=True)
        return
    # Clear resources.
    clear_resource()


if __name__ == '__main__':
    main()
