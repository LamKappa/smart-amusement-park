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
import Intl from "@ohos.intl"
import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index'

describe('intlTest', function () {
    console.log("*************00000000000000000000000*************");

    /* *
    * @tc.number SUB_GLOBAL_I18N_JS_0100
    * @tc.name format the language in locale
    * @tc.desc check the language
    */
    it('locale_test_001', 0, function () {
        var locale = new Intl.Locale("en-Latn-GB");
        console.log("jessie " + locale.language);
        expect(locale.language).assertEqual("en");
    })

    /* *
    * @tc.number SUB_GLOBAL_I18N_JS_0200
    * @tc.name format the script in locale
    * @tc.desc check the script
    */
    it('locale_test_002', 0, function () {
        var locale = new Intl.Locale("en-Latn-GB");
        console.log("jessie " + locale.script);
        expect(locale.script).assertEqual("Latn");
    })

    /* *
    * @tc.number SUB_GLOBAL_I18N_JS_0300
    * @tc.name format the region in locale
    * @tc.desc check the region
    */
    it('locale_test_003', 0, function () {
        var locale = new Intl.Locale("en-Latn-GB");
        console.log("jessie " + locale.region);
        expect(locale.region).assertEqual("GB");
    })

    /* *
    * @tc.number SUB_GLOBAL_I18N_JS_0400
    * @tc.name format the basename in locale
    * @tc.desc check the basename
    */
    it('locale_test_004', 0, function () {
        var locale = new Intl.Locale("en-Latn-GB");
        console.log("jessie " + locale.baseName);
        expect(locale.baseName).assertEqual("en-Latn-GB");
    })

    /* *
    * @tc.number SUB_GLOBAL_I18N_JS_0500
    * @tc.name format the datetime with en-GB locale
    * @tc.desc check the datetime is not null
    */
    it('dateTimeFormat_test_001', 0, function () {
        var datefmt = new Intl.DateTimeFormat("en-GB");
        expect(datefmt != null).assertTrue();
    })

    /* *
    * @tc.number SUB_GLOBAL_I18N_JS_0600
    * @tc.name format the date with en-GB locale
    * @tc.desc check the date
    */
    it('formatDateTimeFormat_test_001', 0, function () {
        var date = new Date(2021, 11, 17, 3, 24, 0);
        var datefmt = new Intl.DateTimeFormat("en-GB");
        expect(datefmt.format(date)).assertEqual('17 Dec 2021');
    })
    console.log("*************00000000000000000000000*************");
}) 