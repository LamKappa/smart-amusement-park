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
#ifndef UPDATE_LOG_H__
#define UPDATE_LOG_H__

#include <cstring>
#include <iostream>
#include <fstream>
#include <string>
#include "error_code.h"

namespace updater {
constexpr size_t MAX_LOG_SPACE = 4 * 5 * 1024 * 1024;
#define __FILE_NAME__   (strrchr((__FILE__), '/') ? strrchr((__FILE__), '/') + 1 : (__FILE__))
#define LOG(level) UpdaterLogger(level).OutputUpdaterLog((__FILE_NAME__), (__LINE__))
#define STAGE(stage) StageLogger(stage).OutputUpdaterStage()
#define ERROR_CODE(code) ErrorCode(code).OutputErrorCode((__FILE_NAME__), (__LINE__), (code))

#define UPDATER_ERROR_CHECK(ret, log, statement)  \
    if (!(ret)) {                                 \
        LOG(ERROR) << log;                        \
        statement;                                \
    }

#define UPDATER_WARING_CHECK(ret, log, statement)  \
    if (!(ret)) {                                  \
        LOG(WARNING) << log;                       \
        statement;                                 \
    }

#define UPDATER_INFO_CHECK(ret, log, statement)  \
    if (!(ret)) {                                  \
        LOG(INFO) << log;                       \
        statement;                                 \
    }

#define UPDATER_ERROR_CHECK_NOT_RETURN(ret, log)   \
    if (!(ret)) {                                  \
        LOG(ERROR) << log;                         \
    }

#define UPDATER_INFO_CHECK_NOT_RETURN(ret, log)   \
    if (!(ret)) {                                  \
        LOG(INFO) << log;                         \
    }

#define UPDATER_WARNING_CHECK_NOT_RETURN(ret, log)   \
    if (!(ret)) {                                  \
        LOG(WARNING) << log;                         \
    }

#define UPDATER_FILE_CHECK(ret, log, statement)         \
    if (!(ret)) {                                       \
        LOG(ERROR) << log << " : " << strerror(errno);  \
        statement;                                      \
    }

#define UPDATER_CHECK_FILE_OP(ret, log, fd, statement)  \
    if (!(ret)) {                                       \
        LOG(ERROR) << log << " : " << strerror(errno);  \
        close(fd);                                      \
        statement;                                      \
    }

#define UPDATER_POST_STAGE_ERROR_CHECK(ret, log, statement)  \
    if (!(ret)) {                                            \
        LOG(ERROR) << log;                                   \
        STAGE(UPDATE_STAGE_FAIL) << "PostUpdater";           \
        statement;                                           \
    }

#define UPDATER_CHECK_ONLY_RETURN(ret, statement)         \
    if (!(ret)) {                                       \
        statement;                                      \
    }

enum {
    VERBOSE,
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    FATAL,
};

enum {
    UPDATE_STAGE_BEGIN,
    UPDATE_STAGE_SUCCESS,
    UPDATE_STAGE_FAIL,
    UPDATE_STAGE_OUT,
};

void SetLogLevel(int level);

void InitUpdaterLogger(const std::string &tag, const std::string &logFile, const std::string &stageFile,
    const std::string &errorCodeFile);

void Logger(int level, const char* fileName, int32_t line, const char* format, ...);

class UpdaterLogger {
public:
    UpdaterLogger(int level) : level_(level) {}

    ~UpdaterLogger();

    std::ostream& OutputUpdaterLog(const std::string &path, int line);
private:
    int level_;
};

class StageLogger {
public:
    StageLogger(int stage) : stage_(stage) { }

    ~StageLogger();

    std::ostream& OutputUpdaterStage();
private:
    int stage_;
};

class ErrorCode {
public:
    ErrorCode(enum UpdaterErrorCode level)     {}

    ~ErrorCode() {};

    std::ostream& OutputErrorCode(const std::string &path, int line, UpdaterErrorCode code);
};
} // updater
#endif /* UPDATE_LOG_H__ */
