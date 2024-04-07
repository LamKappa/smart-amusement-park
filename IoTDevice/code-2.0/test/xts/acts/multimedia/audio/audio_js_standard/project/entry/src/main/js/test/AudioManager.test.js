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

import audio from '@ohos.multimedia.audio';
import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index'
describe('AudioManager.test.js', function () {
    const audioManager = audio.getAudioManager();
    var maxVolume = 1;
    var minVolume = 0;
    var deviceTypeValue = null;
    var deviceRoleValue = null;

    beforeAll(function () {
        console.info('beforeAll: Prerequisites at the test suite level, which are executed before the test suite is executed.');

    })

    beforeEach(function () {
        console.info('beforeEach: Prerequisites at the test case level, which are executed before each test case is executed.');

    })
    afterEach(function () {
        console.info('afterEach: Test case-level clearance conditions, which are executed after each test case is executed.');

    })
    afterAll(function () {
        console.info('afterAll: Test suite-level cleanup condition, which is executed after the test suite is executed');

    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_MANAGER_SetVolume_001
        * @tc.name      : Set Audiomanager with volume type media 01
        * @tc.desc      : Audiomanager SetVolume media-success
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
    it('SUB_MEDIA_AUDIO_MANAGER_SetVolume_001', 0, function () {
        audioManager.setVolume(1, 12).then(function (data) {
            console.info('Media setVolume successful promise');
            audioManager.getVolume(1).then(function (data) {
                console.info('Media getVolume promise ' + data);
                expect(data).assertEqual(12);
                console.info('testCase_SUB_MEDIA_AUDIO_MANAGER_SetVolume_001 :  PASS');
            });
        });
    })
    
    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_MANAGER_SetVolume_002
        * @tc.name      : Set Audiomanager with volume type ringtone 02
        * @tc.desc      : Audiomanager SetVolume media-success
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
    it('SUB_MEDIA_AUDIO_MANAGER_SetVolume_002', 0, function () {
        audioManager.setVolume(2, 11).then(function (data) {
            console.info('Media setVolume successful promise ');
            audioManager.getVolume(2).then(function (data) {
                console.info('Media getVolume promise ' + data);
                expect(data).assertEqual(11);
                console.info('testCase_SUB_MEDIA_AUDIO_MANAGER_SetVolume_002 :  PASS');
            });
        });
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_MANAGER_SetVolume_003
        * @tc.name      : Set Audiomanager with volume type media and callback
        * @tc.desc      : Audiomanager SetVolume media-success
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
    it('SUB_MEDIA_AUDIO_MANAGER_SetVolume_003', 0, function () {
        audioManager.setVolume(1, 10, (err, value) => {
            if (err) {
                console.error(`failed to set volume ${err.message}`);
                return;
            }
            console.log(`Media setVolume successful callback`);
            audioManager.getVolume(1, (err, value) => {
                if (err) {
                    console.error(`failed to get volume ${err.message}`);
                    return;
                }
                console.log(`Media getVolume  ${value}`);
                expect(value).assertEqual(10);
                console.info('testCase_SUB_MEDIA_AUDIO_MANAGER_SetVolume_003 :  PASS');
            });
        });
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_MANAGER_SetVolume_004
        * @tc.name      : Set Audiomanager with volume type ringtone and callback
        * @tc.desc      : Audiomanager should set the volume as per the API and return it from Get API
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
    it('SUB_MEDIA_AUDIO_MANAGER_SetVolume_004', 0, function () {
        audioManager.setVolume(2, 13, (err, value) => {
            if (err) {
                console.error(`failed to set volume ${err.message}`);
                return;
            }
            console.log(`Media setVolume successful callback`);
            audioManager.getVolume(1, (err, value) => {
                if (err) {
                    console.error(`failed to get volume ${err.message}`);
                    return;
                }
                console.log(`Media getVolume  ${value}`);
                expect(value).assertEqual(13);
                console.info('testCase_SUB_MEDIA_AUDIO_MANAGER_SetVolume_004 :  PASS');
            });
        });
    })


    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_MANAGER_SetVolume_005
        * @tc.name      : Set Audiomanager with volume level beyond maxvolume
        * @tc.desc      : Audiomanager should return an error for setting the volume
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
    it('SUB_MEDIA_AUDIO_MANAGER_SetVolume_005', 0, function () {
        audioManager.setVolume(2, 16, (err, value) => {
            if (err) {
                console.error(`failed to set volume ${err.message}`);
                return;
            }
            audioManager.getVolume(2, (err, value) => {
                if (err) {
                    console.error(`failed to get volume ${err.message}`);
                    return;
                }
                console.log(`Media getVolume  ${value}`);
                if (value != 16) {
                    expect(true).assertTrue();
                    console.info('testCase_SUB_MEDIA_AUDIO_MANAGER_SetVolume_005 :  PASS');
                }
            });
        });
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_MANAGER_GetMaxVolume_001
        * @tc.name      : Check Audiomanager get max volume for media
        * @tc.desc      : Audiomanager get max volume
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
    it('SUB_MEDIA_AUDIO_MANAGER_GetMaxVolume_001', 0, function () {
        audioManager.getMaxVolume(1).then(function (data) {
            console.info('Media getMaxVolume promise ' + data);
            expect(data).assertEqual(maxVolume);
            console.info('testCase_SUB_MEDIA_AUDIO_MANAGER_GetMaxVolume_001 :  PASS');
        });
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_MANAGER_GetMaxVolume_002
        * @tc.name      : Check Audiomanager get max volume for ringtone
        * @tc.desc      : Audiomanager get max volume-ringtone
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
    it('SUB_MEDIA_AUDIO_MANAGER_GetMaxVolume_002', 0, function () {

        audioManager.getMaxVolume(2).then(function (data) {
            console.info('Media getMaxVolume promise ' + data);
            expect(data).assertEqual(maxVolume);
            console.info('testCase_SUB_MEDIA_AUDIO_MANAGER_GetMaxVolume_002 :  PASS');
        });
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_MANAGER_GetMaxVolume_003
        * @tc.name      : Check Audiomanager get max volume for media with callback
        * @tc.desc      : Audiomanager get max volume
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
    it('SUB_MEDIA_AUDIO_MANAGER_GetMaxVolume_003', 0, function () {
        audioManager.getMaxVolume(1, (err, value) => {
            if (err) {
                console.error(`failed to get volume ${err.message}`);
                return;
            }
            console.log(`Media getMaxVolume  ${value}`);
            expect(value).assertEqual(maxVolume);
            console.info('testCase_SUB_MEDIA_AUDIO_MANAGER_GetMaxVolume_003 :  PASS');
        });
    });

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_MANAGER_GetMaxVolume_004
        * @tc.name      : Check Audiomanager get max volume for ringtone with callback
        * @tc.desc      : Audiomanager get max volume - rigtone
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
    it('SUB_MEDIA_AUDIO_MANAGER_GetMaxVolume_004', 0, function () {
        audioManager.getMaxVolume(2, (err, value) => {
            if (err) {
                console.error(`failed to get volume ${err.message}`);
                return;
            }
            console.log(`Media getMaxVolume  ${value}`);
            expect(value).assertEqual(maxVolume);
            console.info('testCase_SUB_MEDIA_AUDIO_MANAGER_GetMaxVolume_004 :  PASS');
        });
    });

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_MANAGER_GetMinVolume_001
        * @tc.name      : Check Audiomanager get min volume for media
        * @tc.desc      : Audiomanager get min volume- media
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
    it('SUB_MEDIA_AUDIO_MANAGER_GetMinVolume_001', 0, function () {
        audioManager.getMinVolume(1).then(function (data) {
            console.info('Media getMinVolume promise ' + data);
            expect(data).assertEqual(minVolume);
            console.info('testCase_SUB_MEDIA_AUDIO_MANAGER_GetMinVolume_001 : PASS');
        });
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_MANAGER_GetMinVolume_002
        * @tc.name      : Check Audiomanager get min volume for ringtone
        * @tc.desc      : Audiomanager get min volume-ringtone
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
    it('SUB_MEDIA_AUDIO_MANAGER_GetMinVolume_002', 0, function () {
        audioManager.getMinVolume(2).then(function (data) {
            console.info('Media getMinVolume promise ' + data);
            expect(data).assertEqual(minVolume);
            console.info('testCase_SUB_MEDIA_AUDIO_MANAGER_GetMinVolume_002 : PASS');
        });
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_MANAGER_GetMinVolume_003
        * @tc.name      : Check Audiomanager get min volume for media with callback
        * @tc.desc      : Audiomanager get min volume- media
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
    it('SUB_MEDIA_AUDIO_MANAGER_GetMinVolume_003', 0, function () {
        audioManager.getMinVolume(1, (err, value) => {
            if (err) {
                console.error(`failed to get volume ${err.message}`);
                return;
            }
            console.log(`Media getMinVolume  ${value}`);
            expect(value).assertEqual(minVolume);
            console.info('testCase_SUB_MEDIA_AUDIO_MANAGER_GetMinVolume_003 : PASS');
        });
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_MANAGER_GetMinVolume_004
        * @tc.name      : Check Audiomanager get min volume for ringtone
        * @tc.desc      : Audiomanager get min volume-ringtone
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
    it('SUB_MEDIA_AUDIO_MANAGER_GetMinVolume_004', 0, function () {
        audioManager.getMinVolume(2, (err, value) => {
            if (err) {
                console.error(`failed to get volume ${err.message}`);
                return;
            }
            console.log(`Media getMinVolume  ${value}`);
            expect(value).assertEqual(minVolume);
            console.info('testCase_SUB_MEDIA_AUDIO_MANAGER_GetMinVolume_004 : PASS');
        });
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_MANAGER_Get_Devices_001
        * @tc.name      : Check Audiomanager get devices with DEVICES_FLAG 01
        * @tc.desc      : Audiomanager get device-positive return value
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
    it('SUB_MEDIA_AUDIO_MANAGER_Get_Devices_001', 0, function () {
        deviceRoleValue = null;
        deviceTypeValue = null;
        audioManager.getDevices(1, (err, value) => {
            if (err) {
                console.error(`failed to get devices ${err.message}`);
                return;
            }
            console.log('getDevices output devices');
            value.forEach(displayDeviceProp);
            if (deviceTypeValue != null && deviceRoleValue != null) {
                expect(true).assertTrue();
                console.info('testCase_SUB_MEDIA_AUDIO_MANAGER_Get Devices_001 :  PASS');
            }
        });
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_MANAGER_Get_Devices_002
        * @tc.name      : Check Audiomanager get devices with DEVICES_FLAG 02
        * @tc.desc      : Audiomanager get device-positive return value
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
    it('SUB_MEDIA_AUDIO_MANAGER_Get_Devices_002', 0, function () {
        deviceRoleValue = null;
        deviceTypeValue = null;
        audioManager.getDevices(2, (err, value) => {
            if (err) {
                console.error(`failed to get devices ${err.message}`);
                return;
            }
            console.log('getDevices output devices');
            value.forEach(displayDeviceProp);
            if (deviceTypeValue != null && deviceRoleValue != null) {
                expect(true).assertTrue();
                console.info('testCase_SUB_MEDIA_AUDIO_MANAGER_Get Devices_002 :  PASS');
            }
        });
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_MANAGER_Get_Devices_003
        * @tc.name      : Check Audiomanager get devices with DEVICES_FLAG 03
        * @tc.desc      : Audiomanager get device-positive return value
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
    it('SUB_MEDIA_AUDIO_MANAGER_Get_Devices_003', 0, function () {
        deviceRoleValue = null;
        deviceTypeValue = null;
        audioManager.getDevices(3, (err, value) => {
            if (err) {
                console.error(`failed to get devices ${err.message}`);
                return;
            }
            console.log('getDevices All devices');
            value.forEach(displayDeviceProp);
            if (deviceTypeValue != null && deviceRoleValue != null) {
                expect(true).assertTrue();
                console.info('testCase_SUB_MEDIA_AUDIO_MANAGER_Get Devices_003 :  PASS');
            }
        });
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_MANAGER_GetDevice_004
        * @tc.name      : Check Audiomanager get devices with deviceflag value as 1
        * @tc.desc      : Audiomanager get getdevices without callback
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
    it('SUB_MEDIA_AUDIO_MANAGER_Get_Devices_004', 0, function () {
        deviceRoleValue = null;
        deviceTypeValue = null;
        audioManager.getDevices(1).then(function (value) {
            value.forEach(displayDeviceProp);
            if (deviceTypeValue != null && deviceRoleValue != null) {
                expect(true).assertTrue();
                console.info('testCase_SUB_MEDIA_AUDIO_MANAGER_Get Devices_004 :  PASS');
            }
        });
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_MANAGER_GetDevice_005
        * @tc.name      : Check Audiomanager get devices with deviceflag values as 2
        * @tc.desc      : Audiomanager get getdevices without callback
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
    it('SUB_MEDIA_AUDIO_MANAGER_Get_Devices_005', 0, function () {
        deviceRoleValue = null;
        deviceTypeValue = null;
        audioManager.getDevices(2).then(function (value) {
            value.forEach(displayDeviceProp);
            if (deviceTypeValue != null && deviceRoleValue != null) {
                expect(true).assertTrue();
                console.info('testCase_SUB_MEDIA_AUDIO_MANAGER_Get Devices_005 :  PASS');
            }
        });
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_MANAGER_GetDevice_006
        * @tc.name      : Check Audiomanager get devices with deviceflag values as 3
        * @tc.desc      : Audiomanager get getdevices without callback
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
    it('SUB_MEDIA_AUDIO_MANAGER_Get_Devices_006', 0, function () {
        deviceRoleValue = null;
        deviceTypeValue = null;
        audioManager.getDevices(3).then(function (value) {
            value.forEach(displayDeviceProp);
            if (deviceTypeValue != null && deviceRoleValue != null) {
                expect(true).assertTrue();
                console.info('testCase_SUB_MEDIA_AUDIO_MANAGER_Get Devices_006 :  PASS');
            }
        });
    })

    function displayDeviceProp(value, index, array) {
        console.log(`device role: ${value.deviceRole}`);
        deviceRoleValue = value.deviceRole;
        console.log(`device type: ${value.deviceType}`);
        deviceTypeValue = value.deviceType;
    }
})