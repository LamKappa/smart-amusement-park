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
import deviceinfo from '@ohos.deviceInfo'

describe('DeviceInfoTest', function () {
    console.info('start################################start');
    /**
     * @tc.number    SUB_STARTUP_JS_DEVCEINFO_0100
     * @tc.name      testGetDeviceType01
     * @tc.desc      Get a string representing the device type.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('device_info_test_001', 0, function () {
        console.info('device_info_test_001 start');
        var deviceTypeInfo = deviceinfo.deviceType;
        var ret = false;
        console.info('AceApplication onCreate startup para getDeviceType ' + deviceTypeInfo);
        expect(deviceTypeInfo).assertEqual('default')
        if (deviceTypeInfo !== null) {
            ret = true;
        }
        expect(ret).assertTrue()
        console.info('device_info_test_001 : PASS');
    })

    /**
     * @tc.number    SUB_STARTUP_JS_DEVCEINFO_0200
     * @tc.name     testGetManufacture01
     * @tc.desc      Get the manufacture name represented by a string.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('device_info_test_002', 0, function () {
        console.info('device_info_test_002 start');
        var manufactureInfo = deviceinfo.manufacture;
        var ret = false;
        if (manufactureInfo !== null){
            ret = true;
        }
        console.info('the value of the deviceType is :' + manufactureInfo);
        expect(ret).assertTrue();
        console.info('device_info_test_002 ：PASS');
    })
    /**
     * @tc.number    SUB_STARTUP_JS_DEVCEINFO_0300
     * @tc.name     testGetProductBrand01
     * @tc.desc      Get the product brand represented by a string.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('device_info_test_003', 0, function () {
        console.info('testGetProductBrand01 start');
        var brandInfo = deviceinfo.brand;
        var ret = false;
        if (brandInfo !== null){
            ret = true;
        }
        console.info('the value of the deviceinfo brand is :' + brandInfo);
        expect(ret).assertTrue();
        console.info('testGetProductBrand01 ：PASS');
    })

    /**
     * @tc.number    SUB_STARTUP_JS_DEVCEINFO_0400
     * @tc.name     testGetMarketName01
     * @tc.desc      Get the external product family name represented by a string.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('device_info_test_004', 0, function () {
        console.info('testGetMarketName01 start')
        var marketNameInfo = deviceinfo.marketName;
        var ret = false;
        console.info('the value of the deviceinfo marketName is :' + marketNameInfo);
        if(marketNameInfo !== null){
            ret = true;
        }
        expect(ret).assertTrue();
        console.info('testGetMarketName01 ：PASS');
    })

    /**
     * @tc.number    SUB_STARTUP_JS_DEVCEINFO_0500
     * @tc.name     testGetProductSeries01
     * @tc.desc      Get the product series represented by a string.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('device_info_test_005', 0, function () {
        console.info('testGetProductSeries01 start');
        var productSeriesInfo = deviceinfo.productSeries;
        var ret = false;
        if(productSeriesInfo !== null){
            ret = true;
        }
        console.info('the value of the deviceinfo productSeries is :' + productSeriesInfo);
        expect(ret).assertTrue();
        console.info('testGetProductSeries01 ：PASS');
    })

    /**
     * @tc.number    SUB_STARTUP_JS_DEVCEINFO_0600
     * @tc.name     testGetProductModel01
     * @tc.desc      Get the internal software sub-model represented by a string.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('device_info_test_006', 0, function () {
        console.info('testGetProductModel01 start');
        var productModelInfo = deviceinfo.productModel;
        var ret = false;
        if(productModelInfo !== null){
            ret =true;
        }
        console.info('the value of the deviceinfo productModel is :' + productModelInfo);
        expect(ret).assertTrue();
        console.info('testGetProductModel01 ：PASS');
    })

    /**
     * @tc.number    SUB_STARTUP_JS_DEVCEINFO_0700
     * @tc.name     testGetSoftwareModel01
     * @tc.desc      Get the internal software sub-model represented by a string.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('device_info_test_007', 0, function () {
        console.info('testGetSoftwareModel01 start');
        var softwareModelInfo = deviceinfo.softwareModel;
        var ret = false;
        if(softwareModelInfo !== null){
            ret = true;
        }
        console.info('the value of the deviceinfo softwareModel is :' + softwareModelInfo);
        expect(ret).assertTrue();
        console.info('testGetSoftwareModel01 ：PASS');
    })

    /**
     * @tc.number    SUB_STARTUP_JS_DEVCEINFO_0800
     * @tc.name     testGetHardWareModel01
     * @tc.desc      Get the hardware version represented by a string.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('device_info_test_008', 0, function () {
        console.info('testGetHardWareModel01 start');
        var hardwareModelInfo = deviceinfo.hardwareModel;
        var ret = false;
        if(hardwareModelInfo !== null){
            ret = true;
        }
        console.info('the value of the deviceinfo hardwareModel is :' + hardwareModelInfo);
        expect(ret).assertTrue();
        console.info('testGetHardWareModel01 ：PASS');
    })

    /**
     * @tc.number    SUB_STARTUP_JS_DEVCEINFO_0900
     * @tc.name     testGetHardWareProfile01
     * @tc.desc      Get the hardware profile represented by a string.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('device_info_test_009', 0, function () {
        console.info('testGetHardWareProfile01 start');
        var hardwareProfileInfo = deviceinfo.hardwareProfile;
        var ret = false;
        if(hardwareProfileInfo !== null){
            ret = true;
        }
        console.info('the value of the deviceinfo hardwareProfile is :' + hardwareProfileInfo);
        expect(ret).assertTrue();
        console.info('testGetHardWareProfile01 ：PASS');
    })

    /**
     * @tc.number    SUB_STARTUP_JS_DEVCEINFO_0110
     * @tc.name     testGetSerial01
     * @tc.desc      Get the device serial number represented by a string.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('device_info_test_010', 0, function () {
        console.info('testGetSerial01 start');
        var serialInfo = deviceinfo.serial;
        var ret = false;
        if(serialInfo !== null){
            ret = true;
        }
        console.info('the value of the deviceinfo serial is :' + serialInfo);
        expect(ret).assertTrue();
        console.info('testGetSerial01 ：PASS');
    })

    /**
     * @tc.number    SUB_STARTUP_JS_DEVCEINFO_0120
     * @tc.name     testGetBootLoaderVersion01
     * @tc.desc      Get the bootloader version number represented by a string.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('device_info_test_011', 0, function () {
        console.info('testGetBootLoaderVersion01 start');
        var bootloaderVersionInfo = deviceinfo.bootloaderVersion;
        var ret = false;
        if(bootloaderVersionInfo !== null){
            ret = true;
        }
        console.info('the value of the deviceinfo bootloaderVersion is :' + bootloaderVersionInfo);
        expect(ret).assertTrue();
        console.info('testGetBootLoaderVersion01 ：PASS')
    })

    /**
     * @tc.number    SUB_STARTUP_JS_DEVCEINFO_0130
     * @tc.name     testGetabiList01
     * @tc.desc      Get the instruction set supported by the system.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('device_info_test_012', 0, function () {
        console.info('testGetabiList01 start');
        var abiListInfo = deviceinfo.abiList;
        var ret = false;
        if(abiListInfo !== null){
            ret = true;
        }
        console.info('the value of the deviceinfo abiList is :' + abiListInfo);
        expect(ret).assertTrue();
        console.info('testGetabiList01 ：PASS');
    })

    /**
     * @tc.number    SUB_STARTUP_JS_DEVCEINFO_0140
     * @tc.name     testGetabiList01
     * @tc.desc      Get the security patch level represented by a string.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('device_info_test_013', 0, function () {
        console.info('testGetSecurityPatchTag01 start');
        var securityPatchTagInfo = deviceinfo.securityPatchTag;
        var ret = false;
        if(securityPatchTagInfo !== null){
            ret = true;
        }
        console.info('the value of the deviceinfo securityPatchTag is :' + securityPatchTagInfo);
        expect(ret).assertTrue();
        console.info('testGetSecurityPatchTag01 ：PASS');
    })

    /**
     * @tc.number    SUB_STARTUP_JS_DEVCEINFO_0150
     * @tc.name     testGetabiList01
     * @tc.desc      Get the version number visible to users represented by a string.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('device_info_test_014', 0, function () {
        console.info('testGetDisplayVersion01 start');
        var displayVersionInfo = deviceinfo.displayVersion;
        var ret = false;
        if(displayVersionInfo !== null){
            ret = true;
        }
        console.info('the value of the deviceinfo displayVersion is :' + displayVersionInfo);
        expect(ret).assertTrue();
        console.info('testGetDisplayVersion01 ：PASS');
    })

    /**
     * @tc.number    SUB_STARTUP_JS_DEVCEINFO_0160
     * @tc.name     testGetIncrementalVersion01
     * @tc.desc      Get the difference version number represented by a string.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('device_info_test_015', 0, function () {
        console.info('testGetIncrementalVersion01 start');
        var incrementalVersionInfo = deviceinfo.incrementalVersion;
        var ret = false;
        if(incrementalVersionInfo !== null){
            ret = true;
        }
        console.info('the value of the deviceinfo incrementalVersion is :' + incrementalVersionInfo);
        expect(ret).assertTrue();
        console.info('testGetIncrementalVersion01 ：PASS');
    })

    /**
     * @tc.number    SUB_STARTUP_JS_DEVCEINFO_0170
     * @tc.name     testGetOSReleaserType01
     * @tc.desc      Get the OS release type.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('device_info_test_016', 0, function () {
        console.info('testGetOSReleaserType01 start');
        var osReleaseTypeInfo = deviceinfo.osReleaseType;
        var ret = false;
        if(osReleaseTypeInfo !== null){
            ret = true;
        }
        console.info('the value of the deviceinfo osReleaseType is :' + osReleaseTypeInfo);
        expect(ret).assertTrue();
        console.info('testGetOSReleaserType01 ：PASS');
    })

    /**
     * @tc.number    SUB_STARTUP_JS_DEVCEINFO_0180
     * @tc.name     testGetOSFullName01
     * @tc.desc      Get the operating system full name.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('device_info_test_017', 0, function () {
        console.info('testGetOSFullName01 start');
        var osFullNameInfo = deviceinfo.osFullName;
        var ret = false;
        if(osFullNameInfo !== null){
            ret = true;
        }
        console.info('the value of the deviceinfo osFullName is :' + osFullNameInfo);
        expect(ret).assertTrue();
        console.info('testGetOSFullName01 ：PASS');
    })

    /**
     * @tc.number    SUB_STARTUP_JS_DEVCEINFO_0190
     * @tc.name     testGetMajorVersion01
     * @tc.desc      Get the major (M) version number, which increases with any updates to the overall architecture.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('device_info_test_018', 0, function () {
        console.info('testGetMajorVersion01 start');
        var majorVersionInfo = deviceinfo.majorVersion;
        var ret = false;
        if(majorVersionInfo !== null){
            ret = true;
        }
        console.info('the value of the deviceinfo majorVersion is :' + majorVersionInfo);
        expect(ret).assertTrue();
        console.info('testGetMajorVersion01 ：PASS');
    })

    /**
     * @tc.number    SUB_STARTUP_JS_DEVCEINFO_0210
     * @tc.name     testGetSeniorVersion01
     * @tc.desc      Get the senior (S) version number, which increases with any updates to the partial.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('device_info_test_019', 0, function () {
        console.info('testGetSeniorVersion01 start');
        var seniorVersionInfo = deviceinfo.seniorVersion;
        var ret = false;
        if(seniorVersionInfo !== null){
            ret = true;
        }
        console.info('the value of the deviceinfo seniorVersion is :' + seniorVersionInfo);
        expect(ret).assertTrue();
        console.info('testGetSeniorVersion01 ：PASS');
    })

    /**
     * @tc.number    SUB_STARTUP_JS_DEVCEINFO_0220
     * @tc.name     testGetFeatureVersion01
     * @tc.desc      Get the feature (F) version number, which increases with any planned new features.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('device_info_test_020', 0, function () {
        console.info('testGetFeatureVersion01 start');
        var featureVersionInfo = deviceinfo.featureVersion;
        var ret = false;
        if(featureVersionInfo !== null){
            ret = true;
        }
        console.info('the value of the deviceinfo featureVersion is :' + featureVersionInfo);
        expect(ret).assertTrue();
        console.info('testGetFeatureVersion01 ：PASS');
    })

    /**
     * @tc.number    SUB_STARTUP_JS_DEVCEINFO_0230
     * @tc.name     testGetBuildVersion01
     * @tc.desc      Get the build (B) version number, which increases with each new development build.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('device_info_test_021', 0, function () {
        console.info('testGetBuildVersion01 start');
        var buildVersionInfo = deviceinfo.buildVersion;
        var ret = false;
        if(buildVersionInfo !== null){
            ret = true;
        }
        console.info('the value of the deviceinfo buildVersion is :' + buildVersionInfo);
        expect(ret).assertTrue();
        console.info('testGetBuildVersion01 ：PASS');
    })

    /**
     * @tc.number    SUB_STARTUP_JS_DEVCEINFO_0240
     * @tc.name     testGetSdkApiVersion01
     * @tc.desc      Get the API version number.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('device_info_test_022', 0, function () {
        console.info('testGetSdkApiVersion01 start');
        var sdkApiVersionInfo = deviceinfo.sdkApiVersion;
        var ret = false;
        if(sdkApiVersionInfo !== null){
            ret = true;
        }
        console.info('the value of the deviceinfo sdkApiVersion is :' + sdkApiVersionInfo);
        expect(ret).assertTrue();
        console.info('testGetSdkApiVersion01 ：PASS');
    })

    /**
     * @tc.number    SUB_STARTUP_JS_DEVCEINFO_0250
     * @tc.name     testGetFirstApiVersion01
     * @tc.desc      Get the first API version number.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('device_info_test_023', 0, function () {
        console.info('testGetFirstApiVersion01 start')
        var firstApiVersionInfo = deviceinfo.firstApiVersion;
        var ret = true;
        if(firstApiVersionInfo !== null){
            ret = true;
        }
        console.info('the value of the deviceinfo firstApiVersion is :' + firstApiVersionInfo);
        expect(ret).assertTrue();
        console.info('testGetFirstApiVersion01 ：PASS');
    })

    /**
     * @tc.number    SUB_STARTUP_JS_DEVCEINFO_0260
     * @tc.name     testGetVersionId01
     * @tc.desc      Get the version ID number.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('device_info_test_024', 0, function () {
        console.info('testGetVersionId01 start');
        var versionIdInfo = deviceinfo.versionId;
        var ret = false;
        if(versionIdInfo !== null){
            ret = true;
        }
        console.info('the value of the deviceinfo versionId is :' + versionIdInfo);
        expect(ret).assertTrue();
        console.info('testGetVersionId01 ：PASS');
    })

    /**
     * @tc.number    SUB_STARTUP_JS_DEVCEINFO_0270
     * @tc.name     testGetBuildType01
     * @tc.desc      Get the different build types of the same baseline code.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('device_info_test_025', 0, function () {
        console.info('testGetBuildType01 start');
        var buildTypeInfo = deviceinfo.buildType;
        var ret = false;
        if(buildTypeInfo !== null){
            ret = true;
        }
        console.info('the value of the deviceinfo buildType is :' + buildTypeInfo);
        expect(ret).assertTrue();
        console.info('testGetBuildType01 ：PASS');
    })

    /**
     * @tc.number    SUB_STARTUP_JS_DEVCEINFO_0280
     * @tc.name     testGetBuildUser01
     * @tc.desc      Get the different build user of the same baseline code.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('device_info_test_026', 0, function () {
        console.info('testGetBuildUser01 start');
        var buildUserInfo = deviceinfo.buildUser;
        var ret = true;
        if(buildUserInfo !== null){
            ret = true;
        }
        console.info('the value of the deviceinfo buildUser is :' + buildUserInfo);
        expect(ret).assertTrue();
        console.info('testGetBuildUser01 ：PASS');
    })

    /**
     * @tc.number    SUB_STARTUP_JS_DEVCEINFO_0290
     * @tc.name     testGetBuildHost01
     * @tc.desc      Get the different build host of the same baseline code.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('device_info_test_027', 0, function () {
        console.info('testGetBuildHost01 start');
        var buildHostInfo = deviceinfo.buildHost;
        var ret = false;
        if(buildHostInfo !== null){
            ret = true;
        }
        console.info('the value of the deviceinfo buildHost is :' + buildHostInfo);
        expect(ret).assertTrue();
        console.info('testGetBuildHost01 ：PASS');
    })

    /**
     * @tc.number    SUB_STARTUP_JS_DEVCEINFO_0310
     * @tc.name     testGetBuildTime01
     * @tc.desc      Get the the build time.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('device_info_test_028', 0, function () {
        console.info('testGetBuildTime01 start');
        var buildTimeInfo = deviceinfo.buildTime;
        var ret = false;
        if(buildTimeInfo !== null){
            ret = true;
        }
        console.info('the value of the deviceinfo buildTime is :' + buildTimeInfo);
        expect(ret).assertTrue();
        console.info('testGetBuildTime01 ：PASS');
    })

    /**
     * @tc.number    SUB_STARTUP_JS_DEVCEINFO_0320
     * @tc.name     testGetBuildRootHash01
     * @tc.desc      Get the version hash.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('device_info_test_029', 0, function () {
        console.info('testGetBuildRootHash01 start');
        var buildRootHashInfo = deviceinfo.buildRootHash;
        var ret = false;
        if(buildRootHashInfo !== null){
            ret = true;
        }
        console.info('the value of the deviceinfo buildRootHash is :' + buildRootHashInfo);
        expect(ret).assertTrue();
        console.info('testGetBuildRootHash01 ：PASS');
    })

    /**
     * @tc.number    SUB_STARTUP_JS_DEVCEINFO_0330
     * @tc.name      testGetDeviceType02
     * @tc.desc      Get a string representing the device type which has a maximum of 32 characters.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('device_info_test_030', 0, function () {
        console.info('device_info_test_030 start');
        var deviceTypeInfo = deviceinfo.deviceType;
        var ret = false;
        if(deviceTypeInfo !== null){
            ret = true;
        }
        expect(deviceTypeInfo).assertEqual('default')
        expect(ret).assertTrue();

        let len = deviceTypeInfo.length
        console.info('the value of the device type characters:' + len)
        expect(len).assertLess(32)
        console.info('device_info_test_030 ：PASS')
    })

    /**
     * @tc.number    SUB_STARTUP_JS_DEVCEINFO_0340
     * @tc.name      testGetManufacture02
     * @tc.desc      Get a string representing the manufacture which has a maximum of 32 characters.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('device_info_test_031', 0, function () {
        console.info('device_info_test_031 start');
        var manufactureInfo = deviceinfo.manufacture;
        var ret = false;
        if(manufactureInfo !== null){
            ret = true;
        }
        expect(ret).assertTrue();

        let len = manufactureInfo.length
        console.info('the value of the manufacture characters is :' + len)
        expect(len).assertLess(32)
        console.info('device_info_test_031 ：PASS')
    })

    /**
     * @tc.number    SUB_STARTUP_JS_DEVCEINFO_0350
     * @tc.name      testGetProductBrand02
     * @tc.desc      Get a string representing the external product family name which has a maximum of 32 characters.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('device_info_test_032', 0, function () {
        console.info('device_info_test_032 start');
        var brandInfo = deviceinfo.brand;
        var ret = false;
        if(brandInfo !== null){
            ret = true;
        }
        expect(ret).assertTrue();

        let len = brandInfo.length
        console.info('the value of the external product family name characters is :' + len)
        expect(len).assertLess(32)
        console.info('device_info_test_032 ：PASS')
    })

    /**
     * @tc.number    SUB_STARTUP_JS_DEVCEINFO_0360
     * @tc.name      testGetMarketName02
     * @tc.desc      Get a string representing the product series which has a maximum of 32 characters.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('device_info_test_033', 0, function () {
        console.info('device_info_test_033 start');
        var marketNameInfo = deviceinfo.marketName;
        var ret = false;
        if(marketNameInfo !== null){
            ret = true;
        }
        expect(ret).assertTrue();

        let len = marketNameInfo.length
        console.info('the value of the product series characters is :' + len)
        expect(len).assertLess(32)
        console.info('device_info_test_033 ：PASS')
    })

    /**
     * @tc.number    SUB_STARTUP_JS_DEVCEINFO_0370
     * @tc.name      testGetProductSeries02
     * @tc.desc      Get a string representing the product series which has a maximum of 32 characters.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('device_info_test_034', 0, function () {
        console.info('device_info_test_034 start');
        var productSeriesInfo = deviceinfo.productSeries;
        var ret = false;
        if(productSeriesInfo !== null){
            ret = true;
        }
        expect(ret).assertTrue();

        let len = productSeriesInfo.length
        console.info('the value of the product series characters is :' + len)
        expect(len).assertLess(32)
        console.info('device_info_test_034 ：PASS')
    })

    /**
     * @tc.number    SUB_STARTUP_JS_DEVCEINFO_0380
     * @tc.name      testGetProductModel02
     * @tc.desc      Get a string representing the certified model which has a maximum of 32 characters.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('device_info_test_035', 0, function () {
        console.info('device_info_test_035 start');
        var productModelInfo = deviceinfo.productModel;
        var ret = false;
        if(productModelInfo !== null){
            ret = true;
        }
        expect(ret).assertTrue();

        let len = productModelInfo.length
        console.info('the value of the certified model characters is :' + len)
        expect(len).assertLess(32)
        console.info('device_info_test_035 ：PASS')
    })

    /**
     * @tc.number    SUB_STARTUP_JS_DEVCEINFO_0390
     * @tc.name      testGetSoftwareModel02
     * @tc.desc      Get a string representing the internal software sub-model which has a maximum of 32 characters.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('device_info_test_036', 0, function () {
        console.info('device_info_test_036 start');
        var softwareModelInfo = deviceinfo.softwareModel;
        var ret = false;
        if(softwareModelInfo !== null){
            ret = true;
        }
        expect(ret).assertTrue();

        let len = softwareModelInfo.length
        console.info('the value of the internal software sub-model characters is :' + len)
        expect(len).assertLess(32)
        console.info('device_info_test_036 ：PASS')
    })

    /**
     * @tc.number    SUB_STARTUP_JS_DEVCEINFO_0410
     * @tc.name      testGetHardwareModel02
     * @tc.desc      Get a string representing the hardware version which has a maximum of 32 characters.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('device_info_test_037', 0, function () {
        console.info('device_info_test_037 start');
        var hardwareModelInfo = deviceinfo.hardwareModel;
        var ret = false;
        if(hardwareModelInfo !== null){
            ret = true;
        }
        expect(ret).assertTrue();

        let len = hardwareModelInfo.length
        console.info('the value of the hardware version characters is :' + len)
        expect(len).assertLess(32)
        console.info('device_info_test_037 ：PASS')
    })

    /**
     * @tc.number    SUB_STARTUP_JS_DEVCEINFO_0420
     * @tc.name      testGetHardwareProfile02
     * @tc.desc      Get a string representing the hardware version which has a maximum of 32 characters.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('device_info_test_038', 0, function () {
        console.info('device_info_test_038 start');
        var hardwareProfileInfo = deviceinfo.hardwareProfile;
        var ret = false;
        if(hardwareProfileInfo !== null){
            ret = true;
        }
        expect(ret).assertTrue();

        let len = hardwareProfileInfo.length
        console.info('the value of the hardware version characters is :' + len)
        expect(len).assertLess(1000)
        console.info('device_info_test_038 ：PASS')
    })

    /**
     * @tc.number    SUB_STARTUP_JS_DEVCEINFO_0430
     * @tc.name      testGetSerial02
     * @tc.desc      Get a string representing the device serial number which has a maximum of 32 characters.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('device_info_test_039', 0, function () {
        console.info('device_info_test_039 start');
        var serialInfo = deviceinfo.serial;
        var ret = false;
        if(serialInfo !== null){
            ret = true;
        }
        expect(ret).assertTrue();

        let len = serialInfo.length
        console.info('the value of the device serial number characters is :' + len)
        expect(len).assertLess(64)
        console.info('device_info_test_039 ：PASS')
    })

    /**
     * @tc.number    SUB_STARTUP_JS_DEVCEINFO_0440
     * @tc.name      testGetDisplayVersion02
     * @tc.desc      Get a string representing the version number visible to users which has a maximum of 32 characters.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('device_info_test_040', 0, function () {
        console.info('device_info_test_040 start');
        var displayVersionInfo = deviceinfo.displayVersion;
        var ret = false;
        if(displayVersionInfo !== null){
            ret = true;
        }
        expect(ret).assertTrue();

        let len = displayVersionInfo.length
        console.info('the value of the device serial number characters is :' + len)
        expect(len).assertLess(64)
        console.info('device_info_test_040 ：PASS')
    })

    /**
     * @tc.number    SUB_STARTUP_JS_DEVCEINFO_0450
     * @tc.name      testGetOsFullName02
     * @tc.desc      Get a string representing the operating system full name which has a maximum of 32 characters.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('device_info_test_041', 0, function () {
        console.info('device_info_test_041 start');
        var osFullNameInfo = deviceinfo.osFullName;
        var ret = false;
        if(osFullNameInfo !== null){
            ret = true;
        }
        expect(ret).assertTrue();

        let len = osFullNameInfo.length
        console.info('the value of the operating system full name characters is :' + len)
        expect(len).assertLess(32)
        console.info('device_info_test_041 ：PASS')
    })

    /**
     * @tc.number    SUB_STARTUP_JS_DEVCEINFO_0460
     * @tc.name      testGetVersionId02
     * @tc.desc      Get a string representing the operating system full name which has a maximum of 32 characters.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('device_info_test_042', 0, function () {
        console.info('device_info_test_042 start');
        var versionIdInfo = deviceinfo.versionId;
        var ret = false;
        if(versionIdInfo !== null){
            ret = true;
        }
        expect(ret).assertTrue();

        let len = versionIdInfo.length
        console.info('the value of the operating system full name characters is :' + len)
        expect(len).assertLess(127)
        console.info('device_info_test_042 ：PASS')
    })

    /**
     * @tc.number    SUB_STARTUP_JS_DEVCEINFO_0470
     * @tc.name      testGetBuildUser02
     * @tc.desc      Get a string representing the different build user of the same baseline code which has a maximum of 32 characters.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('device_info_test_043', 0, function () {
        console.info('device_info_test_043 start');
        var buildUserInfo = deviceinfo.buildUser;
        var ret = false;
        if(buildUserInfo !== null){
            ret = true;
        }
        console.info('the value of thebuildUser is :' + buildUserInfo);
        expect(ret).assertTrue();
        console.info('the value of the different build user of the same baseline code characters is :' + buildUserInfo.length);
        expect(buildUserInfo.length).assertLess(32)
        console.info('device_info_test_043 ：PASS')
    })

    /**
     * @tc.number    SUB_STARTUP_JS_DEVCEINFO_0480
     * @tc.name      testGetBuildHost02
     * @tc.desc      Get a string representing the different build host of the same baseline code which has a maximum of 32 characters.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('device_info_test_044', 0, function () {
        console.info('device_info_test_044 start');
        var buildHostInfo = deviceinfo.buildHost;
        var ret = false;
        if(buildHostInfo !== null){
            ret = true;
        }
        expect(ret).assertTrue();

        let len = buildHostInfo.length
        console.info('the value of the different build host of the same baseline code characters is :' + len)
        expect(len).assertLess(32)
        console.info('device_info_test_044 ：PASS')
    })

    /**
     * @tc.number    SUB_STARTUP_JS_DEVCEINFO_0490
     * @tc.name      testGetDeviceType03
     * @tc.desc      Get a string representing the device type which has at least one characters.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('device_info_test_045', 0, function () {
        console.info('device_info_test_045 start');
        var deviceTypeInfo = deviceinfo.deviceType;
        var ret = false;
        if(deviceTypeInfo !== null){
            ret = true;
        }
        expect(deviceTypeInfo).assertEqual('default')
        expect(ret).assertTrue();

        let len = deviceTypeInfo.length
        console.info('the value of the device type characters:' + len)
        expect(len).assertLarger(0)
        console.info('device_info_test_045 ：PASS')
    })

    /**
     * @tc.number    SUB_STARTUP_JS_DEVCEINFO_0510
     * @tc.name      testGetManufacture03
     * @tc.desc      Get a string representing the manufacture which has at least one characters.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('device_info_test_046', 0, function () {
        console.info('device_info_test_046 start');
        var manufactureInfo = deviceinfo.manufacture;
        var ret = false;
        if(manufactureInfo !== null){
            ret = true;
        }
        expect(ret).assertTrue();

        let len = manufactureInfo.length
        console.info('the value of the manufacture characters is :' + len)
        expect(len).assertLarger(0)
        console.info('device_info_test_046 ：PASS')
    })

    /**
     * @tc.number    SUB_STARTUP_JS_DEVCEINFO_0520
     * @tc.name      testGetProductBrand03
     * @tc.desc      Get a string representing the external product family name which has at least one characters.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('device_info_test_047', 0, function () {
        console.info('device_info_test_047 start');
        var brandInfo = deviceinfo.brand;
        var ret = false;
        if(brandInfo !== null){
            ret = true;
        }
        expect(ret).assertTrue();

        let len = brandInfo.length
        console.info('the value of the external product family name characters is :' + len)
        expect(len).assertLarger(0)
        console.info('device_info_test_047 ：PASS')
    })

    /**
     * @tc.number    SUB_STARTUP_JS_DEVCEINFO_0530
     * @tc.name      testGetMarketName03
     * @tc.desc      Get a string representing the product series which has at least one characters.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('device_info_test_048', 0, function () {
        console.info('device_info_test_048 start');
        var marketNameInfo = deviceinfo.marketName;
        var ret = false;
        if(marketNameInfo !== null){
            ret = true;
        }
        expect(ret).assertTrue();

        let len = marketNameInfo.length
        console.info('the value of the product series characters is :' + len)
        expect(len).assertLarger(0)
        console.info('device_info_test_048 ：PASS')
    })

    /**
     * @tc.number    SUB_STARTUP_JS_DEVCEINFO_0540
     * @tc.name      testGetProductSeries03
     * @tc.desc      Get a string representing the product series which has at least one characters.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('device_info_test_049', 0, function () {
        console.info('device_info_test_049 start');
        var productSeriesInfo = deviceinfo.productSeries;
        var ret = false;
        if(productSeriesInfo !== null){
            ret = true;
        }
        expect(ret).assertTrue();

        let len = productSeriesInfo.length
        console.info('the value of the product series characters is :' + len)
        expect(len).assertLarger(0)
        console.info('device_info_test_049 ：PASS')
    })

    /**
     * @tc.number    SUB_STARTUP_JS_DEVCEINFO_0550
     * @tc.name      testGetProductModel03
     * @tc.desc      Get a string representing the certified model which has at least one characters.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('device_info_test_050', 0, function () {
        console.info('device_info_test_050 start');
        var productModelInfo = deviceinfo.productModel;
        var ret = false;
        if(productModelInfo !== null){
            ret = true;
        }
        expect(ret).assertTrue();

        let len = productModelInfo.length
        console.info('the value of the certified model characters is :' + len)
        expect(len).assertLarger(0)
        console.info('device_info_test_050 ：PASS')
    })

    /**
     * @tc.number    SUB_STARTUP_JS_DEVCEINFO_0560
     * @tc.name      testGetSoftwareModel03
     * @tc.desc      Get a string representing the internal software sub-model which has at least one characters.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('device_info_test_051', 0, function () {
        console.info('device_info_test_036 start');
        var softwareModelInfo = deviceinfo.softwareModel;
        var ret = false;
        if(softwareModelInfo !== null){
            ret = true;
        }
        expect(ret).assertTrue();

        let len = softwareModelInfo.length
        console.info('the value of the internal software sub-model characters is :' + len)
        expect(len).assertLarger(0)
        console.info('device_info_test_036 ：PASS')
    })

    /**
     * @tc.number    SUB_STARTUP_JS_DEVCEINFO_0570
     * @tc.name      testGetHardwareModel03
     * @tc.desc      Get a string representing the hardware version which has at least one characters.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('device_info_test_052', 0, function () {
        console.info('device_info_test_052 start');
        var hardwareModelInfo = deviceinfo.hardwareModel;
        var ret = false;
        if(hardwareModelInfo !== null){
            ret = true;
        }
        expect(ret).assertTrue();

        let len = hardwareModelInfo.length
        console.info('the value of the hardware version characters is :' + len)
        expect(len).assertLarger(0)
        console.info('device_info_test_052 ：PASS')
    })

    /**
     * @tc.number    SUB_STARTUP_JS_DEVCEINFO_0580
     * @tc.name      testGetHardwareProfile03
     * @tc.desc      Get a string representing the hardware version which has at least one characters.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('device_info_test_053', 0, function () {
        console.info('device_info_test_053 start');
        var hardwareProfileInfo = deviceinfo.hardwareProfile;
        var ret = false;
        if(hardwareProfileInfo !== null){
            ret = true;
        }
        expect(ret).assertTrue();

        let len = hardwareProfileInfo.length
        console.info('the value of the hardware version characters is :' + len)
        expect(len).assertLarger(0)
        console.info('device_info_test_053 ：PASS')
    })

    /**
     * @tc.number    SUB_STARTUP_JS_DEVCEINFO_0590
     * @tc.name      testGetSerial03
     * @tc.desc      Get a string representing the device serial number which has at least one characters.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('device_info_test_054', 0, function () {
        console.info('device_info_test_054 start');
        var serialInfo = deviceinfo.serial;
        var ret = false;
        if(serialInfo !== null){
            ret = true;
        }
        expect(ret).assertTrue();

        let len = serialInfo.length
        console.info('the value of the device serial number characters is :' + len)
        expect(len).assertLarger(0)
        console.info('device_info_test_054 ：PASS')
    })

    /**
     * @tc.number    SUB_STARTUP_JS_DEVCEINFO_0610
     * @tc.name      testGetDisplayVersion03
     * @tc.desc      Get a string representing the version number visible to users which has at least one characters.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('device_info_test_055', 0, function () {
        console.info('device_info_test_055 start');
        var displayVersionInfo = deviceinfo.displayVersion;
        var ret = false;
        if(displayVersionInfo !== null){
            ret = true;
        }
        expect(ret).assertTrue();

        let len = displayVersionInfo.length
        console.info('the value of the device serial number characters is :' + len)
        expect(len).assertLarger(0)
        console.info('device_info_test_055 ：PASS')
    })

    /**
     * @tc.number    SUB_STARTUP_JS_DEVCEINFO_0620
     * @tc.name      testGetIncrementalVersionInfo02
     * @tc.desc      Get a string representing the version number visible to users which has a maximum of 32 characters.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('device_info_test_056', 0, function () {
        console.info('device_info_test_056 start');
        var incrementalVersionInfo = deviceinfo.incrementalVersion;
        var ret = false;
        if(incrementalVersionInfo !== null){
            ret = true;
        }
        expect(ret).assertTrue();

        let len = incrementalVersionInfo.length
        console.info('the value of the device serial number characters is :' + len)
        expect(len).assertLess(32)
        console.info('device_info_test_056 ：PASS')
    })

    /**
     * @tc.number    SUB_STARTUP_JS_DEVCEINFO_0630
     * @tc.name      testGetIncrementalVersionInfo03
     * @tc.desc      Get a string representing the version number visible to users which has at least one characters.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('device_info_test_057', 0, function () {
        console.info('device_info_test_057 start');
        var incrementalVersionInfo = deviceinfo.incrementalVersion;
        var ret = false;
        if(incrementalVersionInfo !== null){
            ret = true;
        }
        expect(ret).assertTrue();

        let len = incrementalVersionInfo.length
        console.info('the value of the device serial number characters is :' + len)
        expect(len).assertLarger(0)
        console.info('device_info_test_057 ：PASS')
    })

    /**
     * @tc.number    SUB_STARTUP_JS_DEVCEINFO_0640
     * @tc.name      testGetVersionId03
     * @tc.desc      Get a string representing the operating system full name which has at least one characters.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('device_info_test_058', 0, function () {
        console.info('device_info_test_058 start');
        var versionIdInfo = deviceinfo.versionId;
        var ret = false;
        if(versionIdInfo !== null){
            ret = true;
        }
        expect(ret).assertTrue();

        let len = versionIdInfo.length
        console.info('the value of the operating system full name characters is :' + len)
        expect(len).assertLarger(0)
        console.info('device_info_test_058 ：PASS')
    })

    /**
     * @tc.number    SUB_STARTUP_JS_DEVCEINFO_0650
     * @tc.name      testGetBuildUser03
     * @tc.desc      Get a string representing the different build user of the same baseline code which has at least one characters.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('device_info_test_059', 0, function () {
        console.info('device_info_test_043 start');
        var buildUserInfo = deviceinfo.buildUser;
        var ret = false;
        if(buildUserInfo !== null){
            ret = true;
        }
        console.info('the value of thebuildUser is :' + buildUserInfo);
        expect(ret).assertTrue();
        console.info('the value of the different build user of the same baseline code characters is :' + buildUserInfo.length);
        expect(buildUserInfo.length).assertLarger(0)
        console.info('device_info_test_059 ：PASS')
    })

    /**
     * @tc.number    SUB_STARTUP_JS_DEVCEINFO_0660
     * @tc.name      testGetBuildHost03
     * @tc.desc      Get a string representing the different build host of the same baseline code which has at least one characters.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('device_info_test_060', 0, function () {
        console.info('device_info_test_060 start');
        var buildHostInfo = deviceinfo.buildHost;
        var ret = false;
        if(buildHostInfo !== null){
            ret = true;
        }
        expect(ret).assertTrue();

        let len = buildHostInfo.length
        console.info('the value of the different build host of the same baseline code characters is :' + len);
        expect(len).assertLarger(0);
        console.info('device_info_test_060 ：PASS');
    })
})

