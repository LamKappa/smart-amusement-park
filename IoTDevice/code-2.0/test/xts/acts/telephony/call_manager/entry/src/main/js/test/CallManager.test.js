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

import call from '@ohos.telephony_call'
import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index'

describe('CallManagementTest', function () {
    const TEST_PHONY_NUMBER = "13788888888";
	const TIMES_1000 = 1000;
    const MSEC_500 = 500;

    /*
     * @tc.number  Telephony_CallManager_dial_Async_0100
     * @tc.name    Call dial() by way of callback when the phone number is a normal number
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_dial_Async_0100', 0, async function (done) {
        call.dial( TEST_PHONY_NUMBER, (err, value) => {
            if (err) {
                expect().assertFail();
                return;
            }
            expect(value).assertTrue();
         })
        done();
    })

    /*
     * @tc.number  Telephony_CallManager_dial_Async_0200
     * @tc.name    Call dial() by way of callback when the phone number is ''
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_dial_Async_0200', 0, async function (done) {
        call.dial( '', (err, value) => {
            if (err) {
                expect().assertFail();
                return;
            }
            expect(value).assertFalse();
        })
        done();
    })

    /*
     * @tc.number  Telephony_CallManager_dial_Async_0300
     * @tc.name    Call dial() by way of callback when the phone number is '12345678901234567890012345678901'
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_dial_Async_0300', 0, async function (done) {
        call.dial( '12345678901234567890012345678901', (err, value) => {
            if (err) {
                expect().assertFail();
                return;
            }
            expect(value).assertFalse();
        })
        done();
    })

    /*
     * @tc.number  Telephony_CallManager_dial_Async_0400
     * @tc.name    Call dial() by way of callback when the phone number is '13788888888,1234567890123456789123'
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_dial_Async_0400', 0, async function (done) {
        call.dial( '13788888888,1234567890123456789123', (err, value) => {
            if (err) {
                expect().assertFail();
                return;
            }
            expect(value).assertTrue();
        })
        done();
    })

    /*
     * @tc.number  Telephony_CallManager_dial_Async_0500
     * @tc.name    Call dial() by way of callback when the phone number is 'abcde123456'
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_dial_Async_0500', 0, async function (done) {
        call.dial( 'abcde123456', (err, value) => {
            if (err) {
                expect().assertFail();
                return;
            }
            expect(value).assertFalse();
        })
        done();
    })

    /*
     * @tc.number  Telephony_CallManager_dial_Async_0600
     * @tc.name    Call dial() by way of callback, phone number is 'abcde123456'
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_dial_Async_0600', 0, async function (done) {
        for (let index = 0; index < TIMES_1000; index++) {
            call.dial( 'abcde123456', (err, value) => {
                if (err) {
                    expect().assertFail();
                    return;
                }
                expect(value).assertFalse();
            })
        }
        done();
    })

    /*
     * @tc.number  Telephony_CallManager_dial_Async_0700
     * @tc.name    Call dail() by way of callback 1000 times, phone number is 'abcde123456'
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_dial_Async_0700', 0, async function (done) {
        const startTime = (new Date).getTime();
        for (let index = 0; index < TIMES_1000; index++) {
            call.dial( 'abcde123456', (err, value) => {
                if (err) {
                    expect().assertFail();
                    return;
                }
                expect(value).assertFalse();
            })
        }
        const endTime = (new Date).getTime();
        expect(endTime - startTime).assertLess(MSEC_500);
        done();
    })

    /*
     * @tc.number  Telephony_CallManager_dial_Async_0800
     * @tc.name    Call dail() by way of callback, phoneNumber is a normal number, options is {extras: false}
     *             and the optional parameter options is {extras: false}
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_dial_Async_0800', 0, async function (done) {
        call.dial( TEST_PHONY_NUMBER, {extras: false}, (err, value) => {
            if (err) {
                expect().assertFail();
                return;
            }
            expect(value.result).assertEqual(0);
        })
        done();
    })

    /*
     * @tc.number  Telephony_CallManager_dial_Async_0900
     * @tc.name    Call dial() by way of callback, phoneNumber is a normal number, options is ''
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_dial_Async_0900', 0, async function (done) {
        call.dial( '', {extras: false}, (err, value) => {
            if (err) {
                expect().assertFail();
                return;
            }
            expect(value.result).assertEqual(0);
        })
        done();
    })

    /*
     * @tc.number  Telephony_CallManager_dial_Async_1000
     * @tc.name    Call dial() by way of callback, phoneNumber is 'abcde123456', options is {extras: false}
     *             parameter options is {extras: false}
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_dial_Async_1000', 0, async function (done) {
        for (let index = 0; index < TIMES_1000; index++) {
            call.dial( 'abcde123456', {extras: false}, (err, value) => {
                if (err) {
                    expect().assertFail();
                    return;
                }
                expect(value.result).assertEqual(call.CALL_MANAGER_PHONENUM_NULL);
            })
        }
        done();
    })

    /*
     * @tc.number  Telephony_CallManager_dial_Async_1100
     * @tc.name    Call dial() by way of callback check the time, phoneNumber is '~!@#$%^&*123', options is {extras: false}
     *             parameter options is empty
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_dial_Async_1100', 0, async function (done) {
        const startTime = (new Date).getTime();
        for (let index = 0; index < TIMES_1000; index++) {
            call.dial( '~!@#$%^&*123', {extras: false}, (err, value) => {
                if (err) {
                    expect().assertFail();
                    return;
                }
                expect(value).assertFalse();
            })
        }
        const endTime = (new Date).getTime();
        expect(endTime - startTime).assertLess(MSEC_500);
        done();
    })

    /*
     * @tc.number  Telephony_CallManager_dial_Promise_0100
     * @tc.name    Call dail(), phoneNumber is a normal number, options is {extras: false}
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_dial_Promise_0100', 0, async function (done) {
        call.dial(TEST_PHONY_NUMBER, {extras: false}).then((data) => {
            expect(data).assertTrue();
        }).catch((err) => {
            expect().assertFail();
        })
        done();
    })

    /*
     * @tc.number  Telephony_CallManager_dial_Promise_0200
     * @tc.name    Call dail(), phoneNumber is '', options is {extras: false}
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_dial_Promise_0100', 0, async function (done) {
        call.dial('', {extras: false}).then((data) => {
            expect(data).assertFalse();
        }).catch((err) => {
            expect().assertFail();
        })
        done();
    })

    /*
     * @tc.number  Telephony_CallManager_dial_Promise_0300
     * @tc.name    Call dail(), phoneNumber is 'abcde123456', options is {extras: false}
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_dial_Promise_0300', 0, async function (done) {
        call.dial('abcde123456', {extras: false}).then((data) => {
            expect(data).assertFalse();
        }).catch((err) => {
            expect().assertFail();
        })
        done();
    })

    /*
     * @tc.number  Telephony_CallManager_dial_Promise_0400
     * @tc.name    Call dail() 1000 times, phoneNumber is 'abcde123456', options is {extras: false}
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_dial_Promise_0400', 0, async function (done) {
        for (let index = 0; index < TIMES_1000; index++) {
            call.dial('abcde123456', {extras: false}).then((data) => {
                expect(data).assertNull();
            }).catch((err) => {
                expect().assertFail();
            })
        }
        done();
    })

    /*
     * @tc.number  Telephony_CallManager_dial_Promise_0500
     * @tc.name    Call dail() 1000 times check the time, phoneNumber is 'abcde123456', options is {extras: false}
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_dial_Promise_0400', 0, async function (done) {
        const startTime = (new Date).getTime();
        for (let index = 0; index < TIMES_1000; index++) {
            call.dial('abcde123456', {extras: false}).then((data) => {
                expect(data).assertNull();
            }).catch((err) => {
                expect().assertFail();
            })
        }
        const endTime = (new Date).getTime();
        expect(endTime - startTime).assertLess(MSEC_500);
        done();
    })

    /*
     * @tc.number  Telephony_CallManager_dial_Promise_0600
     * @tc.name    Call dail(), phoneNumber is a normal number
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_dial_Promise_0100', 0, async function (done) {
        call.dial(TEST_PHONY_NUMBER).then((data) => {
            expect(data).assertTrue();
        }).catch((err) => {
            expect().assertFail();
        })
        done();
    })

    /*
     * @tc.number  Telephony_CallManager_getCallState_Async_0100
     * @tc.name    Call getCallState() by way of callback when idle
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_getCallState_Async_0100', 0, async function (done) {
        call.getCallState((err, data) => {
            if (err) {
                expect().assertFail();
                return;
            }
            expect(value).assertEqual(call.CALL_STATE_IDLE);
        })
        done();
    })

    /*
     * @tc.number  Telephony_CallManager_getCallState_Async_0200
     * @tc.name    Call getCallState() by way of callback when dialing
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_getCallState_Async_0100', 0, async function (done) {
        call.dial(TEST_PHONY_NUMBER, (err, value) => {
            if (err) {
                expect().assertFail();
                return;
            }
            call.getCallState((err, value) => {
                if (err) {
                    expect().assertFail();
                    return;
                }
                expect(value).assertEqual(call.CALL_STATE_OFFHOOK);
            });
        });
        done();
    })

    /*
     * @tc.number  Telephony_CallManager_getCallState_Async_0300
     * @tc.name    Call getCallState() 1000 times by way of callback when idle
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_getCallState_Async_0300', 0, async function (done) {
        for (let index = 0; index < TIMES_1000; index++) { //Get call status 1000 calls in a loop
            call.getCallState((err, data) => {
                if (err) {
                    expect().assertFail();
                    return;
                }
                expect(value).assertEqual(call.CALL_STATE_IDLE);
            })
        }
        done();
    })

    /*
     * @tc.number  Telephony_CallManager_getCallState_Async_0400
     * @tc.name    Call getCallState() 1000 times by way of callback when idle, to check the time
     *             less than 500000us
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_getCallState_Async_0400', 0, async function (done) {
        const startTime = (new Date).getTime();
        for (let index = 0; index < TIMES_1000; index++) {
            call.getCallState((err, value) => {
                if (err) {
                    expect().assertFail();
                    return;
                }
                expect(value).assertEqual(call.CALL_STATE_IDLE);
            })
        }
        const endTime = (new Date).getTime();
        expect(endTime - startTime).assertLess(MSEC_500);
        done();
    })

    /*
     * @tc.number  Telephony_CallManager_getCallState_Promise_0100
     * @tc.name    Call getCallState() when idle
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_getCallState_Promise_0100', 0, async function (done) {
        call.getCallState().then((data) => {
            expect(data).assertEqual(call.CALL_STATE_IDLE);
        }).catch((err) => {
            expect().assertFail();
        });
        done();
    })

    /*
     * @tc.number  Telephony_CallManager_getCallState_Promise_0200
     * @tc.name    Call getCallState() when dialing
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_getCallState_Promise_0200', 0, async function (done) {
        call.dial(TEST_PHONY_NUMBER, (err, value) => {
            if (err) {
                expect().assertFail();
                return;
            }
            call.getCallState().then((data) => {
                expect(data).assertEqual(call.CALL_STATE_OFFHOOK);
            }).catch((err) => {
                expect().assertFail();
            });
        });
        done();
    })

    /*
     * @tc.number  Telephony_CallManager_getCallState_Promise_0300'
     * @tc.name    Call getCallState() 1000 times when idle
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_getCallState_Promise_0300', 0, async function (done) {
        for (let index = 0; index < TIMES_1000; index++) {
            call.getCallState().then((data) => {
                expect(data).assertEqual(call.CALL_STATE_IDLE);
            }).catch((err) => {
                expect().assertFail();
            });
        }
        done();
    })

    /*
     * @tc.number  Telephony_CallManager_getCallState_Promise_0400'
     * @tc.name    Call getCallState() 1000 times when idle, to check the time
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_getCallState_Promise_0400', 0, async function (done) {
        const startTime = (new Date).getTime();
        for (let index = 0; index < TIMES_1000; index++) {
            call.getCallState().then((data) => {
                expect(data).assertEqual(call.CALL_STATE_IDLE);
            }).catch((err) => {
                expect().assertFail();
                return;
            });
        }
        const endTime = (new Date).getTime();
        expect(endTime - startTime).assertLess(MSEC_500);
        done();
    })
})
