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

import sim from '@ohos.telephony_sim'
import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index'

describe('SimTest', function () {
    const SOLTID_1 = 1;
    const SOLTID_0 = 0;
    const MSEC_500 = 500;
    const TIMES_1000 = 1000;

    /*
    * @tc.number  Telephony_Sim_getISOCountryCodeForSim_Async_0100
    * @tc.name    The SOLTID_1 parameter input is 1, test the getISOCountryCodeForSim() query function,
    *             and return  the status of  card 1
    * @tc.desc    Function test
    */
    it('Telephony_Sim_getISOCountryCodeForSim_Async_0100', 0, async function (done) {
        sim.getISOCountryCodeForSim(SOLTID_1,(err, data) => {
            if (err) {
                expect().assertFail();
                return;
            }
            expect(data).assertTrue();
        })
        done();
    })

    /*)
    * @tc.number  Telephony_Sim_getISOCountryCodeForSim_Async_0200
    * @tc.name    The SOLTID_1 parameter input is 0, test the getISOCountryCodeForSim() query function,
    *             and return  the status of  card 1
    * @tc.desc    Function test
    */
    it('Telephony_Sim_getISOCountryCodeForSim_Async_0200', 0, async function (done) {
        sim.getISOCountryCodeForSim(SOLTID_0,(err, data) => {
            if (err) {
                expect().assertFail();
                return;
            }
            expect(data).assertTrue();
        })
        done();
    })

    /*
    * @tc.number  Telephony_Sim_getISOCountryCodeForSim_Async_0300
    * @tc.name    Test getISOCountryCodeForSim() query function is executed 1000 TIMES_1000,
    *             and the output delay is less than 500ms
    * @tc.desc    Function test
    */
    it('Telephony_Sim_getISOCountryCodeForSim_Async_0300', 0, async function (done) {
        const startTime = (new Date).getTime();
        for (let index = 0; index < TIMES_1000; index++) {
            sim.getISOCountryCodeForSim(SOLTID_1,(err, data) => {
                expect().assertFail();
                return;
            })
        }
        const endTime = (new Date).getTime();
        const bool = (endTime - startTime) < MSEC_500;
        expect(bool).assertTrue();
        done();
    })

    /*
    * @tc.number  Telephony_Sim_getISOCountryCodeForSim_Async_0400
    * @tc.name    The SOLTID_1 parameter input is 1, test getISOCountryCodeForSim()  query function is executed 1000 TIMES_1000,
    *             and the status of  card 1 can be returned every time
    * @tc.desc    Function test
    */
    it('Telephony_Sim_getISOCountryCodeForSim_Async_0400', 0, async function (done) {
        for (let index = 0; index < TIMES_1000; index++) {
            sim.getISOCountryCodeForSim(SOLTID_1,(err, data) => {
                if (err) {
                    expect().assertFail();
                    return;
                }
                expect(data).assertTrue();
            })
        }
        done();
    })

    /*
    * @tc.number  Telephony_Sim_getISOCountryCodeForSim_Promise_0100
    * @tc.name    The SOLTID_1 parameter input is 1, test the getISOCountryCodeForSim() query function,
    *             and return  the status of  card 1
    * @tc.desc    Function test
    */
    it('Telephony_Sim_getISOCountryCodeForSim_Promise_0100', 0, async function (done) {
        sim.getISOCountryCodeForSim(SOLTID_1).then((data) => {
            expect(data).assertTrue();
        }).catch((err) => {
            expect().assertFail();
            return;
        })
        done();
    })

    /*
    * @tc.number  Telephony_Sim_getISOCountryCodeForSim_Async_0200
    * @tc.name    The SOLTID_1 parameter input is 0, test the getISOCountryCodeForSim() query function,
    *             and return  the status of  card 1
    * @tc.desc    Function test
    */
    it('Telephony_Sim_getISOCountryCodeForSim_Promise_0200', 0, async function (done) {
        sim.getISOCountryCodeForSim(SOLTID_0).then((data) => {
            expect(data).assertTrue();
        }).catch((err) => {
            expect().assertFail();
            return;
        })
        done();
    })

    /*
    * @tc.number  Telephony_Sim_getISOCountryCodeForSim_Async_0300
    * @tc.name    Test getISOCountryCodeForSim() query function is executed 1000 TIMES_1000,
    *             and the output delay is less than 500ms
    * @tc.desc    Function test
    */
    it('Telephony_Sim_getISOCountryCodeForSim_Promise_0300', 0, async function (done) {
        const startTime = (new Date).getTime();
        for (let index = 0; index < TIMES_1000; index++) {
            sim.getISOCountryCodeForSim(SOLTID_1).then((err, data) => {
            }).catch((err) => {
                expect().assertFail();
                return;
            })
            const endTime = (new Date).getTime();
            const bool = (endTime - startTime) < MSEC_500;
            expect(bool).assertTrue();
        }
        done();
    })

    /*
    * @tc.number  Telephony_Sim_getISOCountryCodeForSim_Async_0400
    * @tc.name    The SOLTID_1 parameter input is 1, test getISOCountryCodeForSim()  query function is executed 1000 TIMES_1000,
    *             and the status of  card 1 can be returned every time
    * @tc.desc    Function test
    */
    it('Telephony_Sim_getISOCountryCodeForSim_Promise_0400', 0, async function (done) {
        for (let index = 0; index < TIMES_1000; index++) {
            sim.getISOCountryCodeForSim(SOLTID_1).then((data) => {
            }).catch((err) => {
                expect().assertFail();
                return;
            })
        }
        done();
    })

    /*
    * @tc.number  Telephony_Sim_getSimOperatorNumeric_Async_0100
    * @tc.name    The SOLTID_1 parameter input is 1, test the getSimOperatorNumeric() query function,
    *             and return  the status of  card 1
    * @tc.desc    Function test
    */
    it('Telephony_Sim_getSimOperatorNumeric_Async_0100', 0, async function (done) {
        sim.getSimOperatorNumeric(SOLTID_1,(err, data) => {
            if (err) {
                expect().assertFail();
                return;
            }
            expect(data).assertTrue();
        })
        done();
    })

    /*
    * @tc.number  Telephony_Sim_getISOCountryCodeForSim_Async_0200
    * @tc.name    The SOLTID_1 parameter input is 0, test the getSimOperatorNumeric() query function,
    *             and return  the status of  card 1
    * @tc.desc    Function test
    */
    it('Telephony_Sim_getSimOperatorNumeric_Async_0200', 0, async function (done) {
        sim.getSimOperatorNumeric(SOLTID_0,(err, data) => {
            if (err) {
                expect().assertFail();
                return;
            }
            expect(data).assertTrue();
        })
        done();
    })

    /*
    * @tc.number  Telephony_Sim_getISOCountryCodeForSim_Async_0300
    * @tc.name    Test getSimOperatorNumeric() query function is executed 1000 TIMES_1000,
    *             and the output delay is less than 500ms
    * @tc.desc    Function test
    */
    it('Telephony_Sim_getSimOperatorNumeric_Async_0300', 0, async function (done) {
        const startTime = (new Date).getTime();
        for (let index = 0; index < TIMES_1000; index++) {
            sim.getSimOperatorNumeric(SOLTID_1,(err, data) => {
                expect().assertFail();
                return;
            })
        }
        const endTime = (new Date).getTime();
        const bool = (endTime - startTime) < MSEC_500;
        expect(bool).assertTrue();
        done();
    })

    /*
    * @tc.number  Telephony_Sim_getISOCountryCodeForSim_Async_0400
    * @tc.name    The SOLTID_1 parameter input is 1, test getSimOperatorNumeric()  query function is executed 1000 TIMES_1000,
    *             and the status of  card 1 can be returned every time
    * @tc.desc    Function test
    */
    it('Telephony_Sim_getSimOperatorNumeric_Async_0400', 0, async function (done) {
        for (let index = 0; index < TIMES_1000; index++) {
            sim.getSimOperatorNumeric(SOLTID_1,(err, data) => {
                if (err) {
                    expect().assertFail();
                    return;
                }
                expect(data).assertTrue();
            })
        }
        done();
    })

    /*
    * @tc.number  Telephony_Sim_getSimOperatorNumeric_Promise_0100
    * @tc.name    The SOLTID_1 parameter input is 1, test the getSimOperatorNumeric() query function,
    *             and return  the status of  card 1
    * @tc.desc    Function test
    */
    it('Telephony_Sim_getSimOperatorNumeric_Promise_0100', 0, async function (done) {
        sim.getSimOperatorNumeric(SOLTID_1).then((data) => {
            expect(data).assertTrue();
        }).catch((err) => {
            expect().assertFail();
            return;
        })
        done();
    })

    /*
    * @tc.number  Telephony_Sim_getISOCountryCodeForSim_Async_0400
    * @tc.name    The SOLTID_1 parameter input is 0, test the getSimOperatorNumeric() query function,
    *             and return  the status of  card 1
    * @tc.desc    Function test
    */
    it('Telephony_Sim_getSimOperatorNumeric_Promise_0200', 0, async function (done) {
        sim.getSimOperatorNumeric(SOLTID_0).then((data) => {
            expect(data).assertTrue();
        }).catch((err) => {
            expect().assertFail();
            return;
        })
        done();
    })

    /*
    * @tc.number  Telephony_Search_getState_0300
    * @tc.name    Test getSimOperatorNumeric() query function is executed 1000 TIMES_1000,
    *             and the output delay is less than 500ms
    * @tc.desc    Function test
    */
    it('Telephony_Sim_getSimOperatorNumeric_Promise_0300', 0, async function (done) {
        const startTime = (new Date).getTime();
        for (let index = 0; index < TIMES_1000; index++) {
            sim.getSimOperatorNumeric(SOLTID_1).then((err, data) => {
            }).catch((err) => {
                expect().assertFail();
                return;
            })
            const endTime = (new Date).getTime();
            const bool = (endTime - startTime) < MSEC_500;
            expect(bool).assertTrue();
        }
        done();
    })

    /*
    * @tc.number  Telephony_Sim_getSimOperatorNumeric_Promise_0400
    * @tc.name    The SOLTID_1 parameter input is 1, test getSimOperatorNumeric()  query function is executed 1000 TIMES_1000,
    *             and the status of  card 1 can be returned every time
    * @tc.desc    Function test
    */
    it('Telephony_Sim_getSimOperatorNumeric_Promise_0400', 0, async function (done) {
        for (let index = 0; index < TIMES_1000; index++) {
            sim.getSimOperatorNumeric().then((data) => {
                expect(data).assertTrue();
            }).catch((err) => {
                expect().assertFail();
                return;
            })
        }
        done();
    })

    /*
    * @tc.number  Telephony_Sim_getSimSpn_Async_0100
    * @tc.name    The SOLTID_1 parameter input is 1, test the getSimSpn() query function,
    *             and return  the status of  card 1
    * @tc.desc    Function test
    */
    it('Telephony_Sim_getSimSpn_Async_0100', 0, async function (done) {
        sim.getSimSpn(SOLTID_1,(err, data) => {
            if (err) {
                expect().assertFail();
                return;
            }
            expect(data).assertTrue();
        })
        done();
    })

    /*
    * @tc.number  Telephony_Sim_getISOCountryCodeForSim_Async_0200
    * @tc.name    The SOLTID_1 parameter input is 0, test the getSimSpn() query function,
    *             and return  the status of  card 1
    * @tc.desc    Function test
    */
    it('Telephony_Sim_getSimSpn_Async_0200', 0, async function (done) {
        sim.getSimSpn(SOLTID_0,(err, data) => {
            if (err) {
                expect().assertFail();
                return;
            }
            expect(data).assertTrue();
        })
        done();
    })

    /*
    * @tc.number  Telephony_Sim_getISOCountryCodeForSim_Async_0300
    * @tc.name    Test getSimSpn() query function is executed 1000 TIMES_1000,
    *             and the output delay is less than 500ms
    * @tc.desc    Function test
    */
    it('Telephony_Sim_getSimSpn_Async_0300', 0, async function (done) {
        const startTime = (new Date).getTime();
        for (let index = 0; index < TIMES_1000; index++) {
            sim.getSimSpn(SOLTID_1,(err, data) => {
                expect().assertFail();
                return;
            })
        }
        const endTime = (new Date).getTime();
        const bool = (endTime - startTime) < MSEC_500;
        expect(bool).assertTrue();
        done();
    })

    /*
    * @tc.number  Telephony_Sim_getSimSpn_Async_0400
    * @tc.name    The SOLTID_1 parameter input is 1, test getSimSpn()  query function is executed 1000 TIMES_1000,
    *             and the status of  card 1 can be returned every time
    * @tc.desc    Function test
    */
    it('Telephony_Sim_getSimSpn_Async_0400', 0, async function (done) {
        for (let index = 0; index < TIMES_1000; index++) {
            sim.getSimSpn(SOLTID_1,(err, data) => {
                if (err) {
                    expect().assertFail();
                    return;
                }
                expect(data).assertTrue();
            })
        }
        done();
    })

    /*
    * @tc.number  Telephony_Sim_getSimSpn_Promise_0100
    * @tc.name    The SOLTID_1 parameter input is 1, test the getSimSpn() query function,
    *             and return  the status of  card 1
    * @tc.desc    Function test
    */
    it('Telephony_Sim_getSimSpn_Promise_0100', 0, async function (done) {
        sim.getSimSpn(SOLTID_1).then((data) => {
            expect(data).assertTrue();
        }).catch((err) => {
            expect().assertFail();
            return;
        })
        done();
    })

    /*
    * @tc.number  Telephony_Sim_getSimSpn_Promise_0200
    * @tc.name    The SOLTID_1 parameter input is 0, test the getSimSpn() query function,
    *             and return  the status of  card 1
    * @tc.desc    Function test
    */
    it('Telephony_Sim_getSimSpn_Promise_0200', 0, async function (done) {
        sim.getSimSpn(SOLTID_0).then((data) => {
            expect(data).assertTrue();
        }).catch((err) => {
            expect().assertFail();
            return;
        })
        done();
    })

    /*
    * @tc.number  Telephony_Search_getState_0300
    * @tc.name    Test getSimSpn() query function is executed 1000 TIMES_1000,
    *             and the output delay is less than 500ms
    * @tc.desc    Function test
    */
    it('Telephony_Sim_getSimSpn_Promise_0300', 0, async function (done) {
        const startTime = (new Date).getTime();
        for (let index = 0; index < TIMES_1000; index++) {
            sim.getSimSpn(SOLTID_1).then((err, data) => {
                const endTime = (new Date).getTime();
                const bool = (endTime - startTime) < MSEC_500;
                expect(bool).assertTrue();
            }).catch((err) => {
                expect().assertFail();
                return;
            })
        }
        done();
    })

    /*
    * @tc.number  Telephony_Sim_getSimSpn_Promise_0400
    * @tc.name    The SOLTID_1 parameter input is 1, test getSimSpn()  query function is executed 1000 TIMES_1000,
    *             and the status of  card 1 can be returned every time
    * @tc.desc    Function test
    */
    it('Telephony_Sim_getSimSpn_Promise_0400', 0, async function (done) {
        for (let index = 0; index < TIMES_1000; index++) {
            sim.getSimSpn().then((data) => {
                expect(data).assertTrue();
            }).catch((err) => {
                expect().assertFail();
                return;
            })
        }
        done();
    })

    /*
    * @tc.number  Telephony_Sim_getSimState_Async_0100
    * @tc.name    The SOLTID_1 parameter input is 1, test the getSimState() query function,
    *             and return  the status of  card 1
    * @tc.desc    Function test
    */
    it('Telephony_Sim_getSimState_Async_0100', 0, async function (done) {
        sim.getSimState(SOLTID_1,(err, data) => {
            if (err) {
                expect(data != sim.SIM_STATE_UNKNOWN).assertTrue();
                return;
            }
            expect(data).assertTrue();
        })
        done();
    })

    /*
    * @tc.number  Telephony_Sim_getISOCountryCodeForSim_Async_0200
    * @tc.name    The SOLTID_1 parameter input is 0, test the getSimState() query function,
    *             and return  the status of  card 1
    * @tc.desc    Function test
    */
    it('Telephony_Sim_getSimState_Async_0200', 0, async function (done) {
        sim.getSimState(SOLTID_0,(err, data) => {
            if (err) {
                expect(data != sim.SIM_STATE_UNKNOWN).assertTrue();
                return;
            }
            expect(data).assertTrue();
        })
        done();
    })

    /*
    * @tc.number  Telephony_Sim_getISOCountryCodeForSim_Async_0300
    * @tc.name    Test getSimState() query function is executed 1000 TIMES_1000,
    *             and the output delay is less than 500ms
    * @tc.desc    Function test
    */
    it('Telephony_Sim_getSimState_Async_0300', 0, async function (done) {
        const startTime = (new Date).getTime();
        for (let index = 0; index < TIMES_1000; index++) {
            sim.getSimState(SOLTID_1,(err, data) => {
                expect(data != sim.SIM_STATE_UNKNOWN).assertTrue();
                return;
            })
        }
        const endTime = (new Date).getTime();
        const bool = (endTime - startTime) < MSEC_500;
        expect(bool).assertTrue();
        done();
    })

    /*
    * @tc.number  Telephony_Sim_getISOCountryCodeForSim_Async_0400
    * @tc.name    The SOLTID_1 parameter input is 1, test getSimState()  query function is executed 1000 TIMES_1000,
    *             and the status of  card 1 can be returned every time
    * @tc.desc    Function test
    */
    it('Telephony_Sim_getSimState_Async_0400', 0, async function (done) {
        for (let index = 0; index < TIMES_1000; index++) {
            sim.getSimState(SOLTID_1,(err, data) => {
                if (err) {
                    expect(data != sim.SIM_STATE_UNKNOWN).assertTrue();
                    return;
                }
                expect(data).assertTrue();
            })
        }
        done();
    })

    /*
    * @tc.number  Telephony_Sim_getSimState_Promise_0100
    * @tc.name    The SOLTID_1 parameter input is 1, test the getSimState() query function,
    *             and return  the status of  card 1
    * @tc.desc    Function test
    */
    it('Telephony_Sim_getSimState_Promise_0100', 0, async function (done) {
        sim.getSimState(SOLTID_1).then((data) => {
            expect(data).assertTrue();
        }).catch((err) => {
            expect().assertFail();
            return;
        })
        done();
    })

    /*
    * @tc.number  Telephony_Sim_getSimState_Promise_0200
    * @tc.name    The SOLTID_1 parameter input is 0, test the getSimState() query function,
    *             and return  the status of  card 1
    * @tc.desc    Function test
    */
    it('Telephony_Sim_getSimState_Promise_0200', 0, async function (done) {
        sim.getSimState(SOLTID_0).then((data) => {
            expect(data).assertTrue();
        }).catch((err) => {
            expect().assertFail();
            return;
        })
        done();
    })

    /*
    * @tc.number  Telephony_Search_getState_0300
    * @tc.name    Test getSimState() query function is executed 1000 TIMES_1000,
    *             and the output delay is less than 500ms
    * @tc.desc    Function test
    */
    it('Telephony_Sim_getSimState_Promise_0300', 0, async function (done) {
        const startTime = (new Date).getTime();
        for (let index = 0; index < TIMES_1000; index++) {
            sim.getSimState(SOLTID_1).then((err, data) => {
            }).catch((err) => {
                expect().assertFail();
                return;
            })
            const endTime = (new Date).getTime();
            const bool = (endTime - startTime) < MSEC_500;
            expect(bool).assertTrue();
        }
        done();
    })

    /*
    * @tc.number  Telephony_Sim_getSimState_Promise_0400
    * @tc.name    The SOLTID_1 parameter input is 1, test getSimState()  query function is executed 1000 TIMES_1000,
    *             and the status of  card 1 can be returned every time
    * @tc.desc    Function test
    */
    it('Telephony_Sim_getSimState_Promise_0400', 0, async function (done) {
        for (let index = 0; index < TIMES_1000; index++) {
            sim.getSimState().then((data) => {
                expect(data).assertTrue();
            }).catch((err) => {
                expect().assertFail();
                return;
            })
        }
        done();
    })
})