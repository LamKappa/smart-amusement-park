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

#ifndef GRAPHIC_LITE_COMPARE_TOOLS_H
#define GRAPHIC_LITE_COMPARE_TOOLS_H

#include "graphic_config.h"
#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif // _WIN32

#ifdef _WIN32
#define UI_AUTO_TEST_RESOURCE_PATH  "..\\simulator\\config\\auto_test\\"
#else
#define UI_AUTO_TEST_RESOURCE_PATH  (RESOURCE_DIR "auto_test/")
#endif

namespace OHOS {
namespace {
    constexpr uint16_t DEFAULT_WAIT_TIME_MS = 300;
    constexpr size_t DEFAULT_FILE_NAME_MAX_LENGTH = 256;
}
class CompareTools {
public:
    enum CompareMode : uint8_t {
        COMPARE_BINARY,
        COMPARE_IMAGE
    };

    static void WaitSuspend();
    static bool StrnCatPath(char* filePath, size_t pathMax, const char* fileName, size_t count);
    static bool CompareFile(const char* filePath, size_t length, uint8_t flag);
    static bool SaveFile(const char* filePath, size_t length, uint8_t flag);
    static bool CheckFileExist(const char* filePath, size_t length);
    static void SetLogPath(const char* filePath, size_t length);
    static void UnsetLogPath();

private:
    CompareTools() {}
    virtual ~CompareTools() {}

    static bool SaveLog(const char* buff, size_t bufSize);
    static bool CompareBinary(const char* filePath, size_t length);
    static bool SaveFrameBuffToBinary(const char* filePath, size_t length);

    static bool enableLog_;
    static char* logPath_;
};
} // namespace OHOS
#endif // GRAPHIC_LITE_COMPARE_TOOLS_H
