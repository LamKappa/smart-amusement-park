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
import systemparameter from '@ohos.systemParameter'

describe('SystemParameterTest', function () {
    console.info('SystemParameterTest start################################start');
    /**
     * @tc.number    SUB_STARTUP_JS_SYSTEM_PARAMETER_0100
     * @tc.name      testSet01
     * @tc.desc       Set the value for the given key with parameter callback.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('system_parameter_test_001', 0, function () {
        console.info('system_parameter_test_001 start');
        var ret = false;
            try {
                systemparameter.set("hw_sc.build.os.version", "10.20.30.4", function (err, data) {
                    if (err == undefined) {
                        ret = true;
                        console.info("set callback hw_sc.build.os.version value success :" + data);
                    } else {
                        console.info("set callback hw_sc.build.os.version value err:" + err.code);
                    }
                });
            }catch(e){
                console.info("set callback hw_sc.build.os.version unexpect err:" + e);
            }
        setTimeout("expect(ret).assertTrue()", "10");
        console.info('system_parameter_test_001 : PASS');
    })

    /**
     * @tc.number    SUB_STARTUP_JS_SYSTEM_PARAMETER_0200
     * @tc.name      testSet02
     * @tc.desc       Set the value for the given key.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('system_parameter_test_002', 0, function () {
        console.info('system_parameter_test_002 start');
        var ret = false;
        try {
            systemparameter.set("ro.secure", "10.20.30.4", function (err, data) {
                if (err == undefined) {
                    console.info("set callback ro.secure value success:" + data)
                } else {
                    ret = true;
                    console.info("set callback ro.secure value err:" + err.code)
                }

            });
        }catch(e){
            console.info("set callback ro.secure unexpect err:" + e)
        }
        setTimeout("expect(ret).assertTrue()", "10");
        console.info('system_parameter_test_002 : PASS');
    })

    /**
     * @tc.number    SUB_STARTUP_JS_SYSTEM_PARAMETER_0300
     * @tc.name      testSet03
     * @tc.desc       Set the value for the given key.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('system_parameter_test_003', 0, function () {
        console.info('system_parameter_test_003 start');
        var parameterInfo = systemparameter.set("hw_sc.build.os.version", "1.5.3.6");
        var ret = false;
        try {
            parameterInfo.then(function (value) {
                console.info("promise  set hw_sc.build.os.version success: " + value);
            }).catch(function (err) {
                console.info("promise  set hw_sc.build.os.version error: " + err.code);
            });
        }catch(e){
            console.info("set callback hw_sc.build.os.version unexpect err:" + e)
        }
        if (parameterInfo !== null) {
            ret = true;
        }
        expect(ret).assertTrue();
        console.info('system_parameter_test_003 : PASS');
    })

    /**
     * @tc.number    SUB_STARTUP_JS_SYSTEM_PARAMETER_0400
     * @tc.name      testSet04
     * @tc.desc       Set the value for the given key.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('system_parameter_test_004', 0, function () {
        console.info('system_parameter_test_004 start');
        var parameterInfo = systemparameter.set("ro.secure", "10");
        var ret = false;
        try {
            parameterInfo.then(function (value) {
                console.info("12333 promise  set ro.secure success: " + value);
            }).catch(function (err) {
                console.info("12333 promise  set ro.secure error: " + err.code);
            });
        }catch(e){
            console.info("set callback ro.secure unexpect err:" + e)
        }
        if (parameterInfo !== null) {
            ret = true;
        }
        expect(ret).assertTrue();
        console.info('system_parameter_test_004 ：PASS');
    })

    /**
     * @tc.number    SUB_STARTUP_JS_SYSTEM_PARAMETER_0500
     * @tc.name      testSetSync01
     * @tc.desc       Set the value for the given key.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('system_parameter_test_005', 0, function () {
        console.info('system_parameter_test_005 start');
        var ret = false;
        try {
            var parameterInfo = systemparameter.setSync("hw_sc.build.os.version", "2.5.3.7");
            console.info("promise  setSync ro.secure success: " + parameterInfo);
            ret = true;
        }catch(e){
            console.info("promise  setSync ro.secure error: " + e);
        }
        expect(ret).assertTrue();
        console.info('system_parameter_test_005 : PASS');
    })

    /**
     * @tc.number    SUB_STARTUP_JS_SYSTEM_PARAMETER_0600
     * @tc.name      testSetSync02
     * @tc.desc       Set the value for the given key.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('system_parameter_test_006', 0, function () {
        console.info('system_parameter_test_006 start');
        var ret = false;
        try {
            var parameterInfo = systemparameter.setSync("hw_sc.build.os.version", 56789);
            console.info("promise  setSync ro.secure success: " + parameterInfo);
        }catch(e){
            ret = true;
            console.info("promise  setSync ro.secure error: " + e);
        }
        expect(ret).assertTrue();
        console.info('system_parameter_test_006 : PASS');
    })

    /**
     * @tc.number    SUB_STARTUP_JS_SYSTEM_PARAMETER_0700
     * @tc.name      testGet01
     * @tc.desc       Set the value for the given key.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('system_parameter_test_007', 0, function () {
        console.info('system_parameter_test_007 start');
        var ret = false;
        try {
            var parameterInfo = systemparameter.get("ro.secure", "1");
            parameterInfo.then(function (value) {
                ret = true;
                console.info("promise get ro.secure success: " + value);
            }).catch(function (err) {
                console.info("promise get ro.secure error: " + err.code);
            });
        }catch(e){
            console.info("promise  setSync ro.secure error: " + e);
        }
        setTimeout("expect(ret).assertTrue()", "10");
        console.info('system_parameter_test_007 : PASS');
    })

    /**
     * @tc.number    SUB_STARTUP_JS_SYSTEM_PARAMETER_0800
     * @tc.name      testGet02
     * @tc.desc       Set the value for the given key.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('system_parameter_test_008', 0, function () {
        console.info('system_parameter_test_008 start');
        var ret = false;
        try {
            var parameterInfo = systemparameter.get("hw_sc.build.os.version");
            parameterInfo.then(function (value) {
                ret = true;
                console.info("promise get hw_sc.build.os.version success: " + value);
            }).catch(function (err) {
                console.info("promise get hw_sc.build.os.version error: " + err.code);
            });
        }catch(e){
            console.info("promise  setSync ro.secure error: " + e);
        }
        setTimeout("expect(ret).assertTrue()", "10");
        console.info('system_parameter_test_008 : PASS');
    })

    /**
     * @tc.number    SUB_STARTUP_JS_SYSTEM_PARAMETER_0900
     * @tc.name      testGet03
     * @tc.desc       Set the value for the given key.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('system_parameter_test_009', 0, function () {
        console.info('system_parameter_test_009 start');
        var ret = false;
        try {
            var parameterInfo = systemparameter.get("hw_sc.build.os.version", 897);
            parameterInfo.then(function (value) {
                console.info("897 promise get hw_sc.build.os.version success: " + value);
            }).catch(function (err) {
                console.info("897 promise get hw_sc.build.os.version error: " + err.code);
            });
        } catch (e) {
            ret = true;
            console.info("promise get input error: " + e);
        }
        expect(ret).assertTrue();
        console.info('system_parameter_test_009 : PASS');
    })

    /**
     * @tc.number    SUB_STARTUP_JS_SYSTEM_PARAMETER_0110
     * @tc.name      testGet04
     * @tc.desc       Set the value for the given key.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('system_parameter_test_010', 0, function () {
        console.info('system_parameter_test_010 start');
        var ret = false;
        try {
            systemparameter.get("hw_sc.build.os.version", 567, function (err, data) {
                if (err == undefined) {
                    console.info(" 567 callback get hw_sc.build.os.version value success:" + data)
                } else {
                    console.info(" 567 callback get hw_sc.build.os.version value err:" + err.code)
                }

            });
        } catch (e) {
            ret = true;
            console.info(" 567 callback get inputttt error:" + e)
        }
        expect(ret).assertTrue();
        console.info('system_parameter_test_010 : PASS');
    })

    /**
     * @tc.number    SUB_STARTUP_JS_SYSTEM_PARAMETER_0120
     * @tc.name      testGet05
     * @tc.desc       Set the value for the given key.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('system_parameter_test_011', 0, function () {
        console.info('system_parameter_test_011 start');
        var ret = false;
        try {
            systemparameter.get("hw_sc.build.os.version", "10.20.30.4", function (err, data) {
                if (err == undefined) {
                    ret = true;
                    console.info(" callback get hw_sc.build.os.version value success:" + data)
                } else {
                    console.info(" callback get hw_sc.build.os.version value err:" + err.code)
                }

            });
        } catch (e) {
            ret = true;
            console.info(" 567 callback get inputttt error:" + e)
        }
        setTimeout("expect(ret).assertTrue()", "10");
        console.info('system_parameter_test_011 : PASS');
    })

    /**
     * @tc.number    SUB_STARTUP_JS_SYSTEM_PARAMETER_0130
     * @tc.name      testGet06
     * @tc.desc       Set the value for the given key.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('system_parameter_test_012', 0, function () {
        console.info('system_parameter_test_012 start');
        var ret = false;
        try {
            systemparameter.get("ro.secure", "zbc", function (err, data) {
                if (err == undefined) {
                    ret = true;
                    console.info("callback get ro.secure value success:" + data)
                } else {
                    console.info("callback get ro.secure value err:" + err.code)
                }

            });
        } catch (e) {
            ret = true;
            console.info(" 567 callback get inputttt error:" + e)
        }
        setTimeout("expect(ret).assertTrue()", "10");
        console.info('system_parameter_test_012 : PASS');
    })

    /**
     * @tc.number    SUB_STARTUP_JS_SYSTEM_PARAMETER_0140
     * @tc.name      testGet07
     * @tc.desc      Gets the value of the attribute with the specified key.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('system_parameter_test_013', 0, function () {
        console.info('system_parameter_test_013 start');
        var ret = false;
        try {
            systemparameter.get("ro.secure", function (err, data) {
                if (err == undefined) {
                    ret = true;
                    console.info("callback get ro.secure value success:" + data)
                } else {
                    console.info("callback get ro.secure value err:" + err.code)
                }

            });
        } catch (e) {
            ret = true;
            console.info("callback get inputttt error:" + e)
        }
        setTimeout("expect(ret).assertTrue()", "10");
        console.info('system_parameter_test_013 : PASS');
    })

    /**
     * @tc.number    SUB_STARTUP_JS_SYSTEM_PARAMETER_0150
     * @tc.name      testGetSync01
     * @tc.desc      Gets the value of the attribute with the specified key.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('system_parameter_test_014', 0, function () {
        console.info('system_parameter_test_014 start');
        var ret = false;
        try {
            var parameterInfo = systemparameter.getSync("hw_sc.build.os.version", 496);
            parameterInfo.then(function (value) {
                console.info("496 promise get hw_sc.build.os.version success: " + value);
            }).catch(function (err) {
                console.info("496 promise get hw_sc.build.os.version error: " + err.code);
            });
        } catch (e) {
            ret = true;
            console.info("promise get input error: " + e);
        }
        expect(ret).assertTrue();
        console.info('system_parameter_test_014 : PASS');
    })

})