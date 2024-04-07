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

import sms from '@ohos.telephony_sms'
import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index'

describe('SmsMmsTest', function () {
    var sendCallback = (err, data) => {
    };
    var deliveryCallback = (err, data) => {
    };
    var rawArray = new Array(
        0x08, 0x91, 0x68, 0x31, 0x08, 0x20, 0x00, 0x75, 0xF4, 0x24, 0x0D,
        0x91, 0x68, 0x81, 0x29, 0x56, 0x29, 0x83, 0xF6, 0x00, 0x00, 0x12,
        0x40, 0x80, 0x01, 0x02, 0x14, 0x23, 0x02, 0xC1, 0x30);
    var rawArrayNull = new Array();
    /*
    * @tc.number  Telephony_SmsMms_sendMessage_0100
    * @tc.name    Call the interface sendMessage, set the card slot parameter "slotId" to 1,
    *             and send SMS successfully
    * @tc.desc    Function test
    */
    it('Telephony_SmsMms_sendMessage_0100', 0, async function (done) {
        sms.sendMessage({
            slotId: 1,
            destinationHost: '18211305277',
            content: 'hello',
            sendCallback: (err, value) => {
                if (err) {
                    expect().assertFail();
                    return;
                }
                expect(value.result).assertEquals(sms.SEND_SMS_SUCCESS);
            },
            deliveryCallback: (err, value) => {
                if (err) {
                    expect().assertFail();
                    return;
                }
                console.log("deliveryCallback success sendResult = " + value.pdu);
                expect(value.pdu).assertTrue();
            }
        })
        done()
    })

    /*
    * @tc.number  Telephony_SmsMms_sendMessage_0200
    * @tc.name    Call the interface sendMessage, set the card slot parameter "slotId" to 0,
    *             SMS failed to send
    * @tc.desc    Function test
    */
    it('Telephony_SmsMms_sendMessage_0200', 0, async function (done) {
        sms.sendMessage({
            slotId: 0,
            destinationHost: '18211305277',
            content: 'hello',
            sendCallback: (err, value) => {
                if (err) {
                    expect().assertFail();
                    return;
                }
                console.log("sendCallback success sendResult = " + value.result);
                const bool =  (value.result != sms.SEND_SMS_SUCCESS);
                expect(bool).assertTrue();
            },
            deliveryCallback: (err, value) => {
                if (err) {
                    expect().assertFail();
                    return;
                }
                console.log("deliveryCallback success sendResult = " + value.pdu);
                expect(value.pdu).assertTrue();
            }
        })
        done()
    })

    /*
    * @tc.number  Telephony_SmsMms_sendMessage_0300
    * @tc.name    Call interface sendMessage,
    *             set the destinationHost "destinationHost" is not empty,
    *             send SMS successfully
    * @tc.desc    Function test
    */
    it('Telephony_SmsMms_sendMessage_0300', 0, async function (done) {
        sms.sendMessage({
            slotId: 1,
            destinationHost: '18211305277',
            content: 'hello',
            sendCallback: (err, value) => {
                if (err) {
                    expect().assertFail();
                    return;
                }
                console.log("sendCallback success sendResult = " + value.result);
                expect(value.result).assertEquals(sms.SEND_SMS_SUCCESS);
            },
            deliveryCallback: (err, value) => {
                if (err) {
                    expect().assertFail();
                    return;
                }
                console.log("deliveryCallback success sendResult = " + value.pdu);
                expect(value.pdu).assertTrue();
            }
        })
        done()
    })

    /*
    * @tc.number  Telephony_SmsMms_sendMessage_0400
    * @tc.name    Call interface sendMessage, set destinationHost "destinationHost" to empty,
    *             send SMS failed
    * @tc.desc    Function test
    */
    it('Telephony_SmsMms_sendMessage_0400', 0, async function (done) {
        sms.sendMessage({
            slotId: 1,
            destinationHost: '',
            content: 'hello',
            sendCallback: (err, value) => {
                if (err) {
                    expect().assertFail();
                    return;
                }
                console.log("sendCallback success sendResult = " + value.result);
                const bool =  (value.result != sms.SEND_SMS_SUCCESS);
                expect(bool).assertTrue();
            },
            deliveryCallback: (err, value) => {
                if (err) {
                    expect().assertFail();
                    return;
                }
                console.log("deliveryCallback success sendResult = " + value.pdu);
                expect(value.pdu).assertTrue();
            }
        })
        done()
    })

    /*
    * @tc.number  Telephony_SmsMms_sendMessage_0500
    * @tc.name    Call interface sendMessage, set the content "content" to empty,
    *             send a message failed
    * @tc.desc    Function test
    */
    it('Telephony_SmsMms_sendMessage_0500', 0, async function (done) {
        sms.sendMessage({
            slotId: 1,
            destinationHost: '18211305277',
            content: '',
            sendCallback: (err, value) => {
                if (err) {
                    expect().assertFail();
                    return;
                }
                console.log("sendCallback success sendResult = " + value.result);
                const bool =  (value.result != sms.SEND_SMS_SUCCESS);
                expect(bool).assertTrue();
            },
            deliveryCallback: (err, value) => {
                if (err) {
                    expect().assertFail();
                    return;
                }
                console.log("deliveryCallback success sendResult = " + value.pdu);
                expect(value.pdu).assertTrue();
            }
        })
        done()
    })

    /*
    * @tc.number  Telephony_SmsMms_sendMessage_0600
    * @tc.name    Call the interface SendMessage, set the length of "content" to 159 bytes,
    *             and send a short message successfully
    * @tc.desc    Function test
    */
    it('Telephony_SmsMms_sendMessage_0600', 0, async function (done) {
        const count = 159;
        let str = '';
        for (let index = 0; index < count; index++) {
            str+='a';
        }
        sms.sendMessage({
            slotId: 1,
            destinationHost: '18211305277',
            content: str,
            sendCallback: (err, value) => {
                if (err) {
                    expect().assertFail();
                    return;
                }
                console.log("sendCallback success sendResult = " + value.result);
                expect(value.result).assertEquals(sms.SEND_SMS_SUCCESS);
            },
            deliveryCallback: (err, value) => {
                if (err) {
                    expect().assertFail();
                    return;
                }
                console.log("deliveryCallback success sendResult = " + value.pdu);
                expect(value.pdu).assertTrue();
            }
        })
        done()
    })

    /*
    * @tc.number  Telephony_SmsMms_sendMessage_0700
    * @tc.name    Call the interface SendMessage, set the length of "content" to 160 bytes,
    *             and send a short message successfully
    * @tc.desc    Function test
    */
    it('Telephony_SmsMms_sendMessage_0700', 0, async function (done) {
        const count = 160;
        let str = '';
        for (let index = 0; index < count; index++) {
            str+='a';
        }
        sms.sendMessage({
            slotId: 1,
            destinationHost: '18211305277',
            content: str,
            sendCallback: (err, value) => {
                if (err) {
                    expect().assertFail();
                    return;
                }
                console.log("sendCallback success sendResult = " + value.result);
                expect(value.result).assertEquals(sms.SEND_SMS_SUCCESS);
            },
            deliveryCallback: (err, value) => {
                if (err) {
                    expect().assertFail();
                    return;
                }
                console.log("deliveryCallback success sendResult = " + value.pdu);
                expect(value.pdu).assertTrue();
            }
        })
        done()
    })

    /*
    * @tc.number  Telephony_SmsMms_sendMessage_0800
    * @tc.name    Call the interface SendMessage, set the length of "content" to 161 bytes,
    *             and send a short message successfully
    * @tc.desc    Function test
    */
    it('Telephony_SmsMms_sendMessage_0800', 0, async function (done) {
        const count = 161;
        let str = '';
        for (let index = 0; index < count; index++) {
            str+='a';
        }
        sms.sendMessage({
            slotId: 1,
            destinationHost: '18211305277',
            content: str,
            sendCallback: (err, value) => {
                if (err) {
                    expect().assertFail();
                    return;
                }
                console.log("sendCallback success sendResult = " + value.result);
                expect(value.result).assertEquals(sms.SEND_SMS_SUCCESS);
            },
            deliveryCallback: (err, value) => {
                if (err) {
                    expect().assertFail();
                    return;
                }
                console.log("deliveryCallback success sendResult = " + value.pdu);
                expect(value.pdu).assertTrue();
            }
        })
        done()
    })

    /*
    * @tc.number  Telephony_SmsMms_sendMessage_0900
    * @tc.name    Call the interface SendMessage,
    *             set the content "Content" as the content of Chinese character type,
    *             and send a short message successfully
    * @tc.desc    Function test
    */
    it('Telephony_SmsMms_sendMessage_0900', 0, async function (done) {
        sms.sendMessage({
            slotId: 1,
            destinationHost: '18211305277',
            content: '这是测试文本',
            sendCallback: (err, value) => {
                if (err) {
                    expect().assertFail();
                    return;
                }
                console.log("sendCallback success sendResult = " + value.result);
                expect(value.result).assertEquals(sms.SEND_SMS_SUCCESS);
            },
            deliveryCallback: (err, value) => {
                if (err) {
                    expect().assertFail();
                    return;
                }
                console.log("deliveryCallback success sendResult = " + value.pdu);
                expect(value.pdu).assertTrue();
            }
        })
        done()
    })

    /*
    * @tc.number  Telephony_SmsMms_sendMessage_1000
    * @tc.name    Call the interface sendMessage,
    *             set the content "content" to a single byte character (English alphabet or number) type of content,
    *             send a short message successfully
    * @tc.desc    Function test
    */
    it('Telephony_SmsMms_sendMessage_1000', 0, async function (done) {
        sms.sendMessage({
            slotId: 1,
            destinationHost: '18211305277',
            content: 'qwe123',
            sendCallback: (err, value) => {
                if (err) {
                    expect().assertFail();
                    return;
                }
                console.log("sendCallback success sendResult = " + value.result);
                expect(value.result).assertEquals(sms.SEND_SMS_SUCCESS);
            },
            deliveryCallback: (err, value) => {
                if (err) {
                    expect().assertFail();
                    return;
                }
                console.log("deliveryCallback success sendResult = " + value.pdu);
                expect(value.pdu).assertTrue();
            }
        })
        done()
    })

    /*
    * @tc.number  Telephony_SmsMms_sendMessage_1100
    * @tc.name    Call the interface sendMessage,
    *             set the content "content" to the content of special character type,
    *             and send the short message successfully
    * @tc.desc    Function test
    */
    it('Telephony_SmsMms_sendMessage_1100', 0, async function (done) {
        sms.sendMessage({
            slotId: 1,
            destinationHost: '18211305277',
            content: 'ㄅㄆ$￡á ǎ㊊↑◎┴%@&*^#',
            sendCallback: (err, value) => {
                if (err) {
                    expect().assertFail();
                    return;
                }
                console.log("sendCallback success sendResult = " + value.result);
                expect(value.result).assertEquals(sms.SEND_SMS_SUCCESS);
            },
            deliveryCallback: (err, value) => {
                if (err) {
                    expect().assertFail();
                    return;
                }
                console.log("deliveryCallback success sendResult = " + value.pdu);
                expect(value.pdu).assertTrue();
            }
        })
        done()
    })

    /*
    * @tc.number  Telephony_SmsMms_sendMessage_1200
    * @tc.name    Call the interface SendMessage.
    *             If the "content" is of data type, set the destinationPort "DestinationPort" as 0x00,
    *             and SMS is sent successfully
    * @tc.desc    Function test
    */
    it('Telephony_SmsMms_sendMessage_1200', 0, async function (done) {
        sms.sendMessage({
            slotId: 1,
            destinationHost: '18211305277',
            content: new Uint8Array([54, 2, 3, 6, 3, 1, 1]),
            destinationPort: 0x00,
            sendCallback: (err, value) => {
                if (err) {
                    console.log("rawdata msg sendCallback err");
                    expect().assertFail();
                    return;
                }
                console.log("rawdata msg sendCallback success sendResult = " + value.result);
                expect(value.result).assertEquals(sms.SEND_SMS_SUCCESS);
            },
            deliveryCallback: (err, value) => {
                if (err) {
                    console.log("deliveryCallback err");
                    expect().assertFail();
                    return;
                }
                console.log("deliveryCallback success sendResult = " + value.pdu);
                expect(value.pdu).assertTrue();
            }
        })
        done()
    })

    /*
    * @tc.number  Telephony_SmsMms_sendMessage_1300
    * @tc.name    Call the interface SendMessage.
    *             If the "content" is of data type, set the destinationPort "DestinationPort" as 0xffff,
    *             and SMS is sent successfully
    * @tc.desc    Function test
    */
    it('Telephony_SmsMms_sendMessage_1300', 0, async function (done) {
        sms.sendMessage({
            slotId: 1,
            destinationHost: '18211305277',
            content: new Uint8Array([54, 2, 3, 6, 3, 1, 1]),
            destinationPort: 0xffff,
            sendCallback: (err, value) => {
                if (err) {
                    console.log("rawdata msg sendCallback err");
                    expect().assertFail();
                    return;
                }
                console.log("rawdata msg sendCallback success sendResult = " + value.result);
                expect(value.result).assertEquals(sms.SEND_SMS_SUCCESS);
            },
            deliveryCallback: (err, value) => {
                if (err) {
                    console.log("deliveryCallback err");
                    expect().assertFail();
                    return;
                }
                console.log("deliveryCallback success sendResult = " + value.pdu);
                expect(value.pdu).assertTrue();
            }
        })
        done()
    })

    /*
    * @tc.number  Telephony_SmsMms_sendMessage_1400
    * @tc.name    Call the interface SendMessage.
    *             If the "content" is of data type, set the destinationPort "DestinationPort" as 0xffff-1,
    *             and SMS is sent successfully
    * @tc.desc    Function test
    */
    it('Telephony_SmsMms_sendMessage_1400', 0, async function (done) {
        sms.sendMessage({
            slotId: 1,
            destinationHost: '18211305277',
            content: new Uint8Array([54, 2, 3, 6, 3, 1, 1]),
            destinationPort: 0xffff-1,
            sendCallback: (err, value) => {
                if (err) {
                    console.log("rawdata msg sendCallback err");
                    expect().assertFail();
                    return;
                }
                console.log("rawdata msg sendCallback success sendResult = " + value.result);
                expect(value.result).assertEquals(sms.SEND_SMS_SUCCESS);
            },
            deliveryCallback: (err, value) => {
                if (err) {
                    console.log("deliveryCallback err");
                    expect().assertFail();
                    return;
                }
                console.log("deliveryCallback success sendResult = " + value.pdu);
                expect(value.pdu).assertTrue();
            }
        })
        done()
    })

    /*
    * @tc.number  Telephony_SmsMms_sendMessage_1500
    * @tc.name    Call the interface SendMessage.
    *             If the "content" is of data type, set the destinationPort "DestinationPort" as 0xffff-1,
    *             and SMS is sent successfully
    * @tc.desc    Function test
    */
    it('Telephony_SmsMms_sendMessage_1500', 0, async function (done) {
        sms.sendMessage({
            slotId: 1,
            destinationHost: '18211305277',
            content: new Uint8Array([54, 2, 3, 6, 3, 1, 1]),
            destinationPort: 0xffff-1,
            sendCallback: (err, value) => {
                if (err) {
                    console.log("rawdata msg sendCallback err");
                    expect().assertFail();
                    return;
                }
                console.log("rawdata msg sendCallback success sendResult = " + value.result);
                expect(value.result).assertEquals(sms.SEND_SMS_SUCCESS);
            },
            deliveryCallback: (err, value) => {
                if (err) {
                    console.log("deliveryCallback err");
                    expect().assertFail();
                    return;
                }
                console.log("deliveryCallback success sendResult = " + value.pdu);
                expect(value.pdu).assertTrue();
            }
        })
        done()
    })

    /*
    * @tc.number  Telephony_SmsMms_sendMessage_1600
    * @tc.name    The loop calls the interface SendMessage1000 times,
    *             and the message is sent successfully each time
    * @tc.desc    Function test
    */
    it('Telephony_SmsMms_sendMessage_1600', 0, async function (done) {
        for(let index = 0; index < 1000; index++)
        {
            sms.sendMessage({
                slotId: 1,
                destinationHost: '18211305277',
                content: 'hello',
                sendCallback: (err, value) => {
                    if (err) {
                        expect().assertFail();
                        return;
                    }
                    console.log("sendCallback success sendResult = " + value.result);
                    expect(value.result).assertEquals(sms.SEND_SMS_SUCCESS);
                },
                deliveryCallback: (err, value) => {
                    if (err) {
                        expect().assertFail();
                        return;
                    }
                    console.log("deliveryCallback success sendResult = " + value.pdu);
                    expect(value.pdu).assertTrue();
                }
            })
        }
        done()
    })

    /*
    * @tc.number  Telephony_SmsMms_sendMessage_1700
    * @tc.name    The loop calls the interface SendMessage1000 times,
    *             Delay < 500ms
    * @tc.desc    Function test
    */
    it('Telephony_SmsMms_sendMessage_1700', 0, async function (done) {
        const startTime = (new Date).getTime();
        for(let index = 0; index < 1000; index++)
        {
            sms.sendMessage({
                slotId: 1,
                destinationHost: '18211305277',
                content: 'hello',
                sendCallback: (err, value) => {
                    if (err) {
                        expect().assertFail();
                        return;
                    }
                },
                deliveryCallback: (err, value) => {
                    if (err) {
                        expect().assertFail();
                        return;
                    }
                }
            })
        }
        const endTime = (new Date).getTime();
        const bool = (endTime - startTime) < 500;
        expect(bool).assertTrue();
        done()
    })

    /*
    * @tc.number  Telephony_SmsMms_createMessage_Async_0100
    * @tc.name    Call interface CreateMessage,
    *             pass in the PDU in line with the coding specification, the specification is 3GPP,
    *             shortMessage Don't empty
    * @tc.desc    Function test
    */
    it('Telephony_SmsMms_createMessage_Async_0100', 0, async function (done) {
        sms.createMessage(rawArray, "3gpp", (err, shortMessage) => {
            if (err) {
                expect().assertFail();
                return;
            }
            const bool = (shortMessage != null);
            expect(bool).assertTrue();
        })
        done()
    })

    /*
    * @tc.number  Telephony_SmsMms_createMessage_Async_0200
    * @tc.name    Call interface CreateMessage,
    *             pass in the PDU in line with the coding specification, the specification is 3GPP2,
    *             shortMessage Don't empty
    * @tc.desc    Function test
    */
    it('Telephony_SmsMms_createMessage_Async_0200', 0, async function (done) {
        sms.createMessage(rawArray, "3gpp2", (err, shortMessage) => {
            if (err) {
                expect().assertFail();
                return;
            }
            const bool = (shortMessage != null);
            expect(bool).assertTrue();
        })
        done()
    })

    /*
    * @tc.number  Telephony_SmsMms_createMessage_Async_0300
    * @tc.name    Call interface CreateMessage,
    *             The incoming PDU is empty, the specification is 3GPP,
    *             shortMessage isn't empty
    * @tc.desc    Function test
    */
    it('Telephony_SmsMms_createMessage_Async_0300', 0, async function (done) {
        sms.createMessage(rawArrayNull, "3gpp", (err, shortMessage) => {
            if (err) {
                expect().assertFail();
                return;
            }
            expect(shortMessage!= null && shortMessage!= undefined).assertTrue();
        })
        done()
    })

    /*
    * @tc.number  Telephony_SmsMms_createMessage_Async_0400
    * @tc.name    Call interface CreateMessage,
    *             The incoming PDU is empty, the specification is 3GPP2,
    *             shortMessage isn't empty
    * @tc.desc    Function test
    */
    it('Telephony_SmsMms_createMessage_Async_0400', 0, async function (done) {
        sms.createMessage(rawArrayNull, "3gpp2", (err, shortMessage) => {
            if (err) {
                expect().assertFail();
                return;
            }
            expect(shortMessage!= null && shortMessage!= undefined).assertTrue();
        })
        done()
    })

    /*
    * @tc.number  Telephony_SmsMms_createMessage_Async_0500
    * @tc.name    Loop through the createMessage1000 times
    * @tc.desc    Function test
    */
    it('Telephony_SmsMms_createMessage_Async_0500', 0, async function (done) {
        for(let index = 0; index < 1000; index++)
        {
            sms.createMessage(rawArray, "3gpp", (err, shortMessage) => {
                if (err) {
                    expect().assertFail();
                    return;
                }
                const bool = (shortMessage != null);
                expect(bool).assertTrue();
            })
        }
        done()
    })

    /*
    * @tc.number  Telephony_SmsMms_createMessage_Async_0600
    * @tc.name    Loop through the createMessage1000 times,
    *             Time delay < 500ms
    * @tc.desc    Function test
    */
    it('Telephony_SmsMms_createMessage_Async_0600', 0, async function (done) {
        const startTime = (new Date).getTime();
        for(let index = 0; index < 1000; index++)
        {
            sms.createMessage(rawArray, "3gpp", (err, shortMessage) => {
                if (err) {
                    expect().assertFail();
                    return;
                }
            })
        }
        const endTime = (new Date).getTime();
        const bool = (endTime - startTime) < 500;
        expect(bool).assertTrue();
        done()
    })

    /*
    * @tc.number  Telephony_SmsMms_createMessage_Promise_0100
    * @tc.name    Call interface CreateMessage,
    *             pass in the PDU in line with the coding specification, the specification is 3GPP,
    *             promise returns the result Don't empty
    * @tc.desc    Function test
    */
    it('Telephony_SmsMms_createMessage_Promise_0100', 0, async function (done) {
        var promise = sms.createMessage(rawArray, '3gpp');
        promise.then((shortMessage) => {
            const bool = (shortMessage != null);
            expect(bool).assertTrue();
        }).catch((err) => {
            expect().assertFail();
            return;
        })
        done()
    })

    /*
    * @tc.number  Telephony_SmsMms_createMessage_Promise_0200
    * @tc.name    Call interface CreateMessage,
    *             pass in the PDU in line with the coding specification, the specification is 3GPP2,
    *             promise returns the result Don't empty
    * @tc.desc    Function test
    */
    it('Telephony_SmsMms_createMessage_Promise_0200', 0, async function (done) {
        var promise = sms.createMessage(rawArray, '3gpp2');
        promise.then((shortMessage) => {
            const bool = (shortMessage != null);
            expect(bool).assertTrue();
        }).catch((err) => {
            expect().assertFail();
            return;
        })
    })

    /*
    * @tc.number  Telephony_SmsMms_createMessage_Promise_0300
    * @tc.name    Call interface CreateMessage,
    *             The incoming PDU is empty, the specification is 3GPP,
    *             promise returns the result Don't empty
    * @tc.desc    Function test
    */
    it('Telephony_SmsMms_createMessage_Promise_0300', 0, async function (done) {
        var promise = sms.createMessage(rawArrayNull, '3gpp');
        promise.then((shortMessage) => {
            expect(shortMessage!= null && shortMessage!= undefined).assertTrue();
        }).catch((err) => {
            expect().assertFail();
            return;
        })
        done()
    })

    /*
    * @tc.number  Telephony_SmsMms_createMessage_Async_0400
    * @tc.name    Call interface CreateMessage,
    *             The incoming PDU is empty, the specification is 3GPP2,
    *             promise returns the result Don't empty
    * @tc.desc    Function test
    */
    it('Telephony_SmsMms_createMessage_Async_0400', 0, async function (done) {
        var promise = sms.createMessage(rawArrayNull, '3gpp2');
        promise.then((shortMessage) => {
            expect(shortMessage!= null && shortMessage!= undefined).assertTrue();
        }).catch((err) => {
            expect().assertFail();
            return;
        })
        done()
    })

    /*
    * @tc.number  Telephony_SmsMms_createMessage_Async_0500
    * @tc.name    Loop through the createMessage1000 times promise
    * @tc.desc    Function test
    */
    it('Telephony_SmsMms_createMessage_Async_0500', 0, async function (done) {
        for(let index = 0; index < 1000; index++)
        {
            var promise = sms.createMessage(rawArray, '3gpp');
            promise.then((shortMessage) => {
                const bool = (shortMessage != null);
                expect(bool).assertTrue();
            }).catch((err) => {
                expect().assertFail();
                return;
            })
        }
        done()
    })

    /*
    * @tc.number  Telephony_SmsMms_createMessage_Async_0600
    * @tc.name    Loop through the createMessage1000 times promise,
    *             Time delay < 500ms
    * @tc.desc    Function test
    */
    it('Telephony_SmsMms_createMessage_Async_0600', 0, async function (done) {
        const startTime = (new Date).getTime();
        for(let index = 0; index < 1000; index++)
        {
            var promise = sms.createMessage(rawArray, '3gpp');
            promise.then((shortMessage) => {
            }).catch((err) => {
                expect().assertFail();
                return;
            })
        }
        const endTime = (new Date).getTime();
        const bool = (endTime - startTime) < 500;
        expect(bool).assertTrue();
        done()
    })
})
