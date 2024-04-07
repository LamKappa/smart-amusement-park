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

describe('fileIOTestDir', function () {
    /**
     * @tc.number SUB_DF_FileIO_OpenClosedirSync_0000
     * @tc.name fileio_test_dir_open_close_sync_000
     * @tc.desc Test opendirSync() and Dir.closeSync() interfaces.
     */
    it('fileio_test_dir_open_close_sync_000', 0, function () {
        let dpath = nextFileName('fileio_test_dir_open_close_sync_000') + 'd'

        try {
            expect(fileio.mkdirSync(dpath)).assertNull()
            let dd = fileio.Dir.opendirSync(dpath)
            expect(dd !== null).assertTrue()
            expect(dd.closeSync()).assertNull()
            expect(fileio.rmdirSync(dpath)).assertNull()
        } catch (e) {
            console.log("fileio_test_dir_open_close_sync_000 has failed for " + e)
            expect(null).assertFail()
        }
    })

    /**
     * @tc.number SUB_DF_FileIO_OpenClosedirSync_0010
     * @tc.name fileio_test_dir_open_close_sync_001
     * @tc.desc Test opendirSync() interface.
     */
    it('fileio_test_dir_open_close_sync_001', 0, function () {
        try {
            fileio.Dir.opendirSync()
            expect(null).assertFail()
        } catch (e) {
        }
    })

    /**
     * @tc.number SUB_DF_FileIO_OpenClosedirSync_0020
     * @tc.name fileio_test_dir_open_close_sync_002
     * @tc.desc Test opendirSync() interface.
     */
    it('fileio_test_dir_open_close_sync_002', 0, function () {
        let dpath = nextFileName('fileio_test_dir_read_sync_000') + 'd'

        try {
            fileio.Dir.opendirSync(dpath)
            expect(null).assertFail()
        } catch (e) {
        }
    })

    /**
     * @tc.number SUB_DF_FileIO_Dir_ReadSync_0000
     * @tc.name fileio_test_dir_read_sync_000
     * @tc.desc Test Dir.readSync() interface.
     */
    it('fileio_test_dir_read_sync_000', 0, function () {
        let dpath = nextFileName('fileio_test_dir_read_sync_000') + 'd'
        let fpath = dpath + '/f1'

        try {
            expect(fileio.mkdirSync(dpath)).assertNull()
            expect(prepareFile(fpath, FILE_CONTENT)).assertTrue()
            let dd = fileio.Dir.opendirSync(dpath)
            expect(dd !== null).assertTrue()
            expect(dd.readSync() != null).assertTrue()
            expect(dd.closeSync()).assertNull()

            expect(fileio.unlinkSync(fpath)).assertNull()
            expect(fileio.rmdirSync(dpath)).assertNull()
        } catch (e) {
            console.log("fileio_test_dir_read_sync_000 has failed for " + e)
            expect(null).assertFail()
        }
    })

    /**
     * @tc.number SUB_DF_FileIO_Dir_ReadSync_0010
     * @tc.name fileio_test_dir_read_sync_001
     * @tc.desc Test Dir.readSync() interface.
     */
    it('fileio_test_dir_read_sync_001', 0, function () {
        let dpath = nextFileName('fileio_test_dir_read_sync_001') + 'd'
        let fpath = dpath + '/f1'
        let dd

        try {
            expect(fileio.mkdirSync(dpath)).assertNull()
            expect(prepareFile(fpath, FILE_CONTENT)).assertTrue()
            dd = fileio.Dir.opendirSync(dpath)
            expect(dd !== null).assertTrue()
            expect(dd.readSync(-1) != null).assertTrue()
            expect(null).assertFail()
        } catch (e) {
            expect(dd.closeSync()).assertNull()
            expect(fileio.unlinkSync(fpath)).assertNull()
            expect(fileio.rmdirSync(dpath)).assertNull()
        }
    })
})