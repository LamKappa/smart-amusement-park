/*
* Copyright (c) 2021 Huawei Device Co., Ltd.
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/
import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index'
import storage from '@ohos.data.storage'

const PATH = '/data/test_storage';
const KEY_TEST_INT_ELEMENT = 'key_test_int';
const KEY_TEST_LONG_ELEMENT = 'key_test_long';
const KEY_TEST_FLOAT_ELEMENT = 'key_test_float';
const KEY_TEST_BOOLEAN_ELEMENT = 'key_test_boolean';
const KEY_TEST_STRING_ELEMENT = 'key_test_string';

var mPref;

describe('storageTest', function () {
    beforeAll(function() {
        console.info('beforeAll')
        mPref = storage.getStorageSync(PATH);
    })

    afterAll(function () {
        console.info('afterAll')
        storage.deleteStorageSync(PATH);
    })

    it('testClear001', 0, function () {
        mPref.putSync(KEY_TEST_STRING_ELEMENT, "test");
        mPref.putSync(KEY_TEST_INT_ELEMENT, 3);
        mPref.flushSync();
        mPref.clearSync();
        expect("defaultvalue").assertEqual(mPref.getSync(KEY_TEST_STRING_ELEMENT, "defaultvalue"));
        expect(0).assertEqual(mPref.getSync(KEY_TEST_INT_ELEMENT, 0));
    })

    /**
     * @tc.name Constructor test
     * @tc.number SUB_DDM_AppDataFWK_JSPreferences_Sync_0010
     * @tc.desc Constructor test
     */
    it('testConstructor002', 0, function () {
        expect(mPref).assertInstanceOf('Object');
    })

    /**
     * @tc.name has sync inteface test
     * @tc.number SUB_DDM_AppDataFWK_JSPreferences_Sync_0020
     * @tc.desc has sync inteface test
     */
    it('testHasKey003', 0, function () {
        mPref.putSync(KEY_TEST_STRING_ELEMENT, "test");
        expect(true).assertEqual(mPref.hasSync(KEY_TEST_STRING_ELEMENT));
    })

    /**
     * @tc.name get boolean sync inteface test
     * @tc.number SUB_DDM_AppDataFWK_JSPreferences_Sync_0030
     * @tc.desc get boolean sync inteface test
     */
    it('testGetBoolean005', 0, function () {
        mPref.putSync(KEY_TEST_BOOLEAN_ELEMENT, true);
        expect(true).assertEqual(mPref.hasSync(KEY_TEST_BOOLEAN_ELEMENT));
    })

    /**
     * @tc.name get defaltValue sync inteface test
     * @tc.number SUB_DDM_AppDataFWK_JSPreferences_Sync_0040
     * @tc.desc get defaltValue sync inteface test
     */
    it('testGetDefValue006', 0, function () {
        mPref.clearSync();
        expect(-1).assertEqual(mPref.getSync(KEY_TEST_INT_ELEMENT, -1));
        expect(1.0).assertEqual(mPref.getSync(KEY_TEST_FLOAT_ELEMENT, 1.0));
        expect(10000).assertEqual(mPref.getSync(KEY_TEST_LONG_ELEMENT, 10000));
        expect(true).assertEqual(mPref.getSync(KEY_TEST_BOOLEAN_ELEMENT, true));
        expect('defaultValue').assertEqual(mPref.getSync(KEY_TEST_STRING_ELEMENT, "defaultValue"));
    })

    /**
     * @tc.name get float sync inteface test
     * @tc.number SUB_DDM_AppDataFWK_JSPreferences_Sync_0050
     * @tc.desc get float sync inteface test
     */
    it('testGetFloat007', 0, function () {
        mPref.clearSync();
        mPref.putSync(KEY_TEST_FLOAT_ELEMENT, 3.0);
        expect(3.0).assertEqual(mPref.getSync(KEY_TEST_FLOAT_ELEMENT, 0.0), 0);
        expect(0.0).assertEqual(mPref.getSync(KEY_TEST_STRING_ELEMENT, 0.0), 0);
    })

    /**
     * @tc.name get int sync inteface test
     * @tc.number SUB_DDM_AppDataFWK_JSPreferences_Sync_0060
     * @tc.desc get int sync inteface test
     */
    it('testGetInt008', 0, function () {
        mPref.clearSync();
        mPref.putSync(KEY_TEST_INT_ELEMENT, 3);
        expect(3).assertEqual(mPref.getSync(KEY_TEST_INT_ELEMENT, 0.0), 0);
    })

    /**
     * @tc.name get long sync inteface test
     * @tc.number SUB_DDM_AppDataFWK_JSPreferences_Sync_0070
     * @tc.desc get long sync inteface test
     */
    it('testGetLong009', 0, function () {
        mPref.clearSync();
        mPref.putSync(KEY_TEST_LONG_ELEMENT, 3);
        expect(3).assertEqual(mPref.getSync(KEY_TEST_LONG_ELEMENT, 0));
        expect(0).assertEqual(mPref.getSync(KEY_TEST_STRING_ELEMENT, 0));
    })

    /**
     * @tc.name get String sync inteface test
     * @tc.number SUB_DDM_AppDataFWK_JSPreferences_Sync_0080
     * @tc.desc get String sync inteface test
     */
    it('testGetString10', 0, function () {
        mPref.clearSync();
        mPref.putSync(KEY_TEST_STRING_ELEMENT, "test");
        mPref.putSync(KEY_TEST_INT_ELEMENT, 3);
        mPref.flushSync();
        expect('test').assertEqual(mPref.getSync(KEY_TEST_STRING_ELEMENT, "defaultvalue"));
        expect('defaultvalue').assertEqual(mPref.getSync(KEY_TEST_INT_ELEMENT, "defaultvalue"));
    })

    /**
     * @tc.name put float sync inteface test
     * @tc.number SUB_DDM_AppDataFWK_JSPreferences_Sync_0090
     * @tc.desc put float sync inteface test
     */
    it('testPutBoolean012', 0, function () {
        mPref.clearSync();
        mPref.putSync(KEY_TEST_BOOLEAN_ELEMENT, true);
        expect(true).assertEqual(mPref.getSync(KEY_TEST_BOOLEAN_ELEMENT, false));
        mPref.flushSync();
        expect(true).assertEqual(mPref.getSync(KEY_TEST_BOOLEAN_ELEMENT, false));
    })

    /**
     * @tc.name put float sync inteface test
     * @tc.number SUB_DDM_AppDataFWK_JSPreferences_Sync_0100
     * @tc.desc put float sync inteface test
     */
    it('testPutFloat013', 0, function () {
        mPref.clearSync();
        mPref.putSync(KEY_TEST_FLOAT_ELEMENT, 4.0);
        expect(4.0).assertEqual(mPref.getSync(KEY_TEST_FLOAT_ELEMENT, 0.0));
        mPref.flushSync();
        expect(4.0).assertEqual(mPref.getSync(KEY_TEST_FLOAT_ELEMENT, 0.0));
    })

    /**
     * @tc.name put int sync inteface test
     * @tc.number SUB_DDM_AppDataFWK_JSPreferences_Sync_0110
     * @tc.desc put int sync inteface test
     */
    it('testPutInt014', 0, function () {
        mPref.clearSync();
        mPref.putSync(KEY_TEST_INT_ELEMENT, 4);
        expect(4).assertEqual(mPref.getSync(KEY_TEST_INT_ELEMENT, 0));
        mPref.flushSync();
        expect(4).assertEqual(mPref.getSync(KEY_TEST_INT_ELEMENT, 0));
    })

    /**
     * @tc.name put long sync inteface test
     * @tc.number SUB_DDM_AppDataFWK_JSPreferences_Sync_0120
     * @tc.desc put long sync inteface test
     */
    it('testPutLong015', 0, function () {
        mPref.clearSync();
        mPref.putSync(KEY_TEST_LONG_ELEMENT, 4);
        expect(4).assertEqual(mPref.getSync(KEY_TEST_LONG_ELEMENT, 0));
        mPref.flushSync();
        expect(4).assertEqual(mPref.getSync(KEY_TEST_LONG_ELEMENT, 0));
    })

    /**
     * @tc.name put String sync inteface test
     * @tc.number SUB_DDM_AppDataFWK_JSPreferences_Sync_0130
     * @tc.desc put String sync inteface test
     */
    it('testPutString016', 0, function () {
        mPref.clearSync();
        mPref.putSync(KEY_TEST_STRING_ELEMENT, "abc");
        mPref.putSync(KEY_TEST_STRING_ELEMENT, '');
        expect('').assertEqual(mPref.getSync(KEY_TEST_STRING_ELEMENT, "defaultvalue"));
        mPref.flushSync();
        expect('').assertEqual(mPref.getSync(KEY_TEST_STRING_ELEMENT, "defaultvalue"));
    })

    /**
     * @tc.name on interface test
     * @tc.number SUB_DDM_AppDataFWK_JSPreferences_Sync_0140
     * @tc.desc on interface test
     */
    it('testRegisterObserver001', 0, function () {
        mPref.clearSync();
        var observer = function (key) {
            expect('abcd').assertEqual(key);
        };
        mPref.on('change', observer);
        mPref.putSync(KEY_TEST_STRING_ELEMENT, "abcd");
    })

    /**
     * @tc.name repeat on interface test
     * @tc.number SUB_DDM_AppDataFWK_JSPreferences_Sync_0150
     * @tc.desc repeat on interface test
     */
    it('testRegisterObserver002', 0, function () {
        mPref.clearSync();
        var observer = function (key) {
            console.info('testRegisterObserver001 key' + key);
            expect('abc').assertEqual(key);
        };
        mPref.on('change', observer);
        mPref.on('change', observer);
        mPref.putSync(KEY_TEST_STRING_ELEMENT, "abc");
    })

    /**
     * @tc.name off interface test
     * @tc.number SUB_DDM_AppDataFWK_JSPreferences_Sync_0160
     * @tc.desc off interface test
     */
    it('testUnRegisterObserver001', 0, function () {
        var observer = function (key) {
            console.info('testUnRegisterObserver001 key' + key);
            expect('').assertEqual(key);
        };
        mPref.on('change', observer);
        mPref.off('change', observer);
        mPref.putSync(KEY_TEST_STRING_ELEMENT, "abc");
    })
})