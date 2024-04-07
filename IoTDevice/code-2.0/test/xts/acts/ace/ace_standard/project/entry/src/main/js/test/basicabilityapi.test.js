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

import configuration from '@system.configuration';
import prompt from '@system.prompt';
import router from '@system.router';

describe('basicabilityapi', function() {
    let testResult;
    let testResultFail;
    let test;
    beforeAll(function() {
        testResult = true;
        testResultFail = false;
        test="success"
    });
    beforeEach(function() {});
    afterEach(function() {});

    /**
     * @tc.number    SUB_ACE_BASICABILITY_JS_API_0100
     * @tc.name      testClearInterval
     * @tc.desc      Cancel the repetitive timing tasks previously set by setInterval.
     */
    it('testClearInterval', 0, function() {
        console.info('testClearInterval START');
        let intervalID = setInterval(function() {
            console.info('TEST do very 1s.');
        }, 1000);
        clearInterval(intervalID);
        expect(test).assertEqual('success');
        console.info('[clearInterval] success');
        console.info('testClearInterval END');
    });

    /**
     * @tc.number    SUB_ACE_BASICABILITY_JS_API_0200
     * @tc.name      testConsole
     * @tc.desc      Print a text message.
     */
    it('testConsole', 0, function() {
        console.info('testConsole START');
        const versionCode = 1.1;
        console.info('[console.info] versionCode: ' + versionCode);
        console.debug('[console.debug] versionCode: ' + versionCode);
        console.log('[console.log] versionCode: ' + versionCode);
        console.warn('[console.warn] versionCode: ' + versionCode);
        console.error('[console.error] versionCode: ' + versionCode);
        expect(test).assertEqual('success');
        console.info('testConsole END');
    });

    /**
     * @tc.number    SUB_ACE_BASICABILITY_JS_API_0300
     * @tc.name      testRouterPush
     * @tc.desc      Go to the specified page of the application.
     */
    it('testRouterPush', 0, function() {
        router.push({
            uri:'pages/routePush/index',
            params: {
                myData:'this is params data'
            }
        });
        setTimeout(() => {
            let pages = router.getState();
            console.info("[router.clear] getState"+JSON.stringify(pages));
        }, 500);
        expect(test).assertEqual('success');
    });

    /**
     * @tc.number    SUB_ACE_BASICABILITY_JS_API_0400
     * @tc.name      testRouterReplace
     * @tc.desc      Replace the current page with a page in the application, and destroy the replaced page.
     */
    it('testRouterReplace', 0, function() {
        console.info('testRouterReplace START');
        router.replace({
            uri: 'pages/routerReplace/index',
            params: {
                data1: 'message',
            }
        });
        expect(test).assertEqual('success');
        console.info("[router.replace] success");
        console.info('testRouterReplace END');
    });

    /**
     * @tc.number    SUB_ACE_BASICABILITY_JS_API_0500
     * @tc.name      testRouterBack
     * @tc.desc      Return to the previous page or the specified page.
     */
    it('testRouterBack', 0, function() {
        console.info('testRouterBack START');
        router.push({
            uri: 'pages/detail/detail'
        });
        router.back({
            path: 'pages/mediaquery/mediaquery'
        });
        expect(test).assertEqual('success');
        console.info('[router.back] success');
        console.info('testRouterBack END');
    });

    /**
     * @tc.number    SUB_ACE_BASICABILITY_JS_API_0600
     * @tc.name      testRouterClear
     * @tc.desc      Clear all historical pages in the page stack, and only keep the current page as the top page.
     */
    it('testRouterClear', 0, function() {
        console.info('testRouterClear START');
        new Promise(function(resolve, reject) {
            router.push({
                uri:'pages/routePush/index',
                params:{
                    myData:'message'
                }
            });
            setTimeout(()=>{
                console.info("[router.clear] getLength: " + router.getLength());
            }, 2000);
            resolve();
        }).then(function() {
            expect(testResult).toBeTrue();
        }, function() {
            console.log('[router.clear] fail');
            expect(testResultFail).toBeTrue();
        });
        console.info('testRouterClear END');
    });

    /**
     * @tc.number    SUB_ACE_BASICABILITY_JS_API_0700
     * @tc.name      testRouterLength
     * @tc.desc      Get the number of pages currently in the page stack.
     */
    it('testRouterLength', 0, function() {
        router.push({
            uri: 'pages/dialog/dialog',
            params: {
                data1: 'message',
                data2: {
                    data3: [123, 456, 789],
                    data4: {
                        data5: 'message'
                    }
                }
            }
        });
        console.info('testRouterLength START');
        let size = router.getLength();
        console.info('[router.getLength] pages stack size = ' + size);
        expect(size).assertEqual('1');
        console.info('testRouterLength END');
    });

    /**
     * @tc.number    SUB_ACE_BASICABILITY_JS_API_0800
     * @tc.name      testRouterGetState
     * @tc.desc      Get the status information of the current page.
     */
    it('testRouterGetState', 0, function() {
        console.info('testRouterGetState START');
        let page = router.getState();
        console.info('[router.getState] index: ' + page.index);
        console.info('[router.getState] name: ' + page.name);
        console.info('[router.getState] path: ' + page.path);
        expect(page.index).assertEqual('1');
        expect(page.name).assertEqual('index');
        expect(page.path).assertEqual('pages/index/');
        console.info('testRouterGetState END');
    });

    /**
     * @tc.number    SUB_ACE_BASICABILITY_JS_API_0900
     * @tc.name      testPromptShowToast
     * @tc.desc      Show text pop-up window.
     */
    it('testPromptShowToast', 0, function() {
        console.info('testPromptShowToast START');
        const delay = 5000;
        prompt.showToast({
            message:'message',
            duration:delay,
        });
        expect(test).assertEqual('success');
        console.info('[prompt.showToast] success');
        console.info('testPromptShowToast END');
    });

    /**
     * @tc.number    SUB_ACE_BASICABILITY_JS_API_1000
     * @tc.name      testPromptDialog
     * @tc.desc      Display the dialog box in the page.
     */
    it('testPromptDialog', 0, function() {
        console.info('testPromptDialog START')
        prompt.showDialog({
            title: 'dialog showDialog test',
            message: 'message of dialog',
            buttons: [
                {
                    text: 'OK',
                    color: '#0000ff',
                    index: 0
                }
            ],
            success: function(ret) {
                console.info("[prompt.showDialog] ret.index " + ret.index);
                expect(testResult).toBeTrue();
            },
            cancel: function() {
                console.log('[prompt.showDialog] dialog cancel callback');
                expect(testResultFail).toBeTrue();
            },
            complete: function() {
                console.log('[prompt.showDialog] complete');
            }
        });
        console.info('testPromptDialog END');
    });

    /**
     * @tc.number    SUB_ACE_BASICABILITY_JS_API_1100
     * @tc.name      testConfigurationGetLocale
     * @tc.desc      Get the current language and region of the app. Synchronize with the language and region.
     */
    it('testConfigurationGetLocale', 0, function() {
        console.info('testConfigurationGetLocale START');
        const localeInfo = configuration.getLocale();
        console.info("[configuration.getLocale] language: " + localeInfo.language);
        console.info("[configuration.getLocale] countryOrRegion: " + localeInfo.countryOrRegion);
        console.info("[configuration.getLocale] dir: " + localeInfo.dir);
        expect(localeInfo.language).assertEqual('zh');
        expect(localeInfo.countryOrRegion).assertEqual('CN');
        expect(localeInfo.dir).assertEqual('ltr');
        console.info('testConfigurationGetLocale END');
    });

    /**
     * @tc.number    SUB_ACE_BASICABILITY_JS_API_1200
     * @tc.name      testSetTimeout
     * @tc.desc      Set up a timer that executes a function or a specified piece of code after the timer expires.
     */
    it('testSetTimeout', 0, function() {
        console.info('testSetTimeout START');
        let start_time = new Date().getTime();
        const delay = 200;
        console.info("[settimeout] start_time: " + start_time);
        setTimeout(function(v1,v2) {
            let end_time = new Date().getTime();
            console.info('[settimeout] delay: ' + (end_time - start_time) / 1000);
            console.info('[settimeout] v1: ' + v1);
            console.info('[settimeout] v2: ' + v2);
            expect(testResult).toBeTrue();
        }, delay, 'test', 'message');
        console.info('testSetTimeout END');
    });

    /**
     * @tc.number    SUB_ACE_BASICABILITY_JS_API_1300
     * @tc.name      testClearTimeout
     * @tc.desc      The timer previously established by calling setTimeout() is cancelled.
     */
    it('testClearTimeout', 0, function() {
        console.info('testClearTimeout START');
        let timeoutID = setTimeout(function() {
            console.info('delay 1s');
        }, 1000);
        clearTimeout(timeoutID);
        expect(test).assertEqual('success');
        console.info("[clearTimeout] success");
        console.info('testClearTimeout END');
    });

    /**
     * @tc.number    SUB_ACE_BASICABILITY_JS_API_1400
     * @tc.name      testSetInterval
     * @tc.desc      Call a function or execute a code segment repeatedly, with a fixed time delay between each call.
     */
    it('testSetInterval', 0, function() {
        console.info('testSetInterval START');
        let intervalID = setInterval(function() {
            console.log('do very 1s.');
            expect(testResult).toBeTrue();
        }, 1000);
        console.info("[setInterval] intervalID: " + intervalID);
        console.info('testSetInterval END');
    });
});