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
#ifndef _SCRIPT_MANAGER_H
#define _SCRIPT_MANAGER_H

#include <cstdlib>
#include <memory>
#include "pkg_manager.h"
#include "script_instruction.h"

namespace uscript {
enum {
    USCRIPT_SUCCESS = 0,
    USCRIPT_BASE = 500,
    USCRIPT_INVALID_PARAM = USCRIPT_BASE,
    USCRIPT_INVALID_SCRIPT,
    USCRIPT_INVALID_PRIORITY,
    USCRIPT_NOTEXIST_INSTRUCTION,
    USCRIPT_ERROR_CREATE_OBJ,
    USCRIPT_ERROR_REVERED,
    USCRIPT_ASSERT,
    USCRIPT_ABOART,
    USCRIPT_ERROR_EXECUTE,
    USCRIPT_ERROR_INTERPRET,
};

/**
 * Script Manager
 * 1, Management of script instruction and instruction registration
 * 2, Management of parsing and executing the script
 **/
class ScriptManager {
public:
    static const int32_t MAX_PRIORITY = 4;

    virtual ~ScriptManager() = default;

    /**
     * Execute all script under this priority
     * priority:        The priority of the script to run
     */
    virtual int32_t ExecuteScript(int32_t priority) = 0;

    /**
     * Get script manager
     */
    static ScriptManager* GetScriptManager(UScriptEnv *env);
    static void ReleaseScriptManager();
};
} // namespace uscript
#endif
