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

import cellular from '@ohos.telephony.data.d.ts'
import { describe, beforeAll, beforeEach, afterEach, afterAll, it, expect } from 'deccjsunit/index'


describe('Telephony_CellularData_isCellularDataEnabled_Async_0100', function () {

    const USEC_500 = 500 ;
    const TTEST_RUN_TIME_1000 = 1000 ;

    /**
    * @tc.number   Telephony_CellularData_isCellularDataEnabled_Async_0100
    * @tc.name     User data switch default state
    * @tc.desc     Function test
    */
    it('Telephony_CellularData_isCellularDataEnabled_Async_0100', 0, function () {
        cellular.isCellularDataEnabled((err, data) => {
            if (err) {
                expect(false == true).assertTrue();
                return;
            }
            expect(data).assertEqual(false);
        })
    })

    /**
     * @tc.number   Telephony_CellularData_isCellularDataEnabled_Promise_0200
     * @tc.name     User data switch default state
     * @tc.desc     Function test
     */
    it('Telephony_CellularData_isCellularDataEnabled_Promise_0200', 0, function () {
        cellular.isCellularDataEnabled().then((data) => {
            expect(data).assertEqual(false);
        }).catch((err) => {
            expect(false == true).assertTrue();
            return;
        });
    })


    /**
     * @tc.number   Telephony_CellularData_getCellularDataState_Async_0300
     * @tc.name     Verify cellular data status with data switch off
     * @tc.desc     Function test
     */
    it('Telephony_CellularData_getCellularDataState_Async_0300', 0, function () {
        cellular.getCellularDataState((err, data) => {
            if (err) {
                expect(false == true).assertTrue();
                return;
            }
            expect(data).assertEqual(cellular.DATA_STATE_DISCONNECTED);
        })
    })

    /**
     * @tc.number   Telephony_CellularData_getCellularDataState_Promise_0400
     * @tc.name     Verify cellular data status with data switch off
     * @tc.desc     Function test
     */
    it('Telephony_CellularData_getCellularDataState_Promise_0400', 0, function () {
        cellular.getCellularDataState().then((data) => {
            expect(data).assertEqual(cellular.DATA_STATE_DISCONNECTED);
        }).catch((err) => {
            expect(false == true).assertTrue();
            return;
        });
    })


    /**
     * @tc.number   Telephony_CellularData_getCellularDataState_Async_0500
     * @tc.name     Open the data switch and query the switch status and data status
     * @tc.desc     Function test
     */
    it('Telephony_CellularData_getCellularDataState_Async_0500', 0, function () {
        cellular.enableCellularData((err) => {
            if (err) {
                expect(false == true).assertTrue();
                return;
            }
            //Turn on the cellular data switch
        })
        cellular.isCellularDataEnabled((err, data) => {
            if (err) {
                expect(false == true).assertTrue();
                return;
            }
            expect(data).assertEqual(true);
        })
        cellular.getCellularDataState((err, data) => {
            if (err) {
                expect(false == true).assertTrue();
                return;
            }
            expect(data).assertEqual(cellular.DATA_STATE_CONNECTING);
        })
        setTimeout(() => {
        }, 1000);
        cellular.getCellularDataState((err, data) => {
            if (err) {
                expect(false == true).assertTrue();
                return;
            }
            expect(data).assertEqual(cellular.DATA_STATE_CONNECTED);
        })

    })

    /**
     * @tc.number   Telephony_CellularData_getCellularDataState_Async_0600
     * @tc.name     Turn off the data switch and query the switch status and data status
     * @tc.desc     Function test
     */
    it('Telephony_CellularData_getCellularDataState_Async_0600', 0, function () {
        cellular.disableCellularData((err) => {
            if (err) {
                expect(false == true).assertTrue();
                return;
            }
            //Turn off the cellular data switch
        })
        cellular.isCellularDataEnabled((err, data) => {
            if (err) {
                expect(false == true).assertTrue();
                return;
            }
            expect(data).assertEqual(false);
        })
        cellular.getCellularDataState((err, data) => {
            if (err) {
                expect(false == true).assertTrue();
                return;
            }
            expect(data).assertEqual(cellular.DATA_STATE_DISCONNECTED);
        })
    })

    /**
     * @tc.number   Telephony_CellularData_getCellularDataState_Promise_0700
     * @tc.name     Open the data switch and query the switch status and data status
     * @tc.desc     Function test
     */
    it('Telephony_CellularData_getCellularDataState_Promise_0700', 0, function () {
        cellular.enableCellularData().then(() => {
            //Turn on the cellular data switch
        }).catch((err) => {
            expect(false == true).assertTrue();
            return;
        });
        cellular.isCellularDataEnabled().then((data) => {
            expect(data).assertEqual(true);
        }).catch((err) => {
            expect(false == true).assertTrue();
            return;
        });
        cellular.getCellularDataState().then((data) => {
            expect(data).assertEqual(cellular.DATA_STATE_CONNECTING);
        }).catch((err) => {
            expect(false == true).assertTrue();
            return;
        });
        setTimeout(() => {
        }, 1000);
        cellular.getCellularDataState().then((data) => {
            expect(data).assertEqual(cellular.DATA_STATE_CONNECTED);
        }).catch((err) => {
            expect(false == true).assertTrue();
            return;
        });
    })

    /**
     * @tc.number   Telephony_CellularData_getCellularDataState_Promise_0800
     * @tc.name     Turn off the data switch and query the switch status and data status
     * @tc.desc     Function test
     */
    it('Telephony_CellularData_getCellularDataState_Promise_0800', 0, function () {
        cellular.disableCellularData().then(() => {
            //Turn off the cellular data switch
        }).catch((err) => {
            expect(false == true).assertTrue();
            return;
        });
        cellular.isCellularDataEnabled().then((data) => {
            expect(data).assertEqual(false);
        }).catch((err) => {
            expect(false == true).assertTrue();
            return;
        });
        cellular.getCellularDataState().then((data) => {
            expect(data).assertEqual(cellular.DATA_STATE_DISCONNECTED);
        }).catch((err) => {
            expect(false == true).assertTrue();
            return;
        });
    })

    /**
     * @tc.number   Telephony_CellularData_getCellularDataState_Async_0900
     * @tc.name     CellularData Open state 1000 queries
     * @tc.desc     Reliability test
     */
    it('Telephony_CellularData_getCellularDataState_Async_0900', 0, function () {
        for (let index = 0; index < TTEST_RUN_TIME_1000; index++) {
            cellular.isCellularDataEnabled((err, data) => {
                if (err) {
                    expect(false == true).assertTrue();
                    return;
                }
                expect(data).assertEqual(false);
            })
        }
    })

    /**
     * @tc.number   Telephony_CellularData_getCellularDataState_Async_1000
     * @tc.name     CellularData Open state 1000 queries
     * @tc.desc     Reliability test
     */
    it('Telephony_CellularData_getCellularDataState_Async_1000', 0, function () {
        const startTime = (new Date).getTime();
        for (let index = 0; index < TTEST_RUN_TIME_1000; index++) {
            cellular.isCellularDataEnabled((err, data) => {
                if (err) {
                    expect(false == true).assertTrue();
                    return;
                }
            })
        }
        const endTime = (new Date).getTime();
        const bool = (endTime - startTime) < USEC_500;
        expect(bool).assertEqual(true);
    })

    /**
     * @tc.number   Telephony_CellularData_isCellularDataEnabled_Promise_1100
     * @tc.name     CellularData Open state 1000 queries
     * @tc.desc     Reliability test
     */
    it('Telephony_CellularData_isCellularDataEnabled_Promise_1100', 0, function () {
        for (let index = 0; index < TTEST_RUN_TIME_1000; index++) {
            cellular.isCellularDataEnabled().then((data) => {
                expect(data).assertEqual(false);
            }).catch((err) => {
                expect(false == true).assertTrue();
                return;
            });
        }
    })

    /**
    * @tc.number   Telephony_CellularData_isCellularDataEnabled_Promise_1200
    * @tc.name     IsCellularDataEnabled Perform an average of 20 performance tests
    * @tc.desc     Performance test
    */
    it('Telephony_CellularData_isCellularDataEnabled_Promise_1200', 0, function () {
        const startTime = (new Date).getTime();
        for (let index = 0; index < TTEST_RUN_TIME_1000; index++) {
            cellular.isCellularDataEnabled().then((data) => {
            }).catch((err) => {
                expect(false == true).assertTrue();
                return;
            });
        }
        const endTime = (new Date).getTime();
        const bool = (endTime - startTime) < USEC_500;
        expect(bool).assertEqual(true);
    })

    /**
    * @tc.number   Telephony_CellularData_getCellularDataState_Async_1300
    * @tc.name     GetCellularDataState Perform an average of 20 performance tests
    * @tc.desc     Performance test
    */
    it('Telephony_CellularData_getCellularDataState_Async_1300', 0, function () {
        for (let index = 0; index < TTEST_RUN_TIME_1000; index++) {
            cellular.getCellularDataState((err, data) => {
                if (err) {
                    expect(false == true).assertTrue();
                    return;
                }
                expect(data).assertEqual(cellular.DATA_STATE_DISCONNECTED);
            })
        }
    })

    /**
    * @tc.number   Telephony_CellularData_getCellularDataState_Promise_1400
    * @tc.name     GetCellularDataState Perform an average of 20 performance tests
    * @tc.desc     Performance test
    */
    it('Telephony_CellularData_getCellularDataState_Promise_1400', 0, function () {
        for (let index = 0; index < TTEST_RUN_TIME_1000; index++) {
            cellular.getCellularDataState().then((data) => {
                expect(data).assertEqual(cellular.DATA_STATE_DISCONNECTED);
            }).catch((err) => {
                expect(false == true).assertTrue();
                return;
            });
        }
    })

    /**
    * @tc.number   Telephony_CellularData_getCellularDataState_Async_1500
    * @tc.name     Access to cellular data has been linked 1000 times
    * @tc.desc     Reliability test
    */
    it('Telephony_CellularData_getCellularDataState_Async_1500', 0, function () {
        cellular.enableCellularData((err) => {
            if (err) {
                expect(false == true).assertTrue();
                return;
            }
            //Turn on the cellular data switch
        })

        cellular.isCellularDataEnabled((err, data) => {
            if (err) {
                expect(false == true).assertTrue();
                return;
            }
            expect(data).assertEqual(true);
        })
        cellular.getCellularDataState((err, data) => {
            if (err) {
                expect(false == true).assertTrue();
                return;
            }
            expect(data).assertEqual(cellular.DATA_STATE_CONNECTING);
        })
        setTimeout(() => {
        }, 1000);
        cellular.getCellularDataState((err, data) => {
            if (err) {
                expect(false == true).assertTrue();
                return;
            }
            expect(data).assertEqual(cellular.DATA_STATE_CONNECTED);
        })
        for (let index = 0; index < TTEST_RUN_TIME_1000; index++) {
            cellular.getCellularDataState((err, data) => {
                if (err) {
                    expect(false == true).assertTrue();
                    return;
                }
                expect(data).assertEqual(cellular.DATA_STATE_CONNECTED);
            })
        }
    })

    /**
     * @tc.number   Telephony_CellularData_getCellularDataState_Async_1600
     * @tc.name     getCellularDataState Perform an average of 1000 performance tests
     * @tc.desc     Performance test
     */
    it('Telephony_CellularData_getCellularDataState_Async_1600', 0, function () {
        const startTime = (new Date).getTime();
        for (let index = 0; index < TTEST_RUN_TIME_1000; index++) {
            cellular.getCellularDataState((err, data) => {
                if (err) {
                    expect(false == true).assertTrue();
                    return;
                }
                expect(data).assertEqual(cellular.DATA_STATE_CONNECTED);
            })
        }
        const endTime = (new Date).getTime();
        const bool = (endTime - startTime) < USEC_500;
        expect(bool).assertEqual(true);
    })

    /**
    * @tc.number   Telephony_CellularData_getCellularDataState_Promise_1700
    * @tc.name     Access to cellular data has been linked 1000 times
    * @tc.desc     Reliability test
    */
    it('Telephony_CellularData_getCellularDataState_Promise_1700', 0, function () {
        for (let index = 0; index < TTEST_RUN_TIME_1000; index++) {
            cellular.getCellularDataState().then((data) => {
                expect(data).assertEqual(cellular.DATA_STATE_CONNECTED);
            }).catch((err) => {
                expect(false == true).assertTrue();
                return;
            });
        }
    })

    /**
     * @tc.number   Telephony_CellularData_getCellularDataState_Promise_1800
     * @tc.name     getCellularDataState Perform an average of 1000 performance tests
     * @tc.desc     Performance test
     */
    it('Telephony_CellularData_getCellularDataState_Promise_1800', 0, function () {
        const startTime = (new Date).getTime();
        for (let index = 0; index < TTEST_RUN_TIME_1000; index++) {
            cellular.getCellularDataState((err, data) => {
                expect(data).assertEqual(cellular.DATA_STATE_CONNECTED);
            }).catch((err) => {
                expect(false == true).assertTrue();
                return;
            });
        }
        const endTime = (new Date).getTime();
        const bool = (endTime - startTime) < USEC_500;
        expect(bool).assertEqual(true);
    })
})