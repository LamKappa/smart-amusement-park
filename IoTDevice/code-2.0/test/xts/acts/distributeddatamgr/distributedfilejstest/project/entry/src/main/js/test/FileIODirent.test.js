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
import {FILE_CONTENT, prepareFile, nextFileName} from './Common'

describe('fileIOTestDirent', function () {
    /**
     * @tc.number SUB_DF_FileIO_Dir_ReadSync_0000
     * @tc.name fileio_test_dirent_name_000
     * @tc.desc Test Dir.readSync() interface.
     */
    it('fileio_test_dirent_name_000', 0, function () {
        let dpath = nextFileName('fileio_test_dirent_name_000') + 'd'
        let fpath = dpath + '/f1'

        try {
            expect(fileio.mkdirSync(dpath)).assertNull()
            expect(prepareFile(fpath, FILE_CONTENT)).assertTrue()
            let dd = fileio.Dir.opendirSync(dpath)
            expect(dd !== null).assertTrue()
            let dirent = dd.readSync()
            expect(dirent !== null).assertTrue()
            expect(dirent.name).assertInstanceOf('String')
            expect(dd.closeSync()).assertNull()
            expect(fileio.unlinkSync(fpath)).assertNull()
            expect(fileio.rmdirSync(dpath)).assertNull()
        } catch (e) {
            console.log("fileio_test_dirent_name_000 has failed for " + e)
            expect(null).assertFail()
        }
    })

    /**
     * @tc.number SUB_DF_FileIO_Dir_IsBlockDevice_0000
     * @tc.name fileio_test_dirent_is_block_device_000
     * @tc.desc Test Dir.isBlockDevice() interface.
     */
    it('fileio_test_dirent_is_block_device_000', 0, function () {
        let dpath = nextFileName('fileio_test_dirent_is_block_device_000') + 'd'
        let fpath = dpath + '/f1'

        try {
            expect(fileio.mkdirSync(dpath)).assertNull()
            expect(prepareFile(fpath, FILE_CONTENT)).assertTrue()
            let dd = fileio.Dir.opendirSync(dpath)
            expect(dd !== null).assertTrue()
            let dirent = dd.readSync()
            expect(dirent !== null).assertTrue()
            expect(dirent.isBlockDevice()).assertInstanceOf('Boolean')
            expect(dd.closeSync()).assertNull()
            expect(fileio.unlinkSync(fpath)).assertNull()
            expect(fileio.rmdirSync(dpath)).assertNull()
        } catch (e) {
            console.log("fileio_test_dirent_is_block_device_000 has failed for " + e)
            expect(null).assertFail()
        }
    })

    /**
     * @tc.number SUB_DF_FileIO_Dir_IsBlockDevice_0010
     * @tc.name fileio_test_dirent_is_block_device_001
     * @tc.desc Test Dir.isBlockDevice() interface.
     */
    it('fileio_test_dirent_is_block_device_001', 0, function () {
        let dpath = nextFileName('fileio_test_dirent_is_block_device_001') + 'd'
        let fpath = dpath + '/f1'
        let dd

        try {
            expect(fileio.mkdirSync(dpath)).assertNull()
            expect(prepareFile(fpath, FILE_CONTENT)).assertTrue()
            dd = fileio.Dir.opendirSync(dpath)
            expect(dd !== null).assertTrue()
            let dirent = dd.readSync()
            expect(dirent !== null).assertTrue()
            dirent.isBlockDevice(-1)
            expect(null).assertFail()
        } catch (e) {
            expect(dd.closeSync()).assertNull()
            expect(fileio.unlinkSync(fpath)).assertNull()
            expect(fileio.rmdirSync(dpath)).assertNull()
        }
    })

    /**
     * @tc.number SUB_DF_FileIO_Dir_IsCharacterDevice_0000
     * @tc.name fileio_test_dirent_is_character_device_000
     * @tc.desc Test Dir.isCharacterDevice() interface.
     */
    it('fileio_test_dirent_is_character_device_000', 0, function () {
        let dpath = nextFileName('fileio_test_dirent_is_character_device_000') + 'd'
        let fpath = dpath + '/f1'

        try {
            expect(fileio.mkdirSync(dpath)).assertNull()
            expect(prepareFile(fpath, FILE_CONTENT)).assertTrue()
            let dd = fileio.Dir.opendirSync(dpath)
            expect(dd !== null).assertTrue()
            let dirent = dd.readSync()
            expect(dirent !== null).assertTrue()
            expect(dirent.isCharacterDevice()).assertInstanceOf('Boolean')
            expect(dd.closeSync()).assertNull()
            expect(fileio.unlinkSync(fpath)).assertNull()
            expect(fileio.rmdirSync(dpath)).assertNull()
        } catch (e) {
            console.log("fileio_test_dirent_is_character_device_000 has failed for " + e)
            expect(null).assertFail()
        }
    })

    /**
     * @tc.number SUB_DF_FileIO_Dir_IsCharacterDevice_0010
     * @tc.name fileio_test_dirent_is_character_device_001
     * @tc.desc Test Dir.isCharacterDevice() interface.
     */
    it('fileio_test_dirent_is_character_device_001', 0, function () {
        let dpath = nextFileName('fileio_test_dirent_is_character_device_001') + 'd'
        let fpath = dpath + '/f1'
        let dd

        try {
            expect(fileio.mkdirSync(dpath)).assertNull()
            expect(prepareFile(fpath, FILE_CONTENT)).assertTrue()
            dd = fileio.Dir.opendirSync(dpath)
            expect(dd !== null).assertTrue()
            let dirent = dd.readSync()
            expect(dirent !== null).assertTrue()
            dirent.isCharacterDevice(-1)
            expect(null).assertFail()
        } catch (e) {
            expect(dd.closeSync()).assertNull()
            expect(fileio.unlinkSync(fpath)).assertNull()
            expect(fileio.rmdirSync(dpath)).assertNull()
        }
    })

    /**
     * @tc.number SUB_DF_FileIO_Dir_IsDirectory_0000
     * @tc.name fileio_test_dirent_is_directory_000
     * @tc.desc Test Dir.isDirectory() interface.
     */
    it('fileio_test_dirent_is_directory_000', 0, function () {
        let dpath = nextFileName('fileio_test_dirent_is_directory_000') + 'd'
        let fpath = dpath + '/f1'

        try {
            expect(fileio.mkdirSync(dpath)).assertNull()
            expect(prepareFile(fpath, FILE_CONTENT)).assertTrue()
            let dd = fileio.Dir.opendirSync(dpath)
            expect(dd !== null).assertTrue()
            let dirent = dd.readSync()
            expect(dirent !== null).assertTrue()
            expect(dirent.isDirectory()).assertInstanceOf('Boolean')
            expect(dd.closeSync()).assertNull()
            expect(fileio.unlinkSync(fpath)).assertNull()
            expect(fileio.rmdirSync(dpath)).assertNull()
        } catch (e) {
            console.log("fileio_test_dirent_is_directory_000 has failed for " + e)
            expect(null).assertFail()
        }
    })

    /**
     * @tc.number SUB_DF_FileIO_Dir_IsDirectory_0010
     * @tc.name fileio_test_dirent_is_directory_001
     * @tc.desc Test Dir.isDirectory() interface.
     */
    it('fileio_test_dirent_is_directory_001', 0, function () {
        let dpath = nextFileName('fileio_test_dirent_is_directory_001') + 'd'
        let fpath = dpath + '/f1'
        let dd

        try {
            expect(fileio.mkdirSync(dpath)).assertNull()
            expect(prepareFile(fpath, FILE_CONTENT)).assertTrue()
            dd = fileio.Dir.opendirSync(dpath)
            expect(dd !== null).assertTrue()
            let dirent = dd.readSync()
            expect(dirent !== null).assertTrue()
            dirent.isDirectory(-1)
            expect(null).assertFail()
        } catch (e) {
            expect(dd.closeSync()).assertNull()
            expect(fileio.unlinkSync(fpath)).assertNull()
            expect(fileio.rmdirSync(dpath)).assertNull()
        }
    })

    /**
     * @tc.number SUB_DF_FileIO_Dir_IsFIFO_0000
     * @tc.name fileio_test_dirent_is_fifo_000
     * @tc.desc Test Dir.isFIFO() interface.
     */
    it('fileio_test_dirent_is_fifo_000', 0, function () {
        let dpath = nextFileName('fileio_test_dirent_is_fifo_000') + 'd'
        let fpath = dpath + '/f1'

        try {
            expect(fileio.mkdirSync(dpath)).assertNull()
            expect(prepareFile(fpath, FILE_CONTENT)).assertTrue()
            let dd = fileio.Dir.opendirSync(dpath)
            expect(dd !== null).assertTrue()
            let dirent = dd.readSync()
            expect(dirent !== null).assertTrue()
            expect(dirent.isFIFO()).assertInstanceOf('Boolean')
            expect(dd.closeSync()).assertNull()
            expect(fileio.unlinkSync(fpath)).assertNull()
            expect(fileio.rmdirSync(dpath)).assertNull()
        } catch (e) {
            console.log("fileio_test_dirent_is_fifo_000 has failed for " + e)
            expect(null).assertFail()
        }
    })

    /**
     * @tc.number SUB_DF_FileIO_Dir_IsFIFO_0010
     * @tc.name fileio_test_dirent_is_fifo_001
     * @tc.desc Test Dir.isFIFO() interface.
     */
    it('fileio_test_dirent_is_fifo_001', 0, function () {
        let dpath = nextFileName('fileio_test_dirent_is_fifo_001') + 'd'
        let fpath = dpath + '/f1'
        let dd

        try {
            expect(fileio.mkdirSync(dpath)).assertNull()
            expect(prepareFile(fpath, FILE_CONTENT)).assertTrue()
            dd = fileio.Dir.opendirSync(dpath)
            expect(dd !== null).assertTrue()
            let dirent = dd.readSync()
            expect(dirent !== null).assertTrue()
            dirent.isFIFO(-1)
            expect(null).assertFail()
        } catch (e) {
            expect(dd.closeSync()).assertNull()
            expect(fileio.unlinkSync(fpath)).assertNull()
            expect(fileio.rmdirSync(dpath)).assertNull()
        }
    })

    /**
     * @tc.number SUB_DF_FileIO_Dir_IsFILE_0000
     * @tc.name fileio_test_dirent_is_file_000
     * @tc.desc Test Dir.isFILE() interface.
     */
    it('fileio_test_dirent_is_file_000', 0, function () {
        let dpath = nextFileName('fileio_test_dirent_is_file_000') + 'd'
        let fpath = dpath + '/f1'

        try {
            expect(fileio.mkdirSync(dpath)).assertNull()
            expect(prepareFile(fpath, FILE_CONTENT)).assertTrue()
            let dd = fileio.Dir.opendirSync(dpath)
            expect(dd !== null).assertTrue()
            let dirent = dd.readSync()
            expect(dirent !== null).assertTrue()
            expect(dirent.isFile()).assertInstanceOf('Boolean')
            expect(dd.closeSync()).assertNull()
            expect(fileio.unlinkSync(fpath)).assertNull()
            expect(fileio.rmdirSync(dpath)).assertNull()
        } catch (e) {
            console.log("fileio_test_dirent_is_file_000 has failed for " + e)
            expect(null).assertFail()
        }
    })

    /**
     * @tc.number SUB_DF_FileIO_Dir_IsFILE_0010
     * @tc.name fileio_test_dirent_is_file_001
     * @tc.desc Test Dir.isFILE() interface.
     */
    it('fileio_test_dirent_is_file_001', 0, function () {
        let dpath = nextFileName('fileio_test_dirent_is_file_001') + 'd'
        let fpath = dpath + '/f1'
        let dd

        try {
            expect(fileio.mkdirSync(dpath)).assertNull()
            expect(prepareFile(fpath, FILE_CONTENT)).assertTrue()
            dd = fileio.Dir.opendirSync(dpath)
            expect(dd !== null).assertTrue()
            let dirent = dd.readSync()
            expect(dirent !== null).assertTrue()
            dirent.isFile(-1)
            expect(null).assertFail()
        } catch (e) {
            expect(dd.closeSync()).assertNull()
            expect(fileio.unlinkSync(fpath)).assertNull()
            expect(fileio.rmdirSync(dpath)).assertNull()
        }
    })

    /**
     * @tc.number SUB_DF_FileIO_Dir_IsSocket_0000
     * @tc.name fileio_test_dirent_is_socket_000
     * @tc.desc Test Dir.isSocket() interface.
     */
    it('fileio_test_dirent_is_socket_000', 0, function () {
        let dpath = nextFileName('fileio_test_dirent_is_socket_000') + 'd'
        let fpath = dpath + '/f1'

        try {
            expect(fileio.mkdirSync(dpath)).assertNull()
            expect(prepareFile(fpath, FILE_CONTENT)).assertTrue()
            let dd = fileio.Dir.opendirSync(dpath)
            expect(dd !== null).assertTrue()
            let dirent = dd.readSync()
            expect(dirent !== null).assertTrue()
            expect(dirent.isSocket()).assertInstanceOf('Boolean')
            expect(dd.closeSync()).assertNull()
            expect(fileio.unlinkSync(fpath)).assertNull()
            expect(fileio.rmdirSync(dpath)).assertNull()
        } catch (e) {
            console.log("fileio_test_dirent_is_socket_000 has failed for " + e)
            expect(null).assertFail()
        }
    })

    /**
     * @tc.number SUB_DF_FileIO_Dir_IsSocket_0010
     * @tc.name fileio_test_dirent_is_socket_001
     * @tc.desc Test Dir.isSocket() interface.
     */
    it('fileio_test_dirent_is_socket_001', 0, function () {
        let dpath = nextFileName('fileio_test_dirent_is_socket_001') + 'd'
        let fpath = dpath + '/f1'
        let dd

        try {
            expect(fileio.mkdirSync(dpath)).assertNull()
            expect(prepareFile(fpath, FILE_CONTENT)).assertTrue()
            dd = fileio.Dir.opendirSync(dpath)
            expect(dd !== null).assertTrue()
            let dirent = dd.readSync()
            expect(dirent !== null).assertTrue()
            dirent.isSocket(-1)
            expect(null).assertFail()
        } catch (e) {
            expect(dd.closeSync()).assertNull()
            expect(fileio.unlinkSync(fpath)).assertNull()
            expect(fileio.rmdirSync(dpath)).assertNull()
        }
    })

    /**
     * @tc.number SUB_DF_FileIO_Dir_IsSymbolicLink_0000
     * @tc.name fileio_test_dirent_is_symbolic_link_000
     * @tc.desc Test Dir.isSymbolicLink() interface.
     */
    it('fileio_test_dirent_is_symbolic_link_000', 0, function () {
        let dpath = nextFileName('fileio_test_dirent_is_symbolic_link_000') + 'd'
        let fpath = dpath + '/f1'

        try {
            expect(fileio.mkdirSync(dpath)).assertNull()
            expect(prepareFile(fpath, FILE_CONTENT)).assertTrue()
            let dd = fileio.Dir.opendirSync(dpath)
            expect(dd !== null).assertTrue()
            let dirent = dd.readSync()
            expect(dirent !== null).assertTrue()
            expect(dirent.isSymbolicLink()).assertInstanceOf('Boolean')
            expect(dd.closeSync()).assertNull()
            expect(fileio.unlinkSync(fpath)).assertNull()
            expect(fileio.rmdirSync(dpath)).assertNull()
        } catch (e) {
            console.log("fileio_test_dirent_is_symbolic_link_000 has failed for " + e)
            expect(null).assertFail()
        }
    })

    /**
     * @tc.number SUB_DF_FileIO_Dir_IsSymbolicLink_0010
     * @tc.name fileio_test_dirent_is_symbolic_link_001
     * @tc.desc Test Dir.isSymbolicLink() interface.
     */
    it('fileio_test_dirent_is_symbolic_link_001', 0, function () {
        let dpath = nextFileName('fileio_test_dirent_is_symbolic_link_001') + 'd'
        let fpath = dpath + '/f1'
        let dd

        try {
            expect(fileio.mkdirSync(dpath)).assertNull()
            expect(prepareFile(fpath, FILE_CONTENT)).assertTrue()
            dd = fileio.Dir.opendirSync(dpath)
            expect(dd !== null).assertTrue()
            let dirent = dd.readSync()
            expect(dirent !== null).assertTrue()
            dirent.isSymbolicLink(-1)
            expect(null).assertFail()
        } catch (e) {
            expect(dd.closeSync()).assertNull()
            expect(fileio.unlinkSync(fpath)).assertNull()
            expect(fileio.rmdirSync(dpath)).assertNull()
        }
    })
})
