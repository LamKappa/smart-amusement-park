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

import radio from '@ohos.telephony_radio'
import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index'

describe('NetworkSearchTest', function () {
    const STLO_1 = 1;
    const TEST_RUN_TIME = 1000;
    const MSEC_500 = 500;
    const SIGNAL_STRENGTH_INVALID = 0;
    const SIGNAL_STRENGTH_HIGHEST = 4;

    /*
    * @tc.number  Telephony_NetworkSearch_getNetworkState_Async_0100
    * @tc.name    Test the getNetworkState() query function and return the default card 1 network registration status
    * @tc.desc    Function test
    */
    it('Telephony_NetworkSearch_getNetworkState_Async_0100', 0, async function (done) {
        radio.getNetworkState((err, data) => {
            if (err) {
                expect().assertFail();
                return;
            }
            expect(data != null && data != undefined).assertTrue();
            expect( data.longOperatorName != "" &&
                    data.longOperatorName != null &&
                    data.longOperatorName != undefined).assertTrue();
            expect( data.shortOperatorName != "" &&
                    data.shortOperatorName != null &&
                    data.shortOperatorName != undefined).assertTrue();
            expect( data.plmnNumeric != "" &&
                    data.plmnNumeric != null &&
                    data.plmnNumeric != undefined).assertTrue();
        })
        done()
    })

    /*
    * @tc.number  Telephony_NetworkSearch_getNetworkState_Async_0200
    * @tc.name    The slotId parameter input is 1, test the getNetworkState() query function,
    *             and return the network registration status of card 1
    * @tc.desc    Function test
    */
    it('Telephony_NetworkSearch_getNetworkState_Async_0200', 0, async function (done) {
        radio.getNetworkState(STLO_1, (err, data) => {
            if (err) {
                expect().assertFail();
                return;
            }
            expect(data != null && data != undefined).assertTrue();
            expect( data.longOperatorName != "" &&
                    data.longOperatorName != null &&
                    data.longOperatorName != undefined).assertTrue();
            expect( data.shortOperatorName != "" &&
                    data.shortOperatorName != null &&
                    data.shortOperatorName != undefined).assertTrue();
            expect( data.plmnNumeric != "" &&
                    data.plmnNumeric != null &&
                    data.plmnNumeric != undefined).assertTrue();
        })
        done()
    })

    /*
    * @tc.number  Telephony_NetworkSearch_getNetworkState_Async_0300
    * @tc.name    Test getNetworkState() The query function is executed 1000 times, and the network registration status
    *             of the default card 1 can be successfully returned each time
    * @tc.desc    Reliability test
    */
    it('Telephony_NetworkSearch_getNetworkState_Async_0300', 0, async function (done) {
        for (let index = 0; index < TEST_RUN_TIME; index++) {
            radio.getNetworkState((err, data) => {
                if (err) {
                    expect().assertFail();
                    return;
                }
                expect(data != null && data != undefined).assertTrue();
                expect( data.longOperatorName != "" &&
                        data.longOperatorName != null &&
                        data.longOperatorName != undefined).assertTrue();
                expect( data.shortOperatorName != "" &&
                        data.shortOperatorName != null &&
                        data.shortOperatorName != undefined).assertTrue();
                expect( data.plmnNumeric != "" &&
                        data.plmnNumeric != null &&
                        data.plmnNumeric != undefined).assertTrue();
            })
        }
        done()
    })

    /*
    * @tc.number  Telephony_NetworkSearch_getNetworkState_Async_0400
    * @tc.name    Test getNetworkState() query function is executed 1000 times,
    *             and the output delay is less than 500000us
    * @tc.desc    Performance test
    */
    it('Telephony_NetworkSearch_getNetworkState_Async_0400', 0, async function (done) {
        const startTime = (new Date).getTime();
        for (let index = 0; index < TEST_RUN_TIME; index++) {
            radio.getNetworkState((err, data) => {
                if (err) {
                    expect().assertFail();
                    return;
                }
            });
        }
        const endTime = (new Date).getTime();
        expect(endTime - startTime).assertLess(MSEC_500);
        done()
    })

    /*
    * @tc.number  Telephony_NetworkSearch_getNetworkState_Async_0500
    * @tc.name    The slotId parameter input is 1, the test getNetworkState() query function is executed 1000 times,
    *             and the network registration status can be returned every time
    * @tc.desc    Reliability test
    */
    it('Telephony_NetworkSearch_getNetworkState_Async_0500', 0, async function (done) {
        for (let index = 0; index < TEST_RUN_TIME; index++) {
            radio.getNetworkState(STLO_1, (err, data) => {
                if (err) {
                    expect().assertFail();
                    return;
                }
                expect(data != null && data != undefined).assertTrue();
            })
        }
        done()
    })

    /*
    * @tc.number  Telephony_NetworkSearch_getNetworkState_Async_0600
    * @tc.name    The slotId parameter input is 1, the test getNetworkState() query function is executed 1000 times,
    *             and the output delay is less than 500000us
    * @tc.desc    Performance test
    */
    it('Telephony_NetworkSearch_getNetworkState_Async_0600', 0, async function (done) {
        const startTime = (new Date).getTime();
        for (let index = 0; index < TEST_RUN_TIME; index++) {
            radio.getNetworkState(STLO_1, (err, data) => {
                if (err) {
                    expect(data != null && data != undefined).assertTrue();
                    expect().assertFail();
                    return;
                }
            })
        }
        const endTime = (new Date).getTime();
        expect(endTime - startTime).assertLess(MSEC_500);
        done()
    })

    /*
    * @tc.number  Telephony_NetworkSearch_getRadioTech_Async_0100
    * @tc.name    SlotId parameter input is 1, test getRadioTech() query function, return PS, CS network mode
    * @tc.desc    Function test
    */
    it('Telephony_NetworkSearch_getRadioTech_Async_0100', 0, async function (done) {
        radio.getRadioTech(STLO_1, (err, {psRadioTech, csRadioTech}) => {
            if (err) {
                expect().assertFail();
                return;
            }
            expect(psRadioTech != null && psRadioTech != undefined).assertTrue();
            expect(csRadioTech!= null && csRadioTech!= undefined).assertTrue();
            expect()
        })
        done()
    })

    /*
    * @tc.number  Telephony_NetworkSearch_getRadioTech_Async_0200
    * @tc.name    The slotId parameter input is 1, the test getRadioTech() query function is executed 1000 times,
    *             and the network mode of PS and CS is returned.
    * @tc.desc    Reliability test
    */
    it('Telephony_NetworkSearch_getRadioTech_Async_0200', 0, async function (done) {
        for (let index = 0; index < TEST_RUN_TIME; index++) {
            radio.getRadioTech(STLO_1, (err, {psRadioTech, csRadioTech}) => {
                if (err) {
                    expect().assertFail();
                    return;
                }
                expect(psRadioTech != null && psRadioTech != undefined).assertTrue();
                expect(csRadioTech!= null && csRadioTech!= undefined).assertTrue();
            })
        }
        done()
    })

    /*
    * @tc.number  Telephony_NetworkSearch_getRadioTech_Async_0300
    * @tc.name    The slotId parameter input is 1, the test getRadioTech() query function is executed 1000 times,
    *             and the output delay is less than 500000us
    * @tc.desc    Performance test
    */
    it('Telephony_NetworkSearch_getRadioTech_Async_0300', 0, async function (done) {
        const startTime = (new Date).getTime();
        for (let index = 0; index < TEST_RUN_TIME; index++) {
            radio.getRadioTech(STLO_1, (err, {psRadioTech, csRadioTech}) => {
                if (err) {
                    expect(psRadioTech != null && psRadioTech != undefined).assertTrue();
                    expect(csRadioTech!= null && csRadioTech!= undefined).assertTrue();
                    expect().assertFail();
                    return;
                }
            })
        }
        const endTime = (new Date).getTime();
        expect(endTime - startTime).assertLess(MSEC_500);
        done()
    })

    /*
    * @tc.number  Telephony_NetworkSearch_getSignalInformation_Async_0100
    * @tc.name    SlotId parameter input is 1, test getSignalInformation() query function,
    *             return signal strength list information
    * @tc.desc    Function test
    */
    it('Telephony_NetworkSearch_getSignalInformation_Async_0100', 0, async function (done) {
        radio.getSignalInformation(STLO_1, (err, data) => {
            if (err) { // 调用失败
                expect().assertFail();
                return;
            }
            expect(data != null && data != undefined).assertTrue();
        })
        done()
    })

    /*
    * @tc.number  Telephony_NetworkSearch_getSignalInformation_Async_0200
    * @tc.name    The slotId parameter input is 1, the test getSignalInformation() query function is executed 1000
    *             times, and the signal strength list information is returned each time
    * @tc.desc    Reliability test
    */
    it('Telephony_NetworkSearch_getSignalInformation_Async_0200', 0, async function (done) {
        for (let index = 0; index < TEST_RUN_TIME; index++) {
            radio.getSignalInformation(STLO_1, (err, data) => {
                if (err) { // 调用失败
                    expect().assertFail();
                    return;
                }
                expect(data != null && data != undefined).assertTrue();
            })
        }
        done()
    })

    /*
    * @tc.number  Telephony_NetworkSearch_getSignalInformation_Async_0300
    * @tc.name    The slotId parameter input is 1, the test getSignalInformation() query function is executed 1000
    *             times, and the output delay is less than 500000us
    * @tc.desc    Performance test
    */
    it('Telephony_NetworkSearch_getSignalInformation_Async_0300', 0, async function (done) {
        const startTime = (new Date).getTime();
        for (let index = 0; index < TEST_RUN_TIME; index++) {
            radio.getSignalInformation(STLO_1, (err, data) => {
                if (err) {
                    expect(data != null && data != undefined).assertTrue();
                    expect().assertFail();
                    return;
                }
            })
        }
        const endTime = (new Date).getTime();
        expect(endTime - startTime).assertLess(MSEC_500);
        done()
    })

    /*
    * @tc.number  Telephony_NetworkSearch_getNetworkState_Promise_0100
    * @tc.name    Test the getNetworkState() query function and return the default card 1 network registration status
    * @tc.desc    Function test
    */
    it('Telephony_NetworkSearch_getNetworkState_Promise_0100', 0, async function (done) {
        radio.getNetworkState().then((data) => {
               expect(data != null && data != undefined).assertTrue();
               expect( data.longOperatorName != "" &&
                       data.longOperatorName != null &&
                       data.longOperatorName != undefined).assertTrue();
               expect( data.shortOperatorName != "" &&
                       data.shortOperatorName != null &&
                       data.shortOperatorName != undefined).assertTrue();
               expect( data.plmnNumeric != "" &&
                       data.plmnNumeric != null &&
                       data.plmnNumeric != undefined).assertTrue();
        }).catch((err) => {
            console.log(err);
            expect().assertFail();
        })
        done()
    })

    /*
    * @tc.number  Telephony_NetworkSearch_getNetworkState_Promise_0200
    * @tc.name    The slotId parameter input is 1, test the getNetworkState() query function,
    *             and return the network registration status of card 1
    * @tc.desc    Function test
    */
    it('Telephony_NetworkSearch_getNetworkState_Promise_0200', 0, async function (done) {
        radio.getNetworkState(STLO_1).then((data) => {
            expect(data != null && data != undefined).assertTrue();
            expect( data.longOperatorName != "" &&
                    data.longOperatorName != null &&
                    data.longOperatorName != undefined).assertTrue();
            expect( data.shortOperatorName != "" &&
                    data.shortOperatorName != null &&
                    data.shortOperatorName != undefined).assertTrue();
            expect( data.plmnNumeric != "" &&
                    data.plmnNumeric != null &&
                    data.plmnNumeric != undefined).assertTrue();
        }).catch((err) => {
            console.log(err);
            expect().assertFail();
        });
        done()
    })

    /*
    * @tc.number  Telephony_NetworkSearch_getNetworkState_Promise_0300
    * @tc.name    Test getNetworkState() The query function is executed 1000 times, and the network registration status
    *             of the default card 1 can be successfully returned each time
    * @tc.desc    Reliability test
    */
    it('Telephony_NetworkSearch_getNetworkState_Promise_0300', 0, async function (done) {
        for (let index = 0; index < TEST_RUN_TIME; index++) {
            radio.getNetworkState().then((data) => {
                expect(data != null && data != undefined).assertTrue();
                expect(data.longOperatorName).assertTrue();
                expect(data.shortOperatorName).assertTrue();
                expect(data.plmnNumeric).assertTrue();
            }).catch((err) => {
                console.log(err);
                expect().assertFail();
            });
        }
        done()
    })

    /*
    * @tc.number  Telephony_NetworkSearch_getNetworkState_Promise_0400
    * @tc.name    Test getNetworkState() query function is executed 1000 times,
    *             and the output delay is less than 500000us
    * @tc.desc    Performance test
    */
    it('Telephony_NetworkSearch_getNetworkState_Promise_0400', 0, async function (done) {
        const startTime = (new Date).getTime();
        for (let index = 0; index < TEST_RUN_TIME; index++) {
            radio.getNetworkState().then((data) => {
                //expect do nothing for performence test
            }).catch((err) => {
                console.log(err);
                expect().assertFail();
            });
        }
        const endTime = (new Date).getTime();
        expect(endTime - startTime).assertLess(MSEC_500);
        done()
    })

    /*
    * @tc.number  Telephony_NetworkSearch_getNetworkState_Promise_0500
    * @tc.name    The slotId parameter input is 1, the test getNetworkState() query function is executed 1000 times,
    *             and the network registration status can be returned every time
    * @tc.desc    Reliability test
    */
    it('Telephony_NetworkSearch_getNetworkState_Promise_0500', 0, async function (done) {
        for (let index = 0; index < TEST_RUN_TIME; index++) {
            radio.getNetworkState(STLO_1).then((data) => {
                expect(data != null && data != undefined).assertTrue();
            }).catch((err) => {
                console.log(err);
                expect().assertFail();
                return;
            });
        }
        done()
    })

    /*
    * @tc.number  Telephony_NetworkSearch_getNetworkState_Promise_0600
    * @tc.name    The slotId parameter input is 1, the test getNetworkState() query function is executed 1000 times,
    *             and the output delay is less than 500000us
    * @tc.desc    Performance test
    */
    it('Telephony_NetworkSearch_getNetworkState_Promise_0600', 0, async function (done) {
        const startTime = (new Date).getTime();
        for (let index = 0; index < TEST_RUN_TIME; index++) {
            radio.getNetworkState(STLO_1).then((data) => {
                //expect do nothing for performence test
            }).catch((err) => {
                console.log(err);
                expect().assertFail();
                return;
            });
        }
        const endTime = (new Date).getTime();
        expect(endTime - startTime).assertLess(MSEC_500);
        done()
    })

    /*
    * @tc.number  Telephony_NetworkSearch_getRadioTech_Promise_0100
    * @tc.name    SlotId parameter input is 1, test getRadioTech() query function, return PS, CS network mode
    * @tc.desc    Function test
    */
    it('Telephony_NetworkSearch_getRadioTech_Promise_0100', 0, async function (done) {
        radio.getRadioTech(STLO_1).then(({psRadioTech, csRadioTech}) => {
            expect(psRadioTech != null && psRadioTech != undefined).assertTrue();
            expect(csRadioTech!= null && csRadioTech!= undefined).assertTrue();
        }).catch((err) => {
            console.log(err);
            expect().expect().assertFail();
            return;
        })
        done()
    })

    /*
    * @tc.number  Telephony_NetworkSearch_getRadioTech_Promise_0200
    * @tc.name    The slotId parameter input is 1, the test getRadioTech() query function is executed 1000 times,
    *             and the network mode of PS and CS is returned.
    * @tc.desc    Reliability test
    */
    it('Telephony_NetworkSearch_getRadioTech_Promise_0200', 0, async function (done) {
        for (let index = 0; index < TEST_RUN_TIME; index++) {
            radio.getRadioTech(STLO_1).then(({psRadioTech, csRadioTech}) => {
                expect(psRadioTech != null && psRadioTech != undefined).assertTrue();
                expect(csRadioTech!= null && csRadioTech!= undefined).assertTrue();
            }).catch((err) => {
                console.log(err);
                expect().expect().assertFail();
                return;
            })
        }
        done()
    })

    /*
    * @tc.number  Telephony_NetworkSearch_getRadioTech_Promise_0300
    * @tc.name    The slotId parameter input is 1, the test getRadioTech() query function is executed 1000 times,
    *             and the output delay is less than 500000us
    * @tc.desc    Performance test
    */
    it('Telephony_NetworkSearch_getRadioTech_Promise_0300', 0, async function (done) {
        const startTime = (new Date).getTime();
        for (let index = 0; index < TEST_RUN_TIME; index++) {
            radio.getRadioTech(STLO_1).then(({psRadioTech, csRadioTech}) => {
                //expect do nothing for performance test
            }).catch((err) => {
                console.log(err);
                expect().assertFail();
                return;
            })
        }
        const endTime = (new Date).getTime();
        expect(endTime - startTime).assertLess(MSEC_500);
        done()
    })

    /*
    * @tc.number  Telephony_NetworkSearch_getSignalInformation_Promise_0100
    * @tc.name    SlotId parameter input is 1, test getSignalInformation() query function,
    *             return signal strength list information
    * @tc.desc    Function test
    */
    it('Telephony_NetworkSearch_getSignalInformation_Promise_0100', 0, async function (done) {
        radio.getSignalInformation(STLO_1).then((data) => {
            expect(data != null && data != undefined).assertTrue();
            data.forEach((item) => {
                expect(item.signalLevel >= SIGNAL_STRENGTH_INVALID &&
                item.signalLevel <= SIGNAL_STRENGTH_HIGHEST).assertTrue();
                expect(item.signalType).assertTrue();
            })
        }).catch((err) => {
            console.log(err);
            expect().assertFail();
            return;
        })
        done()
    })

    /*
    * @tc.number  Telephony_NetworkSearch_getSignalInformation_Promise_0200
    * @tc.name    The slotId parameter input is 1, the test getSignalInformation() query function is executed 1000
    *             times, and the signal strength list information is returned each time
    * @tc.desc    Reliability test
    */
    it('Telephony_NetworkSearch_getSignalInformation_Promise_0200', 0, async function (done) {
        for (let index = 0; index < TEST_RUN_TIME; index++) {
            radio.getSignalInformation(STLO_1).then((data) => {
                expect(data != null && data != undefined).assertTrue();
            }).catch((err) => {
                console.log(err);
                expect().assertFail();
                return;
            })
        }
        done()
    })

     /*
     * @tc.number  Telephony_NetworkSearch_getSignalInformation_Promise_0300
     * @tc.name    The slotId parameter input is 1, the test getNetworkState() query function is executed 1000 times,
     *             and the output delay is less than 500000us
     * @tc.desc    Performance test
     */
     it('Telephony_NetworkSearch_getSignalInformation_Promise_0300', 0, async function (done) {
         const startTime = (new Date).getTime();
         for (let index = 0; index < TEST_RUN_TIME; index++) {
             radio.getSignalInformation(STLO_1).then((data) => {
                 //expect do nothing for performance test
             }).catch((err) => {
                 console.log(err);
                 expect().assertFail();
                 return;
             })
         }
         const endTime = (new Date).getTime();
         expect(endTime - startTime).assertLess(MSEC_500);
         done()
     })
 })