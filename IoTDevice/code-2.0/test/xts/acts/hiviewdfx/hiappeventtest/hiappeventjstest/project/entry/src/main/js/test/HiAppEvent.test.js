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
import HiAppEvent from '@ohos.hiappevent'
import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index'
import Constant from 'deccjsunit/src/Constant'

describe('HiAppEventApiTest', function () {
    /**
     * @tc.number DFX_DFT_HiviewKit_HiAppEvent_JSNAPI_0100
     * @tc.name testHiAppEventApi01
     * @tc.desc HiAppEvent write interface test.
     */
    it('testHiAppEventApi01', Function|MediumTest|Level1, function () {
        console.info('testHiAppEventApi01 start')
        var result = false;
        HiAppEvent.write("write", HiAppEvent.EventType.FAULT, "key_int", 100, "key_float", 1.1, "key_string", "demo", "key_bool", true,
            "key_array_int", [1, 2, 3], "key_array_float", [1.1, 2.2, 3.3], "key_array_str", ["a", "b", "c"], "key_array_bool", [true, false],
            "key_array_int2", [1, 2, 3], "key_array_float2", [1.1, 2.2, 3.3], "key_array_str2", ["a", "b", "c"], "key_array_bool2", [true, false],
            (err, value) => {
                console.log(`HiAppEvent into callback`);
                if (err) {
                    console.error(`HiAppEvent json-callback-error code=${err.code}`);
                    result = false;
                } else {
                    console.log(`HiAppEvent json-callback-success value=${value}`);
                    result = true;
                }
                console.log(`HiAppEvent result=${result}`);
                expect(result).assertTrue();
                console.log(`HiAppEvent result123=${result}`);

            });
    })

    /**
     * @tc.number DFX_DFT_HiviewKit_HiAppEvent_JSNAPI_0200
     * @tc.name testHiAppEventApi02
     * @tc.desc HiAppEvent write json interface test.
     */
    it('testHiAppEventApi02', Function|MediumTest|Level1, function () {
        console.info('testHiAppEventApi02 start')
        var result = false;
        HiAppEvent.writeJson("writeJson", HiAppEvent.EventType.FAULT, {"key_int": 100, "key_string": "demo", "key_bool":true, "key_float":1.1
        ,"key_array_int": [1, 2, 3], "key_array_float": [1.1, 2.2, 3.3], "key_array_str": ["a", "b", "c"], "key_array_bool": [true, false]
        ,"key_array_int2": [1, 2, 3], "key_arr_float2": [1.1, 2.2, 3.3], "key_arr_str2": ["a", "b", "c"], "key_array_bool2": [true, false]
        },
            (err, value) => {
                console.log(`HiAppEvent into json-callback`);
                if (err) {
                    console.error(`HiAppEvent json-callback-error code=${err.code}`);
                    result = false;
                } else {
                    console.log(`HiAppEvent json-callback-success value=${value}`);
                    result = true;
                }
                expect(result).assertTrue()
            });
    })

    /**
     * @tc.number DFX_DFT_HiviewKit_HiAppEvent_JSNAPI_0300
     * @tc.name testHiAppEventApi03
     * @tc.desc HiAppEvent write EventType of STATISTIC.
     */
    it('testHiAppEventApi03', Function|MediumTest|Level2, function () {
        console.info('testHiAppEventApi03 start')
        var result = false;
        HiAppEvent.write("write", HiAppEvent.EventType.STATISTIC, "key_int", 100, "key_float", 1.1, "key_string", "demo", "key_bool", true,
            "key_array_int", [1, 2, 3], "key_array_float", [1.1, 2.2, 3.3], "key_array_str", ["a", "b", "c"], "key_array_bool", [true, false],
            "key_array_int2", [1, 2, 3], "key_array_float2", [1.1, 2.2, 3.3], "key_array_str2", ["a", "b", "c"], "key_array_bool2", [true, false],
            (err, value) => {
                console.log(`HiAppEvent into json-callback`);
                if (err) {
                    console.error(`HiAppEvent json-callback-error code=${err.code}`);
                    result = false;
                } else {
                    console.log(`HiAppEvent json-callback-success value=${value}`);
                    result = true;
                }
                expect(result).assertTrue();
            });
        console.info('testHiAppEventApi03 end')
    })

    /**
     * @tc.number DFX_DFT_HiviewKit_HiAppEvent_JSNAPI_0400
     * @tc.name testHiAppEventApi04
     * @tc.desc HiAppEvent write EventType of SECURITY.
     */
    it('testHiAppEventApi04', Function|MediumTest|Level2, function () {
        console.info('testHiAppEventApi04 start')
        var result = false;
        HiAppEvent.write("write", HiAppEvent.EventType.SECURITY, "key_int", 100, "key_float", 1.1, "key_string", "demo", "key_bool", true,
            "key_array_int", [1, 2, 3], "key_array_float", [1.1, 2.2, 3.3], "key_array_str", ["a", "b", "c"], "key_array_bool", [true, false],
            "key_array_int2", [1, 2, 3], "key_array_float2", [1.1, 2.2, 3.3], "key_array_str2", ["a", "b", "c"], "key_array_bool2", [true, false],
            (err, value) => {
                console.log(`HiAppEvent into json-callback`);
                if (err) {
                    console.error(`HiAppEvent json-callback-error code=${err.code}`);
                    result = false;
                } else {
                    console.log(`HiAppEvent json-callback-success value=${value}`);
                    result = true;
                }
                expect(result).assertTrue();
            });
        console.info('testHiAppEventApi04 end')
    })

    /**
     * @tc.number DFX_DFT_HiviewKit_HiAppEvent_JSNAPI_0500
     * @tc.name testHiAppEventApi05
     * @tc.desc HiAppEvent write EventType of BEHAVIOR.
     */
    it('testHiAppEventApi05', Function|MediumTest|Level2, function () {
        console.info('testHiAppEventApi05 start')
        var result = false;
        HiAppEvent.write("write", HiAppEvent.EventType.BEHAVIOR, "key_int", 100, "key_float", 1.1, "key_string", "demo", "key_bool", true,
            "key_array_int", [1, 2, 3], "key_array_float", [1.1, 2.2, 3.3], "key_array_str", ["a", "b", "c"], "key_array_bool", [true, false],
            "key_array_int2", [1, 2, 3], "key_array_float2", [1.1, 2.2, 3.3], "key_array_str2", ["a", "b", "c"], "key_array_bool2", [true, false],
            (err, value) => {
                console.log(`HiAppEvent into json-callback`);
                if (err) {
                    console.error(`HiAppEvent json-callback-error code=${err.code}`);
                    result = false;
                } else {
                    console.log(`HiAppEvent json-callback-success value=${value}`);
                    result = true;
                }
                expect(result).assertTrue();
            });
        console.info('testHiAppEventApi05 end')
    })


    /**
     * @tc.number DFX_DFT_HiviewKit_HiAppEvent_JSNAPI_0600
     * @tc.name testHiAppEventApi06
     * @tc.desc HiAppEvent write datatype of int.
     */
    it('testHiAppEventApi06', Function|MediumTest|Level2, function () {
        console.info('testHiAppEventApi06 start')
        var result = false;
        HiAppEvent.write("write", HiAppEvent.EventType.FAULT, "key_int", 100,
            (err, value) => {
                console.log(`HiAppEvent into json-callback`);
                if (err) {
                    console.error(`HiAppEvent json-callback-error code=${err.code}`);
                    result = false;
                } else {
                    console.log(`HiAppEvent json-callback-success value=${value}`);
                    result = true;
                }
                expect(result).assertTrue();
            });
        console.info('testHiAppEventApi06 end')
    })

    /**
     * @tc.number DFX_DFT_HiviewKit_HiAppEvent_JSNAPI_0700
     * @tc.name testHiAppEventApi07
     * @tc.desc HiAppEvent write datatype of int array.
     */
    it('testHiAppEventApi07', Function|MediumTest|Level2, function () {
        console.info('testHiAppEventApi07 start')
        var result = false;
        HiAppEvent.write("write", HiAppEvent.EventType.FAULT, "key_int_array", [100, 200],
            (err, value) => {
                console.log(`HiAppEvent into json-callback`);
                if (err) {
                    console.error(`HiAppEvent json-callback-error code=${err.code}`);
                    result = false;
                } else {
                    console.log(`HiAppEvent json-callback-success value=${value}`);
                    result = true;
                }
                expect(result).assertTrue();
            });
        console.info('testHiAppEventApi07 end')
    })

    /**
     * @tc.number DFX_DFT_HiviewKit_HiAppEvent_JSNAPI_0800
     * @tc.name testHiAppEventApi08
     * @tc.desc HiAppEvent write datatype of float.
     */
    it('testHiAppEventApi08', Function|MediumTest|Level2, function () {
        console.info('testHiAppEventApi08 start')
        var result = false;
        HiAppEvent.write("write", HiAppEvent.EventType.FAULT, "key_float", 1.1,
            (err, value) => {
                console.log(`HiAppEvent into json-callback`);
                if (err) {
                    console.error(`HiAppEvent json-callback-error code=${err.code}`);
                    result = false;
                } else {
                    console.log(`HiAppEvent json-callback-success value=${value}`);
                    result = true;
                }
                expect(result).assertTrue();
            });
        console.info('testHiAppEventApi08 end')
    })

    /**
     * @tc.number DFX_DFT_HiviewKit_HiAppEvent_JSNAPI_0900
     * @tc.name testHiAppEventApi09
     * @tc.desc HiAppEvent write datatype of float array.
     */
    it('testHiAppEventApi09', Function|MediumTest|Level3, function () {
        console.info('testHiAppEventApi09 start')
        var result = false;
        HiAppEvent.write("write", HiAppEvent.EventType.FAULT, "key_float_array", [1.1, 1.2],
            (err, value) => {
                console.log(`HiAppEvent into json-callback`);
                if (err) {
                    console.error(`HiAppEvent json-callback-error code=${err.code}`);
                    result = false;
                } else {
                    console.log(`HiAppEvent json-callback-success value=${value}`);
                    result = true;
                }
                expect(result).assertTrue();
            });
        console.info('testHiAppEventApi09 end')
    })

    /**
     * @tc.number DFX_DFT_HiviewKit_HiAppEvent_JSNAPI_1000
     * @tc.name testHiAppEventApi10
     * @tc.desc HiAppEvent write datatype of string.
     */
    it('testHiAppEventApi10', Function|MediumTest|Level3, function () {
        console.info('testHiAppEventApi10 start')
        var result = false;
        HiAppEvent.write("write", HiAppEvent.EventType.FAULT, "key_string", "hello world",
            (err, value) => {
                console.log(`HiAppEvent into json-callback`);
                if (err) {
                    console.error(`HiAppEvent json-callback-error code=${err.code}`);
                    result = false;
                } else {
                    console.log(`HiAppEvent json-callback-success value=${value}`);
                    result = true;
                }
                expect(result).assertTrue();
            });
        console.info('testHiAppEventApi10 end')
    })

    /**
     * @tc.number DFX_DFT_HiviewKit_HiAppEvent_JSNAPI_1100
     * @tc.name testHiAppEventApi11
     * @tc.desc HiAppEvent write datatype of string array.
     */
    it('testHiAppEventApi11', Function|MediumTest|Level3, function () {
        console.info('testHiAppEventApi11 start')
        var result = false;
        HiAppEvent.write("write", HiAppEvent.EventType.FAULT, "key_string_array", ["hello world", "hello world2"],
            (err, value) => {
                console.log(`HiAppEvent into json-callback`);
                if (err) {
                    console.error(`HiAppEvent json-callback-error code=${err.code}`);
                    result = false;
                } else {
                    console.log(`HiAppEvent json-callback-success value=${value}`);
                    result = true;
                }
                expect(result).assertTrue();
            });
        console.info('testHiAppEventApi11 end')
    })

    /**
     * @tc.number DFX_DFT_HiviewKit_HiAppEvent_JSNAPI_1200
     * @tc.name testHiAppEventApi12
     * @tc.desc HiAppEvent write datatype of bool with true.
     */
    it('testHiAppEventApi12', Function|MediumTest|Level3, function () {
        console.info('testHiAppEventApi12 start')
        var result = false;
        HiAppEvent.write("write", HiAppEvent.EventType.FAULT, "key_bool", true,
            (err, value) => {
                console.log(`HiAppEvent into json-callback`);
                if (err) {
                    console.error(`HiAppEvent json-callback-error code=${err.code}`);
                    result = false;
                } else {
                    console.log(`HiAppEvent json-callback-success value=${value}`);
                    result = true;
                }
                expect(result).assertTrue();
            });
        console.info('testHiAppEventApi12 end')
    })

    /**
     * @tc.number DFX_DFT_HiviewKit_HiAppEvent_JSNAPI_1300
     * @tc.name testHiAppEventApi13
     * @tc.desc HiAppEvent write datatype of bool with false.
     */
    it('testHiAppEventApi13', Function|MediumTest|Level3, function () {
        console.info('testHiAppEventApi13 start')
        var result = false;
        HiAppEvent.write("write", HiAppEvent.EventType.FAULT, "key_bool", false,
            (err, value) => {
                console.log(`HiAppEvent into json-callback`);
                if (err) {
                    console.error(`HiAppEvent json-callback-error code=${err.code}`);
                    result = false;
                } else {
                    console.log(`HiAppEvent json-callback-success value=${value}`);
                    result = true;
                }
                expect(result).assertTrue();
            });
        console.info('testHiAppEventApi13 end')
    })

    /**
     * @tc.number DFX_DFT_HiviewKit_HiAppEvent_JSNAPI_1400
     * @tc.name testHiAppEventApi14
     * @tc.desc HiAppEvent write datatype of bool array.
     */
    it('testHiAppEventApi14', Function|MediumTest|Level3, function () {
        console.info('testHiAppEventApi14 start')
        var result = false;
        HiAppEvent.write("write", HiAppEvent.EventType.FAULT, "key_bool_array", [false, true],
            (err, value) => {
                console.log(`HiAppEvent into json-callback`);
                if (err) {
                    console.error(`HiAppEvent json-callback-error code=${err.code}`);
                    result = false;
                } else {
                    console.log(`HiAppEvent json-callback-success value=${value}`);
                    result = true;
                }
                expect(result).assertTrue();
            });
        console.info('testHiAppEventApi14 end')
    })
})
