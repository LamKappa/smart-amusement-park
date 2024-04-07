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
import bundle_mgr from '@ohos.bundle_mgr'

import { describe, beforeAll, beforeEach, afterEach, afterAll, it, expect } from 'deccjsunit/index'
import { FILE_CONTENT, prepareFile, nextFileName } from './Common'

describe('fileIOTest', function () {
    /**
     * @tc.number SUB_DF_FileIO_UnlinkSync_0000
     * @tc.name fileio_test_unlink_sync_000
     * @tc.desc Test unlinkSync() interface.
     */
    it('fileio_test_unlink_sync_000', 0, function () {
        let fpath = nextFileName('fileIOTest')
        expect(prepareFile(fpath, FILE_CONTENT)).assertTrue()

        try {
            expect(fileio.unlinkSync(fpath)).assertNull()
        } catch (e) {
            console.log("fileio_test_unlink_sync_000 has failed for " + e)
            expect(null).assertFail()
        }
    })

    /**
     * @tc.number SUB_DF_FileIO_UnlinkSync_0010
     * @tc.name fileio_test_unlink_sync_001
     * @tc.desc Test unlinkSync() interface.
     */
    it('fileio_test_unlink_sync_001', 0, function () {
        try {
            fileio.unlinkSync()
            expect(null).assertFail()
        } catch (e) {
        }
    })

    /**
     * @tc.number SUB_DF_FileIO_UnlinkSync_0020
     * @tc.name fileio_test_unlink_sync_002
     * @tc.desc Test unlinkSync() interface.
     */
    it('fileio_test_unlink_sync_002', 0, function () {
        let fpath = nextFileName('fileIOTest')

        try {
            fileio.unlinkSync(fpath)
            expect(null).assertFail()
        } catch (e) {
        }
    })

    /**
     * @tc.number SUB_DF_FileIO_AccessSync_0000
     * @tc.name fileio_test_access_sync_000
     * @tc.desc Test accessSync() interface.
     */
    it('fileio_test_access_sync_000', 0, function () {
        let fpath = nextFileName('fileio_test_access_sync_000')
        expect(prepareFile(fpath, FILE_CONTENT)).assertTrue()

        try {
            expect(fileio.accessSync(fpath)).assertNull()
            expect(fileio.unlinkSync(fpath)).assertNull()
        } catch (e) {
            console.log("fileio_test_access_sync_000 has failed for " + e)
            expect(null).assertFail()
        }
    })

    /**
     * @tc.number SUB_DF_FileIO_AccessSync_0010
     * @tc.name fileio_test_access_sync_001
     * @tc.desc Test accessSync() interface.
     */
    it('fileio_test_access_sync_001', 0, function () {
        try {
            fileio.accessSync()
            expect(null).assertFail()
        } catch (e) {
        }
    })

    /**
     * @tc.number SUB_DF_FileIO_AccessSync_0020
     * @tc.name fileio_test_access_sync_002
     * @tc.desc Test accessSync() interface.
     */
    it('fileio_test_access_sync_002', 0, function () {
        let fpath = nextFileName('fileIOTest')

        try {
            fileio.accessSync(fpath)
            expect(null).assertFail()
        } catch (e) {
        }
    })

    /**
     * @tc.number SUB_DF_FileIO_OpenCloseSync_0000
     * @tc.name fileio_test_open_close_sync_000
     * @tc.desc Test openSync() and closeSync() interfaces.
     */
    it('fileio_test_open_close_sync_000', 0, function () {
        let fpath = nextFileName('fileio_test_open_close_sync_000')

        try {
            let fd = fileio.openSync(fpath, 0o102, 0o666)
            expect(fd).assertInstanceOf('Number')
            expect(fileio.closeSync(fd)).assertNull()
            expect(fileio.unlinkSync(fpath)).assertNull()
        } catch (e) {
            console.log("fileio_test_open_close_sync_000 has failed for " + e)
            expect(null).assertFail()
        }
    })

    /**
     * @tc.number SUB_DF_FileIO_OpenSync_0000
     * @tc.name fileio_test_open_sync_000
     * @tc.desc Test openSync() interface.
     */
    it('fileio_test_open_sync_000', 0, function () {
        try {
            fileio.openSync("/", 0o102, 0o666)
            expect(null).assertFail()
        } catch (e) {
        }
    })

    /**
     * @tc.number SUB_DF_FileIO_OpenSync_0010
     * @tc.name fileio_test_open_sync_001
     * @tc.desc Test openSync() interface.
     */
    it('fileio_test_open_sync_001', 0, function () {
        let fpath = nextFileName('fileio_test_open_sync_001')

        try {
            fileio.openSync(fpath, 0o102)
            expect(null).assertFail()
        } catch (e) {
        }
    })

    /**
     * @tc.number SUB_DF_FileIO_CloseSync_0000
     * @tc.name fileio_test_close_sync_000
     * @tc.desc Test closeSync() interface.
     */
    it('fileio_test_close_sync_000', 0, function () {
        try {
            fileio.closeSync()
            expect(null).assertFail()
        } catch (e) {
        }
    })

    /**
     * @tc.number SUB_DF_FileIO_CloseSync_0010
     * @tc.name fileio_test_close_sync_001
     * @tc.desc Test closeSync() interface.
     */
    it('fileio_test_close_sync_001', 0, function () {
        try {
            fileio.closeSync(-1)
            expect(null).assertFail()
        } catch (e) {
        }
    })

    /**
     * @tc.number SUB_DF_FileIO_WriteSync_0000
     * @tc.name fileio_test_write_sync_000
     * @tc.desc Test writeSync() interface.
     */
    it('fileio_test_write_sync_000', 0, function () {
        let fpath = nextFileName('fileio_test_write_sync_000')

        try {
            let fd = fileio.openSync(fpath, 0o102, 0o666)
            expect(fd).assertInstanceOf('Number')
            expect(fileio.writeSync(fd, FILE_CONTENT)).assertEqual(FILE_CONTENT.length)
            expect(fileio.closeSync(fd)).assertNull()
            expect(fileio.unlinkSync(fpath)).assertNull()
        } catch (e) {
            console.log("fileio_test_write_sync_000 has failed for " + e)
            expect(null).assertFail()
        }
    })

    /**
     * @tc.number SUB_DF_FileIO_WriteSync_0010
     * @tc.name fileio_test_write_sync_001
     * @tc.desc Test writeSync() interface.
     */
    it('fileio_test_write_sync_001', 0, function () {
        let fpath = nextFileName('fileio_test_write_sync_001')

        try {
            let fd = fileio.openSync(fpath, 0o102, 0o666)
            expect(fd).assertInstanceOf('Number')
            expect(fileio.writeSync(fd, FILE_CONTENT, {
                encoding: "utf-8",
            })).assertEqual(FILE_CONTENT.length)
            expect(fileio.closeSync(fd)).assertNull()
            expect(fileio.unlinkSync(fpath)).assertNull()
        } catch (e) {
            console.log("fileio_test_write_sync_001 has failed for " + e)
            expect(null).assertFail()
        }
    })

    /**
     * @tc.number SUB_DF_FileIO_WriteSync_0020
     * @tc.name fileio_test_write_sync_002
     * @tc.desc Test writeSync() interface.
     */
    it('fileio_test_write_sync_002', 0, function () {
        let fpath = nextFileName('fileio_test_write_sync_002')

        try {
            let fd = fileio.openSync(fpath, 0o102, 0o666)
            expect(fd).assertInstanceOf('Number')
            expect(fileio.writeSync(fd, FILE_CONTENT, {
                offset: 1,
            })).assertEqual(FILE_CONTENT.length - 1)
            expect(fileio.closeSync(fd)).assertNull()
            expect(fileio.unlinkSync(fpath)).assertNull()
        } catch (e) {
            console.log("fileio_test_write_sync_002 has failed for " + e)
            expect(null).assertFail()
        }
    })

    /**
     * @tc.number SUB_DF_FileIO_WriteSync_0030
     * @tc.name fileio_test_write_sync_003
     * @tc.desc Test writeSync() interface.
     */
    it('fileio_test_write_sync_003', 0, function () {
        let fpath = nextFileName('fileio_test_write_sync_003')

        try {
            let fd = fileio.openSync(fpath, 0o102, 0o666)
            expect(fd).assertInstanceOf('Number')
            expect(fileio.writeSync(fd, FILE_CONTENT, {
                length: FILE_CONTENT.length - 1,
            })).assertEqual(FILE_CONTENT.length - 1)
            expect(fileio.closeSync(fd)).assertNull()
            expect(fileio.unlinkSync(fpath)).assertNull()
        } catch (e) {
            console.log("fileio_test_write_sync_003 has failed for " + e)
            expect(null).assertFail()
        }
    })

    /**
     * @tc.number SUB_DF_FileIO_WriteSync_0040
     * @tc.name fileio_test_write_sync_004
     * @tc.desc Test writeSync() interface.
     */
    it('fileio_test_write_sync_004', 0, function () {
        let fpath = nextFileName('fileio_test_write_sync_004')

        try {
            let fd = fileio.openSync(fpath, 0o102, 0o666)
            expect(fd).assertInstanceOf('Number')
            expect(fileio.writeSync(fd, FILE_CONTENT, {
                offset: 1,
                length: 1,
            })).assertEqual(1)
            expect(fileio.closeSync(fd)).assertNull()
            expect(fileio.unlinkSync(fpath)).assertNull()
        } catch (e) {
            console.log("fileio_test_write_sync_004 has failed for " + e)
            expect(null).assertFail()
        }
    })

    /**
     * @tc.number SUB_DF_FileIO_WriteSync_0050
     * @tc.name fileio_test_write_sync_005
     * @tc.desc Test writeSync() interface.
     */
    it('fileio_test_write_sync_005', 0, function () {
        let fpath = nextFileName('fileio_test_write_sync_005')
        const invalidOffset = 999
        let fd

        try {
            fd = fileio.openSync(fpath, 0o102, 0o666)
            expect(fd).assertInstanceOf('Number')
            expect(fileio.writeSync(fd, FILE_CONTENT, {
                offset: invalidOffset,
            })).assertEqual(1)
            expect(null).assertFail()
        } catch (e) {
            expect(fileio.closeSync(fd)).assertNull()
            expect(fileio.unlinkSync(fpath)).assertNull()
        }
    })

    /**
     * @tc.number SUB_DF_FileIO_WriteSync_0060
     * @tc.name fileio_test_write_sync_006
     * @tc.desc Test writeSync() interface.
     */
    it('fileio_test_write_sync_006', 0, function () {
        let fpath = nextFileName('fileio_test_write_sync_006')
        const invalidLength = 999
        let fd

        try {
            fd = fileio.openSync(fpath, 0o102, 0o666)
            expect(fd).assertInstanceOf('Number')
            expect(fileio.writeSync(fd, FILE_CONTENT, {
                length: invalidLength,
            })).assertEqual(1)
            expect(null).assertFail()
        } catch (e) {
            expect(fileio.closeSync(fd)).assertNull()
            expect(fileio.unlinkSync(fpath)).assertNull()
        }
    })

    /**
     * @tc.number SUB_DF_FileIO_WriteSync_0070
     * @tc.name fileio_test_write_sync_007
     * @tc.desc Test writeSync() interface.
     */
    it('fileio_test_write_sync_007', 0, function () {
        try {
            fileio.writeSync()
            expect(null).assertFail()
        } catch (e) {
        }
    })

    /**
     * @tc.number SUB_DF_FileIO_WriteSync_0080
     * @tc.name fileio_test_write_sync_008
     * @tc.desc Test writeSync() interface.
     */
    it('fileio_test_write_sync_008', 0, function () {
        try {
            fileio.writeSync(-1, FILE_CONTENT)
            expect(null).assertFail()
        } catch (e) {
        }
    })

    /**
     * @tc.number SUB_DF_FileIO_ReadSync_0000
     * @tc.name fileio_test_read_sync_000
     * @tc.desc Test readSync() interface.
     */
    it('fileio_test_read_sync_000', 0, function () {
        let fpath = nextFileName('fileio_test_read_sync_000')
        expect(prepareFile(fpath, FILE_CONTENT)).assertTrue()

        try {
            let fd = fileio.openSync(fpath, 0o2)
            expect(fd).assertInstanceOf('Number')
            let len = fileio.readSync(fd, new ArrayBuffer(4096))
            expect(len).assertEqual(FILE_CONTENT.length)
            expect(fileio.closeSync(fd)).assertNull()
            expect(fileio.unlinkSync(fpath)).assertNull()
        } catch (e) {
            console.log("fileio_test_read_sync_000 has failed for " + e)
            expect(null).assertFail()
        }
    })

    /**
     * @tc.number SUB_DF_FileIO_ReadSync_0001
     * @tc.name fileio_test_read_sync_001
     * @tc.desc Test readSync() interface.
     */
    it('fileio_test_read_sync_001', 0, function () {
        let bufLen = 5
        expect(FILE_CONTENT.length).assertLarger(bufLen)
        let fpath = nextFileName('fileio_test_read_sync_001')
        expect(prepareFile(fpath, FILE_CONTENT)).assertTrue()

        try {
            let fd = fileio.openSync(fpath, 0o2)
            expect(fd).assertInstanceOf('Number')
            let len = fileio.readSync(fd, new ArrayBuffer(bufLen), {
                offset: 1,
            })
            expect(len).assertEqual(bufLen - 1)
            expect(fileio.closeSync(fd)).assertNull()
            expect(fileio.unlinkSync(fpath)).assertNull()
        } catch (e) {
            console.log("fileio_test_read_sync_001 has failed for " + e)
            expect(null).assertFail()
        }
    })

    /**
     * @tc.number SUB_DF_FileIO_ReadSync_0020
     * @tc.name fileio_test_read_sync_002
     * @tc.desc Test readSync() interface.
     */
    it('fileio_test_read_sync_002', 0, function () {
        let fpath = nextFileName('fileio_test_read_sync_002')
        expect(prepareFile(fpath, FILE_CONTENT)).assertTrue()

        try {
            let fd = fileio.openSync(fpath, 0o2)
            expect(fd).assertInstanceOf('Number')
            let len = fileio.readSync(fd, new ArrayBuffer(4096), {
                length: 1,
            })
            expect(len).assertEqual(1)
            expect(fileio.closeSync(fd)).assertNull()
            expect(fileio.unlinkSync(fpath)).assertNull()
        } catch (e) {
            console.log("fileio_test_read_sync_002 has failed for " + e)
            expect(null).assertFail()
        }
    })

    /**
     * @tc.number SUB_DF_FileIO_ReadSync_0030
     * @tc.name fileio_test_read_sync_003
     * @tc.desc Test readSync() interface.
     */
    it('fileio_test_read_sync_003', 0, function () {
        let fd
        const invalidOffset = 99999
        let fpath = nextFileName('fileio_test_read_sync_003')
        expect(prepareFile(fpath, FILE_CONTENT)).assertTrue()

        try {
            fd = fileio.openSync(fpath, 0o2)
            expect(fd).assertInstanceOf('Number')
            fileio.readSync(fd, new ArrayBuffer(4096), {
                offset: invalidOffset,
            })
            expect(null).assertFail()
        } catch (e) {
            expect(fileio.closeSync(fd)).assertNull()
            expect(fileio.unlinkSync(fpath)).assertNull()
        }
    })

    /**
     * @tc.number SUB_DF_FileIO_ReadSync_0040
     * @tc.name fileio_test_read_sync_004
     * @tc.desc Test readSync() interface.
     */
    it('fileio_test_read_sync_004', 0, function () {
        let fd
        const invalidLength = 9999
        let fpath = nextFileName('fileio_test_read_sync_004')
        expect(prepareFile(fpath, FILE_CONTENT)).assertTrue()

        try {
            fd = fileio.openSync(fpath, 0o2)
            expect(fd).assertInstanceOf('Number')
            fileio.readSync(fd, new ArrayBuffer(4096), {
                length: invalidLength,
            })
            expect(null).assertFail()
        } catch (e) {
            expect(fileio.closeSync(fd)).assertNull()
            expect(fileio.unlinkSync(fpath)).assertNull()
        }
    })

    /**
     * @tc.number SUB_DF_FileIO_ReadSync_0050
     * @tc.name fileio_test_read_sync_005
     * @tc.desc Test readSync() interface.
     */
    it('fileio_test_read_sync_005', 0, function () {
        let fpath = nextFileName('fileio_test_read_sync_005')
        let fd

        try {
            fileio.readSync(-1, new ArrayBuffer(4096))
            expect(null).assertFail()
        } catch (e) {
        }
    })

    /**
     * @tc.number SUB_DF_FileIO_ReadSync_0060
     * @tc.name fileio_test_read_sync_006
     * @tc.desc Test readSync() interface.
     */
    it('fileio_test_read_sync_006', 0, function () {
        let fpath = nextFileName('fileio_test_read_sync_006')
        expect(prepareFile(fpath, FILE_CONTENT)).assertTrue()

        try {
            let fd = fileio.openSync(fpath, 0o2)
            expect(fd).assertInstanceOf('Number')
            let len = fileio.readSync(fd, new ArrayBuffer(4096), {
                position: 1,
            })
            expect(len).assertEqual(FILE_CONTENT.length - 1)
            expect(fileio.closeSync(fd)).assertNull()
            expect(fileio.unlinkSync(fpath)).assertNull()
        } catch (e) {
            console.log("fileio_test_read_sync_006 has failed for " + e)
            expect(null).assertFail()
        }
    })

    /**
     * @tc.number SUB_DF_FileIO_ReadSync_0070
     * @tc.name fileio_test_read_sync_007
     * @tc.desc Test readSync() interface.
     */
    it('fileio_test_read_sync_007', 0, function () {
        let fpath = nextFileName('fileio_test_read_sync_007')
        expect(prepareFile(fpath, FILE_CONTENT)).assertTrue()

        try {
            let fd = fileio.openSync(fpath, 0o2)
            expect(fd).assertInstanceOf('Number')
            let invalidPos = FILE_CONTENT.length + 1
            let len = fileio.readSync(fd, new ArrayBuffer(4096), {
                position: invalidPos,
            })
            expect(len).assertEqual(0)
            expect(fileio.closeSync(fd)).assertNull()
            expect(fileio.unlinkSync(fpath)).assertNull()
        } catch (e) {
            console.log("fileio_test_read_sync_007 has failed for " + e)
            expect(null).assertFail()
        }
    })

    /**
     * @tc.number SUB_DF_FileIO_ChmodSync_0000
     * @tc.name fileio_test_chmod_sync_000
     * @tc.desc Test chmodSync() interface.
     */
    it('fileio_test_chmod_sync_000', 0, function () {
        let fpath = nextFileName('fileio_test_chmod_sync_000')
        expect(prepareFile(fpath, FILE_CONTENT)).assertTrue()

        try {
            expect(fileio.chmodSync(fpath, 0o660)).assertNull()
            expect(fileio.Stat.statSync(fpath).mode & 0o777).assertEqual(0o660)
            expect(fileio.unlinkSync(fpath)).assertNull()
        } catch (e) {
            console.log("fileio_test_chmod_sync_000 has failed for " + e)
            expect(null).assertFail()
        }
    })

    /**
     * @tc.number SUB_DF_FileIO_ChmodSync_0010
     * @tc.name fileio_test_chmod_sync_001
     * @tc.desc Test chmodSync() interface.
     */
    it('fileio_test_chmod_sync_001', 0, function () {
        let fpath = nextFileName('fileio_test_chmod_sync_001')
        expect(prepareFile(fpath, FILE_CONTENT)).assertTrue()

        try {
            fileio.chmodSync(fpath)
            expect(null).assertFail()
        } catch (e) {
            expect(fileio.unlinkSync(fpath)).assertNull()
        }
    })

    /**
     * @tc.number SUB_DF_FileIO_ChmodSync_0020
     * @tc.name fileio_test_chmod_sync_002
     * @tc.desc Test chmodSync() interface.
     */
    it('fileio_test_chmod_sync_002', 0, function () {
        let fpath = nextFileName('fileio_test_chmod_sync_002')

        try {
            fileio.chmodSync(fpath, 0o660)
            expect(null).assertFail()
        } catch (e) {
        }
    })

    /**
     * @tc.number SUB_DF_FileIO_CopyFileSync_0000
     * @tc.name fileio_test_copy_file_sync_000
     * @tc.desc Test copyFileSync() interface.
     */
    it('fileio_test_copy_file_sync_000', 0, function () {
        let fpath = nextFileName('fileio_test_copy_file_sync_000')
        let fpathTarget = fpath + 'tgt'
        expect(prepareFile(fpath, FILE_CONTENT)).assertTrue()

        try {
            expect(fileio.copyFileSync(fpath, fpathTarget)).assertNull()
            expect(fileio.unlinkSync(fpath)).assertNull()
            expect(fileio.unlinkSync(fpathTarget)).assertNull()
        } catch (e) {
            console.log("fileio_test_copy_file_sync_000 has failed for " + e)
            expect(null).assertFail()
        }
    })

    /**
     * @tc.number SUB_DF_FileIO_CopyFileSync_0010
     * @tc.name fileio_test_copy_file_sync_001
     * @tc.desc Test copyFileSync() interface.
     */
    it('fileio_test_copy_file_sync_001', 0, function () {
        let fpath = nextFileName('fileio_test_copy_file_sync_000')
        let fpathTarget = fpath + 'tgt'

        try {
            fileio.copyFileSync(fpath, fpathTarget)
            expect(null).assertFail()
        } catch (e) {
        }
    })

    /**
     * @tc.number SUB_DF_FileIO_CopyFileSync_0020
     * @tc.name fileio_test_copy_file_sync_002
     * @tc.desc Test copyFileSync() interface.
     */
    it('fileio_test_copy_file_sync_002', 0, function () {
        try {
            fileio.copyFileSync()
            expect(null).assertFail()
        } catch (e) {
        }
    })

    /**
     * @tc.number SUB_DF_FileIO_TruncateSync_0000
     * @tc.name fileio_test_truncate_sync_000
     * @tc.desc Test truncateSync() interface.
     */
    it('fileio_test_truncate_sync_000', 0, function () {
        let fpath = nextFileName('fileio_test_truncate_sync_000')
        expect(prepareFile(fpath, FILE_CONTENT)).assertTrue()

        try {
            expect(fileio.truncateSync(fpath)).assertNull()
            expect(fileio.Stat.statSync(fpath).size).assertEqual(0)
            expect(fileio.unlinkSync(fpath)).assertNull()
        } catch (e) {
            console.log("fileio_test_truncate_sync_000 has failed for " + e)
            expect(null).assertFail()
        }
    })

    /**
     * @tc.number SUB_DF_FileIO_TruncateSync_0010
     * @tc.name fileio_test_truncate_sync_001
     * @tc.desc Test truncateSync() interface.
     */
    it('fileio_test_truncate_sync_001', 0, function () {
        let fpath = nextFileName('fileio_test_truncate_sync_001')

        try {
            fileio.truncateSync(fpath)
            expect(null).assertFail()
        } catch (e) {
        }
    })

    /**
     * @tc.number SUB_DF_FileIO_TruncateSync_0020
     * @tc.name fileio_test_truncate_sync_002
     * @tc.desc Test truncateSync() interface.
     */
    it('fileio_test_truncate_sync_002', 0, function () {
        try {
            fileio.truncateSync()
            expect(null).assertFail()
        } catch (e) {
        }
    })

    /**
     * @tc.number SUB_DF_FileIO_RenameSync_0000
     * @tc.name fileio_test_rename_sync_000
     * @tc.desc Test renameSync() interface.
     */
    it('fileio_test_rename_sync_000', 0, function () {
        let fpath = nextFileName('fileio_test_rename_sync_000')
        let fpathTarget = fpath + 'tgt'
        expect(prepareFile(fpath, FILE_CONTENT)).assertTrue()

        try {
            expect(fileio.renameSync(fpath, fpathTarget)).assertNull()
            expect(fileio.accessSync(fpathTarget)).assertNull()
            expect(fileio.unlinkSync(fpathTarget)).assertNull()
        } catch (e) {
            console.log("fileio_test_rename_sync_000 has failed for " + e)
            expect(null).assertFail()
        }
    })

    /**
     * @tc.number SUB_DF_FileIO_RenameSync_0010
     * @tc.name fileio_test_rename_sync_001
     * @tc.desc Test renameSync() interface.
     */
    it('fileio_test_rename_sync_001', 0, function () {
        let fpath = nextFileName('fileio_test_rename_sync_001')
        let fpathTarget = fpath + 'tgt'

        try {
            fileio.renameSync(fpath, fpathTarget)
            expect(null).assertFail()
        } catch (e) {
        }
    })

    /**
     * @tc.number SUB_DF_FileIO_RenameSync_0020
     * @tc.name fileio_test_rename_sync_002
     * @tc.desc Test renameSync() interface.
     */
    it('fileio_test_rename_sync_002', 0, function () {
        let fpath = nextFileName('fileio_test_rename_sync_002')
        expect(prepareFile(fpath, FILE_CONTENT)).assertTrue()

        try {
            fileio.renameSync(fpath, "/")
        } catch (e) {
            expect(fileio.unlinkSync(fpath)).assertNull()
        }
    })

    /**
     * @tc.number SUB_DF_FileIO_FchmodSync_0000
     * @tc.name fileio_test_fchmod_sync_000
     * @tc.desc Test fchmodSync() interface.
     */
    it('fileio_test_fchmod_sync_000', 0, function () {
        let fpath = nextFileName('fileio_test_fchmod_sync_000')
        expect(prepareFile(fpath, FILE_CONTENT)).assertTrue()

        try {
            let fd = fileio.openSync(fpath, 0o2)
            expect(fileio.fchmodSync(fd, 0o660)).assertNull()
            expect(fileio.Stat.statSync(fpath).mode & 0o777).assertEqual(0o660)
            expect(fileio.unlinkSync(fpath)).assertNull()
        } catch (e) {
            console.log("fileio_test_fchmod_sync_000 has failed for " + e)
            expect(null).assertFail()
        }
    })

    /**
     * @tc.number SUB_DF_FileIO_FchmodSync_0010
     * @tc.name fileio_test_fchmod_sync_001
     * @tc.desc Test fchmodSync() interface.
     */
    it('fileio_test_fchmod_sync_001', 0, function () {
        try {
            expect(fileio.fchmodSync(-1, 0o660)).assertNull()
            expect(null).assertFail()
        } catch (e) {
        }
    })

    /**
     * @tc.number SUB_DF_FileIO_FtruncateSync_0000
     * @tc.name fileio_test_ftruncate_sync_000
     * @tc.desc Test ftruncateSync() interface.
     */
    it('fileio_test_ftruncate_sync_000', 0, function () {
        let fpath = nextFileName('fileio_test_ftruncate_sync_000')
        expect(prepareFile(fpath, FILE_CONTENT)).assertTrue()

        try {
            let fd = fileio.openSync(fpath, 0o2)
            expect(fileio.ftruncateSync(fd)).assertNull()
            expect(fileio.Stat.statSync(fpath).size).assertEqual(0)
            expect(fileio.unlinkSync(fpath)).assertNull()
        } catch (e) {
            console.log("fileio_test_ftruncate_sync_000 has failed for " + e)
            expect(null).assertFail()
        }
    })

    /**
     * @tc.number SUB_DF_FileIO_FtruncateSync_0010
     * @tc.name fileio_test_ftruncate_sync_001
     * @tc.desc Test ftruncateSync() interface.
     */
    it('fileio_test_ftruncate_sync_001', 0, function () {
        try {
            fileio.ftruncateSync(-1)
            expect(null).assertFail()
        } catch (e) {
        }
    })

    /**
     * @tc.number SUB_DF_FileIO_FsyncSync_0000
     * @tc.name fileio_test_fsync_sync_000
     * @tc.desc Test fsyncSync() interface.
     */
    it('fileio_test_fsync_sync_000', 0, function () {
        let fpath = nextFileName('fileio_test_fsync_sync_000')
        expect(prepareFile(fpath, FILE_CONTENT)).assertTrue()

        try {
            let fd = fileio.openSync(fpath, 0o2)
            expect(fileio.fsyncSync(fd)).assertNull()
            expect(fileio.unlinkSync(fpath)).assertNull()
        } catch (e) {
            console.log("fileio_test_fsync_sync_000 has failed for " + e)
            expect(null).assertFail()
        }
    })

    /**
     * @tc.number SUB_DF_FileIO_FsyncSync_0010
     * @tc.name fileio_test_fsync_sync_001
     * @tc.desc Test fsyncSync() interface.
     */
    it('fileio_test_fsync_sync_001', 0, function () {
        try {
            fileio.fsyncSync(-1)
            expect(null).assertFail()
        } catch (e) {
        }
    })

    /**
     * @tc.number SUB_DF_FileIO_MkdirSync_0000
     * @tc.name fileio_test_mkdir_sync_rmdir_sync_000
     * @tc.desc Test mkdirSync() interface.
     */
    it('fileio_test_mkdir_sync_rmdir_sync_000', 0, function () {
        let dpath = nextFileName('fileio_test_fsync_sync_000') + 'd'

        try {
            expect(fileio.mkdirSync(dpath)).assertNull()
            expect(fileio.rmdirSync(dpath)).assertNull()
        } catch (e) {
            console.log("fileio_test_mkdir_sync_rmdir_sync_000 has failed for " + e)
            expect(null).assertFail()
        }
    })

    /**
     * @tc.number SUB_DF_FileIO_MkdirSync_0010
     * @tc.name fileio_test_mkdir_sync_rmdir_sync_001
     * @tc.desc Test mkdirSync() interface.
     */
    it('fileio_test_mkdir_sync_rmdir_sync_001', 0, function () {
        try {
            expect(fileio.mkdirSync("/")).assertNull()
            expect(null).assertFail()
        } catch (e) {
        }
    })

    /**
     * @tc.number SUB_DF_FileIO_ChownSync_0000
     * @tc.name fileio_test_chown_sync_000
     * @tc.desc Test chownSync() interface.
     */
    it('fileio_test_chown_sync_000', 0, async function (done) {
        let fpath = nextFileName('fileio_test_chown_sync_000')
        expect(prepareFile(fpath, FILE_CONTENT)).assertTrue()
        let bundleInfo = await bundle_mgr.getBundleInfo("ohos.acts.distributeddatamgr.distributedfile")
        let UID = bundleInfo.uid
        let GID = bundleInfo.gid

        try {
            expect(fileio.chownSync(fpath, UID, GID))
            expect(fileio.unlinkSync(fpath)).assertNull()
        } catch (e) {
            console.log("fileio_test_chown_sync_000 has failed for " + e)
            expect(null).assertFail()
        }
        done()
    })

    /**
     * @tc.number SUB_DF_FileIO_ChownSync_0010
     * @tc.name fileio_test_chown_sync_001
     * @tc.desc Test chownSync() interface.
     */
    it('fileio_test_chown_sync_001', 0, async function (done) {
        let bundleInfo = await bundle_mgr.getBundleInfo("ohos.acts.distributeddatamgr.distributedfile")
        let UID = bundleInfo.uid
        let GID = bundleInfo.gid
        let fpath = nextFileName('fileio_test_chown_sync_001')

        try {
            expect(fileio.chownSync(fpath, UID, GID))
            expect(null).assertFail()
        } catch (e) {
        }
        done()
    })

    /**
     * @tc.number SUB_DF_FileIO_ChownSync_0020
     * @tc.name fileio_test_chown_sync_002
     * @tc.desc Test chownSync() interface.
     */
    it('fileio_test_chown_sync_002', 0, function () {
        let fpath = nextFileName('fileio_test_chown_sync_002')

        try {
            expect(fileio.chownSync(fpath, 0, 0))
            expect(null).assertFail()
        } catch (e) {
        }
    })

    /**
     * @tc.number SUB_DF_FileIO_FchownSync_0000
     * @tc.name fileio_test_fchown_sync_000
     * @tc.desc Test fchownSync() interface.
     */
    it('fileio_test_fchown_sync_000', 0, async function (done) {
        let fpath = nextFileName('fileio_test_fchown_sync_000')
        expect(prepareFile(fpath, FILE_CONTENT)).assertTrue()
        let bundleInfo = await bundle_mgr.getBundleInfo("ohos.acts.distributeddatamgr.distributedfile")
        let UID = bundleInfo.uid
        let GID = bundleInfo.gid

        try {
            let fd = fileio.openSync(fpath, 0o2)
            expect(fileio.fchownSync(fd, UID, GID))
            expect(fd).assertInstanceOf('Number')
            expect(fileio.closeSync(fd)).assertNull()
            expect(fileio.unlinkSync(fpath)).assertNull()

        } catch (e) {
            console.log("fileio_test_fchown_sync_000 has failed for " + e)
            expect(null).assertFail()
        }
        done()
    })

    /**
     * @tc.number SUB_DF_FileIO_FchownSync_0010
     * @tc.name fileio_test_fchown_sync_001
     * @tc.desc Test fchownSync() interface.
     */
    it('fileio_test_fchown_sync_001', 0, async function (done) {
        let bundleInfo = await bundle_mgr.getBundleInfo("ohos.acts.distributeddatamgr.distributedfile")
        let UID = bundleInfo.uid
        let GID = bundleInfo.gid

        try {
            expect(fileio.fchownSync(-1, UID, GID))
            expect(null).assertFail()
        } catch (e) {
        }
        done()
    })

    /**
     * @tc.number SUB_DF_FileIO_FchownSync_0020
     * @tc.name fileio_test_fchown_sync_002
     * @tc.desc Test fchownSync() interface.
     */
    it('fileio_test_fchown_sync_002', 0, function () {
        let fd
        let fpath = nextFileName('fileio_test_fchown_sync_002')
        expect(prepareFile(fpath, FILE_CONTENT)).assertTrue()

        try {
            fd = fileio.openSync(fpath, 0o2)
            fileio.fchownSync(fd, 0, 0)
            expect(null).assertFail()

        } catch (e) {
            expect(fileio.closeSync(fd)).assertNull()
            expect(fileio.unlinkSync(fpath)).assertNull()
        }
    })
})