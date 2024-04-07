/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef USCRIPT_UTILES_H
#define USCRIPT_UTILES_H

#include <cstring>
#include <iostream>
#include "log.h"
#include "script_manager.h"

namespace uscript {
enum {
    USCRIPT_INVALID_STATEMENT = USCRIPT_BASE + 100,
};

#define USCRIPT_LOGE(format, ...) Logger(updater::ERROR, (__FILE_NAME__), (__LINE__), format, ##__VA_ARGS__)
#define USCRIPT_LOGI(format, ...) Logger(updater::INFO, (__FILE_NAME__), (__LINE__), format, ##__VA_ARGS__)
#define USCRIPT_LOGW(format, ...) Logger(updater::WARNING, (__FILE_NAME__), (__LINE__), format, ##__VA_ARGS__)

#define USCRIPT_CHECK(ret, statement, ...) \
    if (!(ret)) {                          \
        USCRIPT_LOGE(__VA_ARGS__);         \
        statement;                         \
    }
} // namespace uscript
#endif /* USCRIPT_UTILES_H */
