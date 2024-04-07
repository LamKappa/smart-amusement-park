/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import fileio from '@ohos.fileio'

import { describe, beforeAll, beforeEach, afterEach, afterAll, it, expect } from 'deccjsunit/index'
import { FILE_CONTENT, prepareFile, nextFileName } from './Common'

describe('fileIOTestStat', function () {
    /**
     * @tc.number SUB_DF_FileIO_Stat_FstatSync_0000
     * @tc.name fileio_test_stat_fstat_sync_000
     * @tc.desc Test fileio.fstatSync() interface.
     */
    it('fileio_test_stat_fstat_sync_000', 0, function () {
        let fpath = nextFileName('fileio_test_stat_fstat_sync_000')
        expect(prepareFile(fpath, FILE_CONTENT)).assertTrue()

        try {
            let fd = fileio.openSync(fpath, 0o2)
            expect(fd).assertInstanceOf('Number')
            let stat = fileio.fstatSync(fd)
            expect(stat !== null).assertTrue()
            expect(fileio.closeSync(fd)).assertNull()
            expect(fileio.unlinkSync(fpath)).assertNull()
        } catch (e) {
            console.log("fileio_test_stat_stat_sync_000 has failed for " + e)
            expect(null).assertFail()
        }
    })

    /**
     * @tc.number SUB_DF_FileIO_Stat_FstatSync_0010
     * @tc.name fileio_test_stat_fstat_sync_001
     * @tc.desc Test fstatSync() interface.
     */
    it('fileio_test_stat_fstat_sync_001', 0, function () {
        try {
            let invalidFD = -1
            fileio.fstatSync(invalidFD)
            expect(null).assertFail()
        } catch (e) {
        }
    })

    /**
     * @tc.number SUB_DF_FileIO_Stat_StatSync_0000
     * @tc.name fileio_test_stat_stat_sync_000
     * @tc.desc Test Stat.statSync() interface.
     */
    it('fileio_test_stat_stat_sync_000', 0, function () {
        let fpath = nextFileName('fileio_test_stat_stat_sync_000')
        expect(prepareFile(fpath, FILE_CONTENT)).assertTrue()

        try {
            let stat = fileio.Stat.statSync(fpath)
            expect(stat !== null).assertTrue()
            expect(fileio.unlinkSync(fpath)).assertNull()
        } catch (e) {
            console.log("fileio_test_stat_stat_sync_000 has failed for " + e)
            expect(null).assertFail()
        }
    })

    /**
     * @tc.number SUB_DF_FileIO_Stat_StatSync_0010
     * @tc.name fileio_test_stat_stat_sync_001
     * @tc.desc Test Stat.statSync() interface.
     */
    it('fileio_test_stat_stat_sync_001', 0, function () {
        let fpath = nextFileName('fileio_test_stat_stat_sync_001')

        try {
            let stat = fileio.Stat.statSync(fpath)
            expect(null).assertFail()
        } catch (e) {
        }
    })

    /**
     * @tc.number SUB_DF_FileIO_Stat_Dev_0000
     * @tc.name fileio_test_stat_dev_000
     * @tc.desc Test Stat.dev() interface.
     */
    it('fileio_test_stat_dev_000', 0, function () {
        let fpath = nextFileName('fileio_test_stat_dev_000')
        expect(prepareFile(fpath, FILE_CONTENT)).assertTrue()

        try {
            let stat = fileio.Stat.statSync(fpath)
            expect(stat.dev).assertInstanceOf('Number')
            expect(fileio.unlinkSync(fpath)).assertNull()
        } catch (e) {
            console.log("fileio_test_stat_dev_000 has failed for " + e)
            expect(null).assertFail()
        }
    })

    /**
     * @tc.number SUB_DF_FileIO_Stat_Ino_0000
     * @tc.name fileio_test_stat_ino_000
     * @tc.desc Test Stat.ino() interface.
     */
    it('fileio_test_stat_ino_000', 0, function () {
        let fpath = nextFileName('fileio_test_stat_ino_000')
        expect(prepareFile(fpath, FILE_CONTENT)).assertTrue()

        try {
            let stat = fileio.Stat.statSync(fpath)
            expect(stat.ino).assertInstanceOf('Number')
            expect(fileio.unlinkSync(fpath)).assertNull()
        } catch (e) {
            console.log("fileio_test_stat_ino_000 has failed for " + e)
            expect(null).assertFail()
        }
    })

    /**
     * @tc.number SUB_DF_FileIO_Stat_Mode_0000
     * @tc.name fileio_test_stat_mode_000
     * @tc.desc Test Stat.mode() interface.
     */
    it('fileio_test_stat_mode_000', 0, function () {
        let fpath = nextFileName('fileio_test_stat_mode_000')
        expect(prepareFile(fpath, FILE_CONTENT)).assertTrue()

        try {
            let stat = fileio.Stat.statSync(fpath)
            expect(stat.mode).assertInstanceOf('Number')
            expect(fileio.unlinkSync(fpath)).assertNull()
        } catch (e) {
            console.log("fileio_test_stat_mode_000 has failed for " + e)
            expect(null).assertFail()
        }
    })

    /**
     * @tc.number SUB_DF_FileIO_Stat_Nlink_0000
     * @tc.name fileio_test_stat_nlink_000
     * @tc.desc Test Stat.nlink() interface.
     */
    it('fileio_test_stat_nlink_000', 0, function () {
        let fpath = nextFileName('fileio_test_stat_nlink_000')
        expect(prepareFile(fpath, FILE_CONTENT)).assertTrue()

        try {
            let stat = fileio.Stat.statSync(fpath)
            expect(stat.nlink).assertInstanceOf('Number')
            expect(fileio.unlinkSync(fpath)).assertNull()
        } catch (e) {
            console.log("fileio_test_stat_nlink_000 has failed for " + e)
            expect(null).assertFail()
        }
    })

    /**
     * @tc.number SUB_DF_FileIO_Stat_Uid_0000
     * @tc.name fileio_test_stat_uid_000
     * @tc.desc Test Stat.uid() interface.
     */
    it('fileio_test_stat_uid_000', 0, function () {
        let fpath = nextFileName('fileio_test_stat_uid_000')
        expect(prepareFile(fpath, FILE_CONTENT)).assertTrue()

        try {
            let stat = fileio.Stat.statSync(fpath)
            expect(stat.uid).assertInstanceOf('Number')
            expect(fileio.unlinkSync(fpath)).assertNull()
        } catch (e) {
            console.log("fileio_test_stat_uid_000 has failed for " + e)
            expect(null).assertFail()
        }
    })

    /**
     * @tc.number SUB_DF_FileIO_Stat_Gid_0000
     * @tc.name fileio_test_stat_gid_000
     * @tc.desc Test Stat.gid() interface.
     */
    it('fileio_test_stat_gid_000', 0, function () {
        let fpath = nextFileName('fileio_test_stat_gid_000')
        expect(prepareFile(fpath, FILE_CONTENT)).assertTrue()

        try {
            let stat = fileio.Stat.statSync(fpath)
            expect(stat.gid).assertInstanceOf('Number')
            expect(fileio.unlinkSync(fpath)).assertNull()
        } catch (e) {
            console.log("fileio_test_stat_gid_000 has failed for " + e)
            expect(null).assertFail()
        }
    })

    /**
     * @tc.number SUB_DF_FileIO_Stat_Rdev_0000
     * @tc.name fileio_test_stat_rdev_000
     * @tc.desc Test Stat.rdev() interface.
     */
    it('fileio_test_stat_rdev_000', 0, function () {
        let fpath = nextFileName('fileio_test_stat_rdev_000')
        expect(prepareFile(fpath, FILE_CONTENT)).assertTrue()

        try {
            let stat = fileio.Stat.statSync(fpath)
            expect(stat.rdev).assertInstanceOf('Number')
            expect(fileio.unlinkSync(fpath)).assertNull()
        } catch (e) {
            console.log("fileio_test_stat_rdev_000 has failed for " + e)
            expect(null).assertFail()
        }
    })

    /**
     * @tc.number SUB_DF_FileIO_Stat_Size_0000
     * @tc.name fileio_test_stat_size_000
     * @tc.desc Test Stat.size() interface.
     */
    it('fileio_test_stat_size_000', 0, function () {
        let fpath = nextFileName('fileio_test_stat_size_000')
        expect(prepareFile(fpath, FILE_CONTENT)).assertTrue()

        try {
            let stat = fileio.Stat.statSync(fpath)
            expect(stat.size).assertInstanceOf('Number')
            expect(fileio.unlinkSync(fpath)).assertNull()
        } catch (e) {
            console.log("fileio_test_stat_size_000 has failed for " + e)
            expect(null).assertFail()
        }
    })

    /**
     * @tc.number SUB_DF_FileIO_Stat_Blocks_0000
     * @tc.name fileio_test_stat_blocks_000
     * @tc.desc Test Stat.blocks() interface.
     */
    it('fileio_test_stat_blocks_000', 0, function () {
        let fpath = nextFileName('fileio_test_stat_blocks_000')
        expect(prepareFile(fpath, FILE_CONTENT)).assertTrue()

        try {
            let stat = fileio.Stat.statSync(fpath)
            expect(stat.blocks).assertInstanceOf('Number')
            expect(fileio.unlinkSync(fpath)).assertNull()
        } catch (e) {
            console.log("fileio_test_stat_blocks_000 has failed for " + e)
            expect(null).assertFail()
        }
    })

    /**
     * @tc.number SUB_DF_FileIO_Stat_Atime_0000
     * @tc.name fileio_test_stat_atime_000
     * @tc.desc Test Stat.atime() interface.
     */
    it('fileio_test_stat_atime_000', 0, function () {
        let fpath = nextFileName('fileio_test_stat_atime_000')
        expect(prepareFile(fpath, FILE_CONTENT)).assertTrue()

        try {
            let stat = fileio.Stat.statSync(fpath)
            expect(stat.atime).assertInstanceOf('Number')
            expect(fileio.unlinkSync(fpath)).assertNull()
        } catch (e) {
            console.log("fileio_test_stat_atime_000 has failed for " + e)
            expect(null).assertFail()
        }
    })

    /**
     * @tc.number SUB_DF_FileIO_Stat_Mtime_0000
     * @tc.name fileio_test_stat_mtime_000
     * @tc.desc Test Stat.mtime() interface.
     */
    it('fileio_test_stat_mtime_000', 0, function () {
        let fpath = nextFileName('fileio_test_stat_mtime_000')
        expect(prepareFile(fpath, FILE_CONTENT)).assertTrue()

        try {
            let stat = fileio.Stat.statSync(fpath)
            expect(stat.mtime).assertInstanceOf('Number')
            expect(fileio.unlinkSync(fpath)).assertNull()
        } catch (e) {
            console.log("fileio_test_stat_mtime_000 has failed for " + e)
            expect(null).assertFail()
        }
    })

    /**
     * @tc.number SUB_DF_FileIO_Stat_Ctime_0000
     * @tc.name fileio_test_stat_ctime_000
     * @tc.desc Test Stat.ctime() interface.
     */
    it('fileio_test_stat_ctime_000', 0, function () {
        let fpath = nextFileName('fileio_test_stat_ctime_000')
        expect(prepareFile(fpath, FILE_CONTENT)).assertTrue()

        try {
            let stat = fileio.Stat.statSync(fpath)
            expect(stat.ctime).assertInstanceOf('Number')
            expect(fileio.unlinkSync(fpath)).assertNull()
        } catch (e) {
            console.log("fileio_test_stat_ctime_000 has failed for " + e)
            expect(null).assertFail()
        }
    })

    /**
     * @tc.number SUB_DF_FileIO_Stat_IsBlockDevice_0000
     * @tc.name fileio_test_stat_is_block_device_000
     * @tc.desc Test Stat.isBlockDevice() interface.
     */
    it('fileio_test_stat_is_block_device_000', 0, function () {
        let fpath = nextFileName('fileio_test_stat_is_block_device_000')
        expect(prepareFile(fpath, FILE_CONTENT)).assertTrue()

        try {
            let stat = fileio.Stat.statSync(fpath)
            expect(stat.isBlockDevice()).assertInstanceOf('Boolean')
            expect(fileio.unlinkSync(fpath)).assertNull()
        } catch (e) {
            console.log("fileio_test_stat_is_block_device_000 has failed for " + e)
            expect(null).assertFail()
        }
    })

    /**
     * @tc.number SUB_DF_FileIO_Stat_IsBlockDevice_0010
     * @tc.name fileio_test_stat_is_block_device_001
     * @tc.desc Test Stat.isBlockDevice() interface.
     */
    it('fileio_test_stat_is_block_device_001', 0, function () {
        let fpath = nextFileName('fileio_test_stat_is_block_device_001')
        expect(prepareFile(fpath, FILE_CONTENT)).assertTrue()

        try {
            let stat = fileio.Stat.statSync(fpath)
            expect(stat.isBlockDevice()).assertFalse()
            expect(fileio.unlinkSync(fpath)).assertNull()
        } catch (e) {
            console.log("fileio_test_stat_is_block_device_001 has failed for " + e)
            expect(null).assertFail()
        }
    })

    /**
     * @tc.number SUB_DF_FileIO_Stat_IsBlockDevice_0020
     * @tc.name fileio_test_stat_is_block_device_002
     * @tc.desc Test Stat.isBlockDevice() interface.
     */
    it('fileio_test_stat_is_block_device_002', 0, function () {
        let fpath = nextFileName('fileio_test_stat_is_block_device_002')
        expect(prepareFile(fpath, FILE_CONTENT)).assertTrue()

        try {
            let stat = fileio.Stat.statSync(fpath)
            expect(stat.isBlockDevice(-1)).assertFalse()
            expect(null).assertFail()
        } catch (e) {
            expect(fileio.unlinkSync(fpath)).assertNull()
        }
    })

    /**
     * @tc.number SUB_DF_FileIO_Stat_IsCharacterDevice_0000
     * @tc.name fileio_test_stat_is_character_device_000
     * @tc.desc Test Stat.isCharacterDevice() interface.
     */
    it('fileio_test_stat_is_character_device_000', 0, function () {
        let fpath = nextFileName('fileio_test_stat_is_character_device_000')
        expect(prepareFile(fpath, FILE_CONTENT)).assertTrue()

        try {
            let stat = fileio.Stat.statSync(fpath)
            expect(stat.isCharacterDevice()).assertInstanceOf('Boolean')
            expect(fileio.unlinkSync(fpath)).assertNull()
        } catch (e) {
            console.log("fileio_test_stat_is_character_device_000 has failed for " + e)
            expect(null).assertFail()
        }
    })

    /**
     * @tc.number SUB_DF_FileIO_Stat_IsCharacterDevice_0010
     * @tc.name fileio_test_stat_is_character_device_001
     * @tc.desc Test Stat.isCharacterDevice() interface.
     */
    it('fileio_test_stat_is_character_device_001', 0, function () {
        let fpath = nextFileName('fileio_test_stat_is_character_device_001')
        expect(prepareFile(fpath, FILE_CONTENT)).assertTrue()

        try {
            let stat = fileio.Stat.statSync(fpath)
            expect(stat.isCharacterDevice()).assertFalse()
            expect(fileio.unlinkSync(fpath)).assertNull()
        } catch (e) {
            console.log("fileio_test_stat_is_character_device_001 has failed for " + e)
            expect(null).assertFail()
        }
    })

    /**
     * @tc.number SUB_DF_FileIO_Stat_IsCharacterDevice_0020
     * @tc.name fileio_test_stat_is_character_device_002
     * @tc.desc Test Stat.isCharacterDevice() interface.
     */
    it('fileio_test_stat_is_character_device_002', 0, function () {
        let fpath = nextFileName('fileio_test_stat_is_character_device_002')
        expect(prepareFile(fpath, FILE_CONTENT)).assertTrue()

        try {
            let stat = fileio.Stat.statSync(fpath)
            expect(stat.isCharacterDevice(-1)).assertFalse()
            expect(null).assertFail()
        } catch (e) {
            expect(fileio.unlinkSync(fpath)).assertNull()
        }
    })

    /**
     * @tc.number SUB_DF_FileIO_Stat_IsDirectory_0000
     * @tc.name fileio_test_stat_is_directory_000
     * @tc.desc Test Stat.isDirectory() interface.
     */
    it('fileio_test_stat_is_directory_000', 0, function () {
        let fpath = nextFileName('fileio_test_stat_is_directory_000')
        expect(prepareFile(fpath, FILE_CONTENT)).assertTrue()

        try {
            let stat = fileio.Stat.statSync(fpath)
            expect(stat.isDirectory()).assertInstanceOf('Boolean')
            expect(fileio.unlinkSync(fpath)).assertNull()
        } catch (e) {
            console.log("fileio_test_stat_is_directory_000 has failed for " + e)
            expect(null).assertFail()
        }
    })

    /**
     * @tc.number SUB_DF_FileIO_Stat_IsDirectory_0010
     * @tc.name fileio_test_stat_is_directory_001
     * @tc.desc Test Stat.isDirectory() interface.
     */
    it('fileio_test_stat_is_directory_001', 0, function () {
        let fpath = nextFileName('fileio_test_stat_is_directory_001')
        expect(prepareFile(fpath, FILE_CONTENT)).assertTrue()

        try {
            let stat = fileio.Stat.statSync(fpath)
            expect(stat.isDirectory()).assertFalse()
            expect(fileio.unlinkSync(fpath)).assertNull()
        } catch (e) {
            console.log("fileio_test_stat_is_directory_001 has failed for " + e)
            expect(null).assertFail()
        }
    })

    /**
     * @tc.number SUB_DF_FileIO_Stat_IsDirectory_0020
     * @tc.name fileio_test_stat_is_directory_002
     * @tc.desc Test Stat.isDirectory() interface.
     */
    it('fileio_test_stat_is_directory_002', 0, function () {
        let dpath = nextFileName('fileio_test_stat_is_directory_002') + 'd'

        try {
            expect(fileio.mkdirSync(dpath)).assertNull()
            let stat = fileio.Stat.statSync(dpath)
            expect(stat.isDirectory()).assertTrue()
            expect(fileio.rmdirSync(dpath)).assertNull()
        } catch (e) {
            console.log("fileio_test_stat_is_directory_002 has failed for " + e)
            expect(null).assertFail()
        }
    })

    /**
     * @tc.number SUB_DF_FileIO_Stat_IsDirectory_0030
     * @tc.name fileio_test_stat_is_directory_003
     * @tc.desc Test Stat.isDirectory() interface.
     */
    it('fileio_test_stat_is_directory_003', 0, function () {
        let dpath = nextFileName('fileio_test_stat_is_directory_003') + 'd'

        try {
            expect(fileio.mkdirSync(dpath)).assertNull()
            let stat = fileio.Stat.statSync(dpath)
            expect(stat.isDirectory(-1)).assertTrue()
            expect(null).assertFail()
        } catch (e) {
            expect(fileio.rmdirSync(dpath)).assertNull()
        }
    })

    /**
     * @tc.number SUB_DF_FileIO_Stat_IsFIFO_0000
     * @tc.name fileio_test_stat_is_fifo_000
     * @tc.desc Test Stat.isFIFO() interface.
     */
    it('fileio_test_stat_is_fifo_000', 0, function () {
        let fpath = nextFileName('fileio_test_stat_is_fifo_000')
        expect(prepareFile(fpath, FILE_CONTENT)).assertTrue()

        try {
            let stat = fileio.Stat.statSync(fpath)
            expect(stat.isFIFO()).assertInstanceOf('Boolean')
            expect(fileio.unlinkSync(fpath)).assertNull()
        } catch (e) {
            console.log("fileio_test_stat_is_fifo_000 has failed for " + e)
            expect(null).assertFail()
        }
    })

    /**
     * @tc.number SUB_DF_FileIO_Stat_IsFIFO_0010
     * @tc.name fileio_test_stat_is_fifo_001
     * @tc.desc Test Stat.isFIFO() interface.
     */
    it('fileio_test_stat_is_fifo_001', 0, function () {
        let fpath = nextFileName('fileio_test_stat_is_fifo_001')
        expect(prepareFile(fpath, FILE_CONTENT)).assertTrue()

        try {
            let stat = fileio.Stat.statSync(fpath)
            expect(stat.isFIFO()).assertFalse()
            expect(fileio.unlinkSync(fpath)).assertNull()
        } catch (e) {
            console.log("fileio_test_stat_is_fifo_001 has failed for " + e)
            expect(null).assertFail()
        }
    })

    /**
     * @tc.number SUB_DF_FileIO_Stat_IsFIFO_0020
     * @tc.name fileio_test_stat_is_fifo_002
     * @tc.desc Test Stat.isFIFO() interface.
     */
    it('fileio_test_stat_is_fifo_002', 0, function () {
        let fpath = nextFileName('fileio_test_stat_is_fifo_001')
        expect(prepareFile(fpath, FILE_CONTENT)).assertTrue()

        try {
            let stat = fileio.Stat.statSync(fpath)
            expect(stat.isFIFO(-1)).assertFalse()
            expect(null).assertFail()
        } catch (e) {
            expect(fileio.unlinkSync(fpath)).assertNull()
        }
    })

    /**
     * @tc.number SUB_DF_FileIO_Stat_IsFILE_0000
     * @tc.name fileio_test_stat_is_file_000
     * @tc.desc Test Stat.isFile() interface.
     */
    it('fileio_test_stat_is_file_000', 0, function () {
        let fpath = nextFileName('fileio_test_stat_is_file_000')
        expect(prepareFile(fpath, FILE_CONTENT)).assertInstanceOf('Boolean')

        try {
            let stat = fileio.Stat.statSync(fpath)
            expect(stat.isFile()).assertInstanceOf('Boolean')
            expect(fileio.unlinkSync(fpath)).assertNull()
        } catch (e) {
            console.log("fileio_test_stat_is_file_000 has failed for " + e)
            expect(null).assertFail()
        }
    })

    /**
     * @tc.number SUB_DF_FileIO_Stat_IsFILE_0010
     * @tc.name fileio_test_stat_is_file_001
     * @tc.desc Test Stat.isFile() interface.
     */
    it('fileio_test_stat_is_file_001', 0, function () {
        let fpath = nextFileName('fileio_test_stat_is_file_001')
        expect(prepareFile(fpath, FILE_CONTENT)).assertInstanceOf('Boolean')

        try {
            let stat = fileio.Stat.statSync(fpath)
            expect(stat.isFile()).assertTrue()
            expect(fileio.unlinkSync(fpath)).assertNull()
        } catch (e) {
            console.log("fileio_test_stat_is_file_001 has failed for " + e)
            expect(null).assertFail()
        }
    })

    /**
     * @tc.number SUB_DF_FileIO_Stat_IsFILE_0020
     * @tc.name fileio_test_stat_is_file_002
     * @tc.desc Test Stat.isFile() interface.
     */
    it('fileio_test_stat_is_file_002', 0, function () {
        let dpath = nextFileName('fileio_test_stat_is_file_002')

        try {
            expect(fileio.mkdirSync(dpath)).assertNull()
            let stat = fileio.Stat.statSync(dpath)
            expect(stat.isFile()).assertFalse()
            expect(fileio.rmdirSync(dpath)).assertNull()
        } catch (e) {
            console.log("fileio_test_stat_is_file_002 has failed for " + e)
            expect(null).assertFail()
        }
    })

    /**
     * @tc.number SUB_DF_FileIO_Stat_IsFILE_0030
     * @tc.name fileio_test_stat_is_file_003
     * @tc.desc Test Stat.isFile() interface.
     */
    it('fileio_test_stat_is_file_003', 0, function () {
        let dpath = nextFileName('fileio_test_stat_is_file_003')

        try {
            expect(fileio.mkdirSync(dpath)).assertNull()
            let stat = fileio.Stat.statSync(dpath)
            expect(stat.isFile(-1)).assertFalse()
            expect(null).assertFail()
        } catch (e) {
            expect(fileio.rmdirSync(dpath)).assertNull()
        }
    })

    /**
     * @tc.number SUB_DF_FileIO_Stat_IsSocket_0000
     * @tc.name fileio_test_stat_is_socket_000
     * @tc.desc Test Stat.isSocket() interface.
     */
    it('fileio_test_stat_is_socket_000', 0, function () {
        let fpath = nextFileName('fileio_test_stat_is_socket_000')
        expect(prepareFile(fpath, FILE_CONTENT)).assertTrue()

        try {
            let stat = fileio.Stat.statSync(fpath)
            expect(stat.isSocket()).assertInstanceOf('Boolean')
            expect(fileio.unlinkSync(fpath)).assertNull()
        } catch (e) {
            console.log("fileio_test_stat_is_socket_000 has failed for " + e)
            expect(null).assertFail()
        }
    })

    /**
     * @tc.number SUB_DF_FileIO_Stat_IsSocket_0010
     * @tc.name fileio_test_stat_is_socket_001
     * @tc.desc Test Stat.isSocket() interface.
     */
    it('fileio_test_stat_is_socket_001', 0, function () {
        let fpath = nextFileName('fileio_test_stat_is_socket_001')
        expect(prepareFile(fpath, FILE_CONTENT)).assertTrue()

        try {
            let stat = fileio.Stat.statSync(fpath)
            expect(stat.isSocket()).assertFalse()
            expect(fileio.unlinkSync(fpath)).assertNull()
        } catch (e) {
            console.log("fileio_test_stat_is_socket_001 has failed for " + e)
            expect(null).assertFail()
        }
    })

    /**
     * @tc.number SUB_DF_FileIO_Stat_IsSocket_0020
     * @tc.name fileio_test_stat_is_socket_002
     * @tc.desc Test Stat.isSocket() interface.
     */
    it('fileio_test_stat_is_socket_002', 0, function () {
        let fpath = nextFileName('fileio_test_stat_is_socket_002')
        expect(prepareFile(fpath, FILE_CONTENT)).assertTrue()

        try {
            let stat = fileio.Stat.statSync(fpath)
            expect(stat.isSocket(-1)).assertFalse()
            expect(null).assertFail()
        } catch (e) {
            expect(fileio.unlinkSync(fpath)).assertNull()
        }
    })

    /**
     * @tc.number SUB_DF_FileIO_Stat_IsSymbolicLink_0000
     * @tc.name fileio_test_stat_is_symbolic_link_000
     * @tc.desc Test Stat.isSymbolicLink() interface.
     */
    it('fileio_test_stat_is_symbolic_link_000', 0, function () {
        let fpath = nextFileName('fileio_test_stat_is_symbolic_link_000')
        expect(prepareFile(fpath, FILE_CONTENT)).assertTrue()

        try {
            let stat = fileio.Stat.statSync(fpath)
            expect(stat.isSymbolicLink()).assertInstanceOf('Boolean')
            expect(fileio.unlinkSync(fpath)).assertNull()
        } catch (e) {
            console.log("fileio_test_stat_is_symbolic_link_000 has failed for " + e)
            expect(null).assertFail()
        }
    })

    /**
     * @tc.number SUB_DF_FileIO_Stat_IsSymbolicLink_0010
     * @tc.name fileio_test_stat_is_symbolic_link_001
     * @tc.desc Test Stat.isSymbolicLink() interface.
     */
    it('fileio_test_stat_is_symbolic_link_001', 0, function () {
        let fpath = nextFileName('fileio_test_stat_is_symbolic_link_001')
        expect(prepareFile(fpath, FILE_CONTENT)).assertTrue()

        try {
            let stat = fileio.Stat.statSync(fpath)
            expect(stat.isSymbolicLink()).assertFalse()
            expect(fileio.unlinkSync(fpath)).assertNull()
        } catch (e) {
            console.log("fileio_test_stat_is_symbolic_link_001 has failed for " + e)
            expect(null).assertFail()
        }
    })

    /**
     * @tc.number SUB_DF_FileIO_Stat_IsSymbolicLink_0020
     * @tc.name fileio_test_stat_is_symbolic_link_002
     * @tc.desc Test Stat.isSymbolicLink() interface.
     */
    it('fileio_test_stat_is_symbolic_link_002', 0, function () {
        let fpath = nextFileName('fileio_test_stat_is_symbolic_link_002')
        expect(prepareFile(fpath, FILE_CONTENT)).assertTrue()

        try {
            let stat = fileio.Stat.statSync(fpath)
            expect(stat.isSymbolicLink(-1)).assertFalse()
            expect(null).assertFail()
        } catch (e) {
            expect(fileio.unlinkSync(fpath)).assertNull()
        }
    })
})
