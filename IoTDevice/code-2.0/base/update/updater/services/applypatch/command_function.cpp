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

#include "applypatch/command_function.h"
#include "command_process.h"

namespace updater {
std::unique_ptr<CommandFunction> CommandFunctionFactory::GetCommandFunction(const CommandType type)
{
    switch (type) {
        case CommandType::ABORT:
        {
            std::unique_ptr<AbortCommandFn> instr(std::make_unique<AbortCommandFn>());
            return std::move(instr);
        }
        case CommandType::NEW:
        {
            std::unique_ptr<NewCommandFn> instr(std::make_unique<NewCommandFn>());
            return std::move(instr);
        }
        case CommandType::BSDIFF:
        {
            std::unique_ptr<DiffAndMoveCommandFn> instr(std::make_unique<DiffAndMoveCommandFn>());
            return std::move(instr);
        }
        case CommandType::IMGDIFF:
        {
            std::unique_ptr<DiffAndMoveCommandFn> instr(std::make_unique<DiffAndMoveCommandFn>());
            return std::move(instr);
        }
        case CommandType::ERASE:
        {
            std::unique_ptr<ZeroAndEraseCommandFn> instr(std::make_unique<ZeroAndEraseCommandFn>());
            return std::move(instr);
        }
        case CommandType::ZERO:
        {
            std::unique_ptr<ZeroAndEraseCommandFn> instr(std::make_unique<ZeroAndEraseCommandFn>());
            return std::move(instr);
        }
        case CommandType::FREE:
        {
            std::unique_ptr<FreeCommandFn> instr(std::make_unique<FreeCommandFn>());
            return std::move(instr);
        }
        case CommandType::MOVE:
        {
            std::unique_ptr<DiffAndMoveCommandFn> instr(std::make_unique<DiffAndMoveCommandFn>());
            return std::move(instr);
        }
        case CommandType::STASH:
        {
            std::unique_ptr<StashCommandFn> instr(std::make_unique<StashCommandFn>());
            return std::move(instr);
        }
        default:
            break;
    }
    return nullptr;
}

void CommandFunctionFactory::ReleaseCommandFunction(std::unique_ptr<CommandFunction> &instr)
{
    instr.reset();
}
}