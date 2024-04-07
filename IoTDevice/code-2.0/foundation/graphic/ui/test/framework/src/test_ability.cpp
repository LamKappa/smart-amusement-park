/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

#include "ui_test_app.h"
#ifdef _WIN32
#include <thread>
#else
#include <pthread.h>
#endif // _WIN32

#ifdef _WIN32
void AutoTestThread()
#else
void* AutoTestThread(void*)
#endif // _WIN32
{
    OHOS::UIAutoTestApp::GetInstance()->Start();
}


void RunApp()
{
    OHOS::UITestApp::GetInstance()->Start();
#if ENABEL_UI_AUTO_TEST
#ifdef _WIN32
    std::thread autoTestPthread(AutoTestThread);
    autoTestPthread.detach();
#else
    pthread_t thread;
    pthread_create(&thread, 0, AutoTestThread, nullptr);
    pthread_detach(thread);
#endif // _WIN32
#endif // ENABEL_UI_AUTO_TEST
}
