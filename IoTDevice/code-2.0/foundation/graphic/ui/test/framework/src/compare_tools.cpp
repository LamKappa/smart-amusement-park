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

#include "compare_tools.h"

#include <cstring>
#include "draw/draw_utils.h"
#include "gfx_utils/file.h"
#include "gfx_utils/graphic_log.h"
#include "graphic_config.h"
#include "securec.h"

namespace OHOS {
bool CompareTools::enableLog_ = false;
char* CompareTools::logPath_ = nullptr;

void CompareTools::WaitSuspend()
{
#ifdef _WIN32
    Sleep(DEFAULT_WAIT_TIME_MS);
#else
    usleep(1000 * DEFAULT_WAIT_TIME_MS); // 1000: us to ms
#endif // _WIN32
}

bool CompareTools::StrnCatPath(char* filePath, size_t pathMax, const char* fileName, size_t count)
{
    if ((filePath == nullptr) || (pathMax > DEFAULT_FILE_NAME_MAX_LENGTH)) {
        return false;
    }
    char dest[DEFAULT_FILE_NAME_MAX_LENGTH] = UI_AUTO_TEST_RESOURCE_PATH;
    if (strncat_s(dest, DEFAULT_FILE_NAME_MAX_LENGTH, fileName, count) != EOK) {
        return false;
    }
    if (memcpy_s(static_cast<void*>(filePath), pathMax, dest, DEFAULT_FILE_NAME_MAX_LENGTH) != EOK) {
        return false;
    }
    return true;
}

bool CompareTools::CompareFile(const char* src, size_t length, uint8_t flag)
{
    switch (flag) {
        case COMPARE_BINARY:
            return CompareBinary(src, length);
        case COMPARE_IMAGE:
            // Unrealized : image for comparison
            break;
        default:
            break;
    }
    return false;
}

bool CompareTools::SaveFile(const char* src, size_t length, uint8_t flag)
{
    switch (flag) {
        case COMPARE_BINARY:
            return SaveFrameBuffToBinary(src, length);
        case COMPARE_IMAGE:
            // Unrealized : save frame buff as image
            break;
        default:
            break;
    }
    return false;
}

bool CompareTools::CompareBinary(const char* filePath, size_t length)
{
    if ((filePath == nullptr) || (length > DEFAULT_FILE_NAME_MAX_LENGTH)) {
        return false;
    }
    BufferInfo* bufferInfo = BaseGfxEngine::GetInstance()->GetFBBufferInfo();
    if (bufferInfo == nullptr) {
        return false;
    }
    uint8_t* frameBuf = static_cast<uint8_t*>(bufferInfo->virAddr);
    if (frameBuf == nullptr) {
        return false;
    }
    uint8_t sizeByColorMode = DrawUtils::GetByteSizeByColorMode(bufferInfo->mode);
    uint32_t buffSize = HORIZONTAL_RESOLUTION * VERTICAL_RESOLUTION * sizeByColorMode;
    uint8_t* readBuf = new uint8_t[buffSize];
    if (readBuf == nullptr) {
        return false;
    }
    FILE* fd = fopen(filePath, "rb");
    if (fd == nullptr) {
        delete[] readBuf;
        return false;
    }
    if (fread(readBuf, sizeof(uint8_t), buffSize, fd) < 0) {
        delete[] readBuf;
        fclose(fd);
        return false;
    }
    bool ret = true;
    for (int32_t i = 0; i < (buffSize / sizeof(uint8_t)); i++) {
        if (readBuf[i] != frameBuf[i]) {
            ret = false;
            break;
        }
    }
    if (ret) {
        GRAPHIC_LOGI("[SUCCESS]:fileName=%s", filePath);
    } else {
        GRAPHIC_LOGI("[FAILURE]:fileName=%s", filePath);
    }
    delete[] readBuf;
    fclose(fd);
    if (enableLog_) {
        char logBuf[DEFAULT_FILE_NAME_MAX_LENGTH] = {0};
        if (ret) {
            if (sprintf_s(logBuf, DEFAULT_FILE_NAME_MAX_LENGTH, "[SUCCESS]:fileName=%s\n", filePath) < 0) {
                return false;
            }
        } else {
            if (sprintf_s(logBuf, DEFAULT_FILE_NAME_MAX_LENGTH, "[FAILURE]:fileName=%s\n", filePath) < 0) {
                return false;
            }
        }
        SaveLog(logBuf, strlen(logBuf));
    }
    return ret;
}

bool CompareTools::SaveFrameBuffToBinary(const char* filePath, size_t length)
{
    if ((filePath == nullptr) || (length > DEFAULT_FILE_NAME_MAX_LENGTH)) {
        return false;
    }
    BufferInfo* bufferInfo = BaseGfxEngine::GetInstance()->GetFBBufferInfo();
    if (bufferInfo == nullptr) {
        return false;
    }
    uint8_t* frameBuf = static_cast<uint8_t*>(bufferInfo->virAddr);
    if (frameBuf == nullptr) {
        GRAPHIC_LOGE("GetBuffer failed");
        return false;
    }
    uint8_t sizeByColorMode = DrawUtils::GetByteSizeByColorMode(bufferInfo->mode);
    uint32_t buffSize = HORIZONTAL_RESOLUTION * VERTICAL_RESOLUTION * sizeByColorMode;
    FILE* fd = fopen(filePath, "wb+");
    if (fd == nullptr) {
        return false;
    }
    if (fwrite(frameBuf, sizeof(uint8_t), buffSize, fd) < 0) {
        fclose(fd);
        return false;
    }
    fclose(fd);
    GRAPHIC_LOGI("[SAVEBIN]:fileName=%s", filePath);
    if (enableLog_) {
        char logBuf[DEFAULT_FILE_NAME_MAX_LENGTH] = {0};
        if (sprintf_s(logBuf, DEFAULT_FILE_NAME_MAX_LENGTH, "[SAVEBIN]:fileName=%s\n", filePath) < 0) {
            return false;
        }
        SaveLog(logBuf, strlen(logBuf));
    }
    return true;
}

bool CompareTools::CheckFileExist(const char* filePath, size_t length)
{
    if ((filePath == nullptr) || (length > DEFAULT_FILE_NAME_MAX_LENGTH)) {
        return false;
    }
    FILE* fd = fopen(filePath, "r");
    if (fd == nullptr) {
        return false;
    }
    fclose(fd);
    return true;
}

void CompareTools::SetLogPath(const char* filePath, size_t length)
{
    if (logPath_ == nullptr) {
        logPath_ = new char[length];
        if (logPath_ == nullptr) {
            return;
        }

        if (memcpy_s(logPath_, length, filePath, length) != EOK) {
            GRAPHIC_LOGE("memcpy filepath failed");
            return;
        }
        enableLog_ = true;
    }
}

void CompareTools::UnsetLogPath()
{
    if (logPath_ != nullptr) {
        delete[] logPath_;
        logPath_ = nullptr;
        enableLog_ = false;
    }
}

bool CompareTools::SaveLog(const char* buff, size_t bufSize)
{
    if ((buff == nullptr) || (logPath_ == nullptr)) {
        return false;
    }
    FILE* log = fopen(logPath_, "a");
    if (log == nullptr) {
        GRAPHIC_LOGE("open log failed");
        return false;
    }
    if (fwrite(buff, 1, bufSize, log) < 0) {
        fclose(log);
        GRAPHIC_LOGE("write log failed");
        return false;
    }
    fclose(log);
    return true;
}
} // namespace OHOS
