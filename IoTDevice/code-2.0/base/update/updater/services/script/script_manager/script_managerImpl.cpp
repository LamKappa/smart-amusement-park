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
#include "script_manager_impl.h"
#include <cstring>
#include <dlfcn.h>
#include "pkg_manager.h"
#include "script_instructionhelper.h"
#include "script_interpreter.h"
#include "script_utils.h"
#include "thread_pool.h"

using namespace hpackage;

namespace uscript {
const std::string LOAD_SCRIPT_NAME = "loadScript.us";
const std::string REGISTER_CMD_SCRIPT_NAME = "registerCmd.us";

static ScriptManagerImpl* g_scriptManager = nullptr;
ScriptManager* ScriptManager::GetScriptManager(UScriptEnv *env)
{
    USCRIPT_CHECK(env != nullptr, return nullptr, "Env null");

    if (g_scriptManager == nullptr) {
        g_scriptManager = new ScriptManagerImpl(env);
        (void)g_scriptManager->Init();
    }
    return g_scriptManager;
}

void ScriptManager::ReleaseScriptManager()
{
    if (g_scriptManager != nullptr) {
        delete g_scriptManager;
    }
    g_scriptManager = nullptr;
}

ScriptManagerImpl::~ScriptManagerImpl()
{
    if (threadPool_) {
        ThreadPool::Destroy();
        threadPool_ = nullptr;
    }
    for (int i = 0; i < MAX_PRIORITY; i++) {
        scriptFiles_[i].clear();
    }
    auto iter1 = scriptInstructions_.begin();
    while (iter1 != scriptInstructions_.end()) {
        UScriptInstructionPtr inst = (*iter1).second;
        delete inst;
        iter1 = scriptInstructions_.erase(iter1);
    }
    scriptInstructions_.clear();
    ScriptInstructionHelper::ReleaseBasicInstructionHelper();
}

int32_t ScriptManagerImpl::Init()
{
    USCRIPT_CHECK(scriptEnv_ != nullptr, return USCRIPT_INVALID_PARAM, "Env null");

    threadPool_ = ThreadPool::CreateThreadPool(MAX_PRIORITY);
    USCRIPT_CHECK(threadPool_ != nullptr, return USCRIPT_INVALID_PARAM, "Failed to create thread pool");

    // Register system reserved instructions
    ScriptInstructionHelper* helper = ScriptInstructionHelper::GetBasicInstructionHelper(this);
    USCRIPT_CHECK(helper != nullptr, return USCRIPT_INVALID_PARAM, "Failed to get helper");
    helper->RegisterInstructions();

    // Register customized instructions
    RegisterInstruction(*helper);

    PkgManager::PkgManagerPtr manager = scriptEnv_->GetPkgManager();
    USCRIPT_CHECK(manager != nullptr, return USCRIPT_INVALID_PARAM, "Failed to get pkg manager");

    // Register other instructions from scripts 
    int32_t ret = USCRIPT_SUCCESS;
    const FileInfo *info = manager->GetFileInfo(REGISTER_CMD_SCRIPT_NAME);
    if (info != nullptr) {
        ret = ExtractAndExecuteScript(manager, REGISTER_CMD_SCRIPT_NAME);
    }
    // Collect scripts
    ret |= ExtractAndExecuteScript(manager, LOAD_SCRIPT_NAME);
    USCRIPT_CHECK(ret == USCRIPT_SUCCESS, return ret, "Failed to extract and execute script ");
    return USCRIPT_SUCCESS;
}

int32_t ScriptManagerImpl::RegisterInstruction(ScriptInstructionHelper &helper)
{
    uscript::UScriptInstructionFactoryPtr factory = scriptEnv_->GetInstructionFactory();
    USCRIPT_CHECK(factory != nullptr, return USCRIPT_SUCCESS, "None factory");

    for (auto instrName : scriptEnv_->GetInstructionNames()) {
        // Create instructions and register it.
        UScriptInstructionPtr instr = nullptr;
        int32_t ret = factory->CreateInstructionInstance(instr, instrName);
        USCRIPT_CHECK(ret == USCRIPT_SUCCESS, return ret, "Failed to create instruction for %s", instrName.c_str());
        helper.AddInstruction(instrName, instr);
        USCRIPT_CHECK(ret == USCRIPT_SUCCESS, return ret, "Failed to add instruction for %s", instrName.c_str());
    }
    return USCRIPT_SUCCESS;
}

int32_t ScriptManagerImpl::ExtractAndExecuteScript(PkgManager::PkgManagerPtr manager,
    const std::string &scriptName)
{
    PkgManager::StreamPtr outStream = nullptr;
    int32_t ret = manager->CreatePkgStream(outStream, scriptName, 0, PkgStream::PkgStreamType_Write);
    USCRIPT_CHECK(ret == USCRIPT_SUCCESS, return ret, "Failed to create script stream %s", scriptName.c_str());
    ret = manager->ExtractFile(scriptName, outStream);
    USCRIPT_CHECK(ret == USCRIPT_SUCCESS, return ret, "Failed to extract script stream %s", scriptName.c_str());

    ret = ScriptInterpreter::ExecuteScript(this, outStream);
    manager->ClosePkgStream(outStream);
    USCRIPT_CHECK(ret == USCRIPT_SUCCESS, return ret, "Failed to ExecuteScript %s", scriptName.c_str());
    return ret;
}

int32_t ScriptManagerImpl::ExecuteScript(int32_t priority)
{
    USCRIPT_CHECK(priority < MAX_PRIORITY && priority >= 0,
        return USCRIPT_INVALID_PRIORITY, "ExecuteScript priority not support %d", priority);
    PkgManager::PkgManagerPtr manager = scriptEnv_->GetPkgManager();
    USCRIPT_CHECK(manager != nullptr, return USCRIPT_INVALID_PARAM, "Failed to get pkg manager");
    if (scriptFiles_[priority].size() == 0) {
        return USCRIPT_SUCCESS;
    }

    // Execute scripts
    int32_t threadNumber = threadPool_->GetThreadNumber();
    Task task;
    int32_t ret = USCRIPT_SUCCESS;
    int32_t retCode = USCRIPT_SUCCESS;
    task.workSize = threadNumber;
    task.processor = [&](int iter) {
        for (size_t i = iter; i < scriptFiles_[priority].size(); i += threadNumber) {
            ret = ExtractAndExecuteScript(manager, scriptFiles_[priority][i]);
            if (ret != USCRIPT_SUCCESS) {
                USCRIPT_LOGE("Failed to execute script %s", scriptFiles_[priority][i].c_str());
                retCode = ret;
            }
        }
    };
    ThreadPool::AddTask(std::move(task));
    return retCode;
}

int32_t ScriptManagerImpl::AddInstruction(const std::string &instrName, const UScriptInstructionPtr instruction)
{
    USCRIPT_LOGI("AddInstruction instrName: %s ", instrName.c_str());
    if (scriptInstructions_.find(instrName) != scriptInstructions_.end()) {
        USCRIPT_LOGW("Instruction: %s exist", instrName.c_str());
        // New instruction has the same name
        // with already registered instruction,
        // just override it.
        delete scriptInstructions_[instrName];
    }
    scriptInstructions_[instrName] = instruction;
    return USCRIPT_SUCCESS;
}

int32_t ScriptManagerImpl::AddScript(const std::string &scriptName, int32_t priority)
{
    USCRIPT_CHECK(priority < MAX_PRIORITY, return USCRIPT_INVALID_PRIORITY, "Invalid priority %d", priority);

    PkgManager::PkgManagerPtr manager = scriptEnv_->GetPkgManager();
    USCRIPT_CHECK(manager != nullptr, return USCRIPT_INVALID_PARAM, "Failed to get pkg manager");

    if (manager->GetFileInfo(scriptName) == nullptr) {
        USCRIPT_LOGE("Failed to access script %s", scriptName.c_str());
        return USCRIPT_SUCCESS;
    }
    scriptFiles_[priority].push_back(scriptName);
    return USCRIPT_SUCCESS;
}

UScriptInstruction* ScriptManagerImpl::FindInstruction(const std::string &instrName)
{
    if (scriptInstructions_.find(instrName) == scriptInstructions_.end()) {
        return nullptr;
    }
    return scriptInstructions_[instrName];
}

UScriptEnv* ScriptManagerImpl::GetScriptEnv(const std::string &instrName) const
{
    return scriptEnv_;
}
} // namespace uscript
