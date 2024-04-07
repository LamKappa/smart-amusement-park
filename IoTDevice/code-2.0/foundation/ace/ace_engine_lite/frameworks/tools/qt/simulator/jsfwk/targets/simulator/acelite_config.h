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
 
#ifndef OHOS_ACELITE_CONFIG_H
#define OHOS_ACELITE_CONFIG_H

/**
 * ================================================================================================
 *                 config for win simulator
 * ================================================================================================
 */
#define OHOS_ACELITE_SIMULATOR_WIN // NOTE: DO NOT use this macro directly

#ifndef JSFWK_TEST
#define JSFWK_TEST
#endif

/**
 * The macro is used to distingush the real device and simulator.
 */
#ifndef TARGET_SIMULATOR
#define TARGET_SIMULATOR
#endif

/**
 * Compile all test entry for hmf
 */
#ifdef FEATURE_TEST_IMPLEMENTATION
#error "must keep the global configuration unique"
#else
#define FEATURE_TEST_IMPLEMENTATION
#endif

#ifndef QT_SIMULATOR
/**
 * enable FeatureAbility API
 */
#define FEATURE_FEATURE_ABILITY_MODULE

/**
 * support device API for JS
 */
#define FEATURE_MODULE_DEVICE

/**
 * support geo location API for JS
 */
#define FEATURE_MODULE_GEO

/**
 * support sensor API for JS
 */
#define FEATURE_MODULE_SENSOR

/**
 * support brightness API for JS
 */
#define FEATURE_MODULE_BRIGHTNESS

/**
 * support battery API for JS
 */
#define FEATURE_MODULE_BATTERY

/**
 * support configuration API for JS
 */
#define FEATURE_MODULE_CONFIGURATION

/**
 * timer module
 */
#define FEATURE_TIMER_MODULE

/**
 * support storage API for JS
 */
#define FEATURE_MODULE_STORAGE


/**
 * localization module
 */
#define FEATURE_LOCALIZATION_MODULE
#endif

#define FEATURE_CUSTOM_ENTRY_PAGE

/**
 * module require test
 */
#ifdef ENABLE_MODULE_REQUIRE_TEST
#error "must keep the global configuration unique"
#else
#define ENABLE_MODULE_REQUIRE_TEST
#endif

/**
 * define the max length of user's console log
 */
#ifndef CONSOLE_LOG_LINE_MAX_LENGTH
#define CONSOLE_LOG_LINE_MAX_LENGTH (96)
#endif

/**
 * enable Canvas component Feature API on simulator
 */
#define FEATURE_COMPONENT_CANVAS

#ifdef FEATURE_COMPONENT_QRCODE
#error "must keep the global configuration unique"
#else
#define FEATURE_COMPONENT_QRCODE
#endif

#ifdef _WIN32
// support memory analysis, only on win
#ifndef QT_SIMULATOR
#define SIMULATOR_MEMORY_ANALYSIS
#endif
#endif


#ifdef _WIN32
#define PROFILER_ENABLE_FLAG_FILE "..\\profiler_enable"
#ifdef SIMULATOR_MEMORY_ANALYSIS
#define MEM_PROC_ENABLE_FLAG_FILE "..\\memproc_enable"
#define MEM_LOG_FILE_PATH "..\\mem.txt"
#define MEM_BACK_UP_LOG_FILE_PREFIX "..\\"
#endif // SIMULATOR_MEMORY_ANALYSIS
#else
// path format is different on linux or apple with windows
#define PROFILER_ENABLE_FLAG_FILE "~/profiler_enable"
#endif

#endif // OHOS_ACELITE_CONFIG_H
