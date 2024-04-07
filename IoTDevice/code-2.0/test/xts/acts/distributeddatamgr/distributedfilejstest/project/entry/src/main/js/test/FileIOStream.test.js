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

describe('fileIOTestStream', function () {
    /**
     * @tc.number SUB_DF_FileIO_Stream_CreateStreamSync_0000
     * @tc.name fileio_test_stream_create_stream_sync_000
     * @tc.desc Test Stream.createStreamSync() interface.
     */
    it('fileio_test_stream_create_stream_sync_000', 0, function () {
        let fpath = nextFileName('fileio_test_stream_create_stream_sync_000')
        expect(prepareFile(fpath, FILE_CONTENT)).assertTrue()

        try {
            var ss = fileio.Stream.createStreamSync(fpath, "r+")
            expect(ss !== null).assertTrue()
            expect(ss.closeSync()).assertNull()
            expect(fileio.unlinkSync(fpath)).assertNull()
        } catch (e) {
            console.log("fileio_test_stream_create_stream_sync_000 has failed for " + e)
            expect(null).assertFail()
        }
    })

    /**
     * @tc.number SUB_DF_FileIO_Stream_CreateStreamSync_0010
     * @tc.name fileio_test_stream_create_stream_sync_001
     * @tc.desc Test Stream.createStreamSync() interface.
     */
    it('fileio_test_stream_create_stream_sync_001', 0, function () {
        let fpath = nextFileName('fileio_test_stream_create_stream_sync_001')

        try {
            fileio.Stream.createStreamSync(fpath, "r+")
            expect(null).assertFail()
        } catch (e) {
        }
    })

    /**
     * @tc.number SUB_DF_FileIO_Stream_CreateStreamSync_0020
     * @tc.name fileio_test_stream_create_stream_sync_002
     * @tc.desc Test Stream.createStreamSync() interface.
     */
    it('fileio_test_stream_create_stream_sync_002', 0, function () {
        let fpath = nextFileName('fileio_test_stream_create_stream_sync_002')
        expect(prepareFile(fpath, FILE_CONTENT)).assertTrue()

        try {
            fileio.Stream.createStreamSync(fpath, "ohos")
            expect(null).assertFail()
        } catch (e) {
            expect(fileio.unlinkSync(fpath)).assertNull()
        }
    })

    /**
     * @tc.number SUB_DF_FileIO_Stream_FdopenStreamSync_0000
     * @tc.name fileio_test_stream_fdopen_stream_sync_000
     * @tc.desc Test Stream.fdopenStreamSync() interface.
     */
    it('fileio_test_stream_fdopen_stream_sync_000', 0, function () {
        let fpath = nextFileName('fileio_test_stream_fdopen_stream_sync_000')
        expect(prepareFile(fpath, FILE_CONTENT)).assertTrue()

        try {
            var fd = fileio.openSync(fpath, 0o2)
            let ss = fileio.Stream.fdopenStreamSync(fd, "r+")
            expect(ss !== null).assertTrue()
            expect(ss.closeSync()).assertNull()
            expect(fileio.unlinkSync(fpath)).assertNull()
        } catch (e) {
            console.log("fileio_test_stream_fdopen_stream_sync_000 has failed for " + e)
            expect(null).assertFail()
        }
    })

    /**
     * @tc.number SUB_DF_FileIO_Stream_FdopenStreamSync_0010
     * @tc.name fileio_test_stream_fdopen_stream_sync_001
     * @tc.desc Test Stream.fdopenStreamSync() interface.
     */
    it('fileio_test_stream_fdopen_stream_sync_001', 0, function () {
        try {
            let ss = fileio.Stream.fdopenStreamSync(-1, "r+")
            expect(null).assertFail()
        } catch (e) {
        }
    })

    /**
     * @tc.number SUB_DF_FileIO_Stream_ReadSync_0000
     * @tc.name fileio_test_stream_read_sync_000
     * @tc.desc Test Stream.readSync() interface.
     */
    it('fileio_test_stream_read_sync_000', 0, function () {
        let fpath = nextFileName('fileio_test_stream_read_sync_000')
        expect(prepareFile(fpath, FILE_CONTENT)).assertTrue()

        try {
            let ss = fileio.Stream.createStreamSync(fpath, "r+")
            expect(ss !== null).assertTrue()
            let len = ss.readSync(new ArrayBuffer(4096))
            expect(len).assertEqual(FILE_CONTENT.length)
            expect(ss.closeSync()).assertNull()
            expect(fileio.unlinkSync(fpath)).assertNull()
        } catch (e) {
            console.log("fileio_test_stream_read_sync_000 has failed for " + e)
            expect(null).assertFail()
        }
    })

    /**
     * @tc.number SUB_DF_FileIO_Stream_WriteSync_0000
     * @tc.name fileio_test_stream_write_sync_000
     * @tc.desc Test Stream.writeSync() interface.
     */
    it('fileio_test_stream_write_sync_000', 0, function () {
        let fpath = nextFileName('fileio_test_stream_write_sync_000')
        expect(prepareFile(fpath, FILE_CONTENT)).assertTrue()

        try {
            let ss = fileio.Stream.createStreamSync(fpath, "r+")
            expect(ss !== null).assertTrue()
            expect(ss.writeSync(FILE_CONTENT)).assertEqual(FILE_CONTENT.length)
            expect(ss.closeSync()).assertNull()
            expect(fileio.unlinkSync(fpath)).assertNull()
        } catch (e) {
            console.log("fileio_test_stream_write_sync_000 has failed for " + e)
            expect(null).assertFail()
        }
    })

    /**
     * @tc.number SUB_DF_FileIO_Stream_FlushSync_0000
     * @tc.name fileio_test_stream_flush_sync_000
     * @tc.desc Test Stream.flushSync() interface.
     */
    it('fileio_test_stream_flush_sync_000', 0, function () {
        let fpath = nextFileName('fileio_test_stream_flush_sync_000')
        expect(prepareFile(fpath, FILE_CONTENT)).assertTrue()

        try {
            let ss = fileio.Stream.createStreamSync(fpath, "r+")
            expect(ss !== null).assertTrue()
            expect(ss.writeSync(FILE_CONTENT)).assertEqual(FILE_CONTENT.length)
            expect(ss.flushSync()).assertNull()
            expect(ss.closeSync()).assertNull()
            expect(fileio.unlinkSync(fpath)).assertNull()
        } catch (e) {
            console.log("fileio_test_stream_flush_sync_000 has failed for " + e)
            expect(null).assertFail()
        }
    })
})