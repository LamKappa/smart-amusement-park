/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

 /**
 * Provides the event logging function for applications to log the fault, statistical, security,
 * and user behavior events reported during running. Based on event information,
 * you will be able to analyze the running status of applications.
 *
 * @devices wearable
 * @since 6
 * @Syscap SystemCapability.Hiviewdfx.Hiappevent
 */
export class HiAppEvent {
    /**
     * write application event.
     *
     * @param eventName application event name.
     * @param eventType application event type.
     * @param keyValues application event params.
     * @return returns {@code 0} if the event parameter verification is successful and the application event is
     * written to the event file asynchronously; returns a non-0 value otherwise.
     *
     * @since 6
     */
    static write(eventName: string, eventType: number, keyValues: any[], callback: ()=>void): number;

    /**
     * write application event.
     *
     * @param eventName application event name.
     * @param eventType application event type.
     * @param keyValues application event params.
     * @return returns {@code 0} if the event parameter verification is successful and the application event is
     * written to the event file asynchronously; returns a non-0 value otherwise.
     *
     * @since 6
     */
    static writeJson(eventName: string, eventType: number, keyValues: object, callback: ()=>void): number;
}