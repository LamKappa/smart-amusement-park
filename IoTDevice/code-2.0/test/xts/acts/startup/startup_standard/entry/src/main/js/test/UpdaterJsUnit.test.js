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

// @ts-nocheck
import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index'
import updateTest from '@ohos.update'

describe('UpdateTest', function () {
    console.info('start################################start');
    /**
     * @tc.number    SUB_STARTUP_JS_UPDATE_0100
     * @tc.name      testVerifyUpdatePackage01
     * @tc.desc      Get Updater handler for the calling device.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('verify_update_packaget_est_001', 0, function () {
        console.info('testVerifyUpdatePackage01 start');
        var ret = false;
        var getVar = updateTest.getUpdater();
        try {
            console.info('zpz AceApplication onCreate1');
            getVar.on("verifyProgress", function (aaa){
                console.info('zpz xxxxx ' + aaa.percent);

                console.info('zpz AceApplication onCreate2');
                getVar.verifyUpdatePackage(null, null);
                getVar.off("verifyProgress", function (aaa){
                    console.info('zpz xxxxx ' + aaa.percent);
                })
            });
        } catch (e){
            console.info(" 567 callback get inputttt error:" + e);
            ret = true;
        }
        setTimeout("expect(ret).assertTrue()", "10");
        console.info('testVerifyUpdatePackage01 : PASS');
    })
})

