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
import app from '@system.app'
import Context from '@ohos.napi_context'

import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index'

describe('appInfoTest', function () {
    console.log("*************00000000000000000000000*************");
    it('app_info_test_001', 0, function () {
        var info = app.getInfo()
        expect(info.versionName).assertEqual('1.0')
        expect(info.versionCode).assertEqual('3')
    })
    it('app_info_test_002', 0, function () {
        var info = app.getInfo()
        expect(info.versionName).assertEqual('1.0')
        expect(info.versionCode).assertEqual('2')
        expect(info).assertNull()
    })
    it('app_info_test_003', 0, function () {
        var info = app.getInfo()
        expect(info.versionName).assertEqual('1.0')
        expect(info.versionCode).assertEqual('4')
    })
    it('app_info_test_004', 0, function () {
        var info = app.getInfo()
        expect(info.versionName).assertEqual('1.0')
        expect(info.versionCode).assertEqual('5')
    })
    it('get_process_info_test_001', 0, async function (done) {
        console.log("111")
        expect(1).assertLarger(0)
        Context.getProcessInfo().then(info => {
            console.log("222");
            console.log("process_info: " + JSON.stringify(info));
            expect(info.processName.length).assertLarger(0);
            expect(info.pid).assertLarger(0);
        });
        done()
    })
})
