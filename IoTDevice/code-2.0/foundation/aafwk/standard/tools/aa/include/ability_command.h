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

#ifndef FOUNDATION_AAFWK_SERVICES_TOOLS_AA_INCLUDE_ABILITY_COMMAND_H
#define FOUNDATION_AAFWK_SERVICES_TOOLS_AA_INCLUDE_ABILITY_COMMAND_H

#include "shell_command.h"
#include "ability_manager_interface.h"

namespace OHOS {
namespace AAFwk {
namespace {
const std::string TOOL_NAME = "aa";

const std::string HELP_MSG = "usage: aa <command> <options>\n"
                             "These are common aa commands list:\n"
                             "  help                 list available commands\n"
                             "  start                start ability with options\n"
                             "  stop-service         stop service with options\n"
                             "  dump                 dump the ability stack info\n";

const std::string HELP_MSG_START =
    "usage: aa start <options>\n"
    "options list:\n"
    "  -h, --help                                               list available commands\n"
    "  [-d <device-id>] -a <ability-name> -b <bundle-name>      start ability with an element name\n";

const std::string HELP_MSG_STOP_SERVICE =
    "usage: aa stop-service <options>\n"
    "options list:\n"
    "  -h, --help                                               list available commands\n"
    "  [-d <device-id>] -a <ability-name> -b <bundle-name>      stop service with an element name\n";

const std::string HELP_MSG_DUMP = "usage: aa dump <options>\n"
                                  "options list:\n"
                                  "  -h, --help                   list available commands\n"
                                  "  -a, --all                    dump all abilities\n"
                                  "  -s, --stack <number>         dump the ability info of a specificed stack\n"
                                  "  -m, --mission <number>       dump the ability info of a specificed mission\n"
                                  "  -l, --stack-list             dump the mission list of every stack\n"
                                  "  -u, --ui                     dump the ability list of system ui stack\n"
                                  "  -e, --serv                   dump the service abilities\n"
                                  "  -d, --data                   dump the data abilities\n";

const std::string HELP_MSG_NO_ABILITY_NAME_OPTION = "error: -a <ability-name> is expected";
const std::string HELP_MSG_NO_BUNDLE_NAME_OPTION = "error: -b <bundle-name> is expected";

const std::string STRING_START_ABILITY_OK = "start ability successfully.";
const std::string STRING_START_ABILITY_NG = "error: failed to start ability.";

const std::string STRING_STOP_SERVICE_ABILITY_OK = "stop service ability successfully.";
const std::string STRING_STOP_SERVICE_ABILITY_NG = "error: failed to stop service ability.";
}  // namespace

class AbilityManagerShellCommand : public ShellCommand {
public:
    AbilityManagerShellCommand(int argc, char *argv[]);
    ~AbilityManagerShellCommand() override
    {}

private:
    ErrCode CreateCommandMap() override;
    ErrCode CreateMessageMap() override;
    ErrCode init() override;

    ErrCode RunAsHelpCommand();
    ErrCode RunAsStartAbility();
    ErrCode RunAsStopService();
    ErrCode RunAsDumpCommand();

    ErrCode RunAsDumpCommandOptopt();
    ErrCode MakeWantFromCmd(Want &want);
};

}  // namespace AAFwk
}  // namespace OHOS

#endif  // FOUNDATION_AAFWK_SERVICES_TOOLS_AA_INCLUDE_ABILITY_COMMAND_H