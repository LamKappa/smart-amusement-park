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
#ifndef UPDATER_CONST_H
#define UPDATER_CONST_H

#include <string>

namespace updater {
const std::string COMMAND_FILE = "/data/updater/command";
const std::string TMP_LOG = "/tmp/updater.log";
const std::string TMP_STAGE_LOG = "/tmp/updater_stage.log";
const std::string TMP_ERROR_CODE_PATH = "/tmp/error_code.log";
const std::string ERROR_CODE_PATH = "/data/updater/log/error_code.log";
const std::string UPDATER_LOG_DIR = "/data/updater/log";
const std::string UPDATER_LOG = "/data/updater/log/updater_log";
const std::string UPDATER_STAGE_LOG = "/data/updater/log/updater_stage_log";
const std::string UPDATER_PATH = "/data/updater";
const std::string MISC_FILE = "/dev/block/platform/soc/10100000.himci.eMMC/by-name/misc";
const std::string UPDATER_BINARY = "updater_binary";
#ifndef UPDATER_UT
const std::string SDCARD_CARD_PATH = "/sdcard/updater";
const std::string SDCARD_CARD_PKG_PATH = "/sdcard/updater/updater.zip";
const std::string DEFAULT_LOCALE = "en-US";
const std::string G_WORK_PATH = "/tmp/";
#else
const std::string SDCARD_CARD_PATH = "/data/sdcard/updater";
const std::string SDCARD_CARD_PKG_PATH = "/data/updater/updater/updater.zip";
const std::string G_WORK_PATH = "/data/updater/src/";
#endif
constexpr int MAX_RETRY_COUNT = 4;
constexpr int MINIMAL_ARGC_LIMIT = 2;
constexpr int MAX_LOG_BUF_SIZE = 4096;
constexpr int MAX_LOG_NAME_SIZE = 100;
constexpr long MAX_LOG_SIZE = 5 * 1024 * 1024;
constexpr int MAX_TIME_SIZE = 20;
constexpr int VERIFICATION_PROGRESS_TIME = 60;
constexpr float VERIFICATION_PROGRESS_FRACTION = 0.25f;
constexpr int PREDICTED_ELAPSED_TIME = 30;
constexpr int PROGRESS_VALUE_MAX = 1;
constexpr int PROGRESS_VALUE_MIN = 0;
constexpr int SLEEP_TIME = 1;
constexpr int DECIMAL = 10;
constexpr int BINARY = 2;
constexpr int LEAST_PARTITION_COUNT = 4;
constexpr int FAKE_TEMPRATURE = 40;
constexpr int32_t DEFAULT_PROCESS_NUM = 2;
constexpr int32_t MAX_BUFFER_SIZE = 512;
constexpr int32_t DEFAULT_PIPE_NUM = 2;
constexpr int32_t BINARY_MAX_ARGS = 3;
constexpr int32_t BINARY_SECOND_ARG = 2;
constexpr int32_t WRITE_SECOND_CMD = 2;
constexpr int REBOOT = 0;
constexpr int WIPE_DATA = 1;
constexpr int WIPE_CACHE = 2;
constexpr int FACTORY_RESET = 3;
constexpr int REBOOT_FASTBOOT = 4;
constexpr int UPDATE_FROM_SDCARD = 5;
constexpr int SHUTDOWN = 6;
constexpr int FULL_PERCENT_PROGRESS = 100;
constexpr int PROGRESS_VALUE_CONST = 2;
constexpr int SHOW_FULL_PROGRESS_TIME = 2000;
constexpr unsigned int UI_SHOW_DURATION = 2000;
} // namespace updater
#endif
