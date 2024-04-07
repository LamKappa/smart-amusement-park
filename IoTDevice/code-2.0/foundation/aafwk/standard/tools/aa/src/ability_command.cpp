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

#include "ability_command.h"

#include <getopt.h>
#include "ability_manager_client.h"
#include "hilog_wrapper.h"

using namespace OHOS::AppExecFwk;

namespace OHOS {
namespace AAFwk {
namespace {
const std::string SHORT_OPTIONS = "hd:a:b:r:t";
const struct option LONG_OPTIONS[] = {
    {"help", no_argument, nullptr, 'h'},
    {"device", required_argument, nullptr, 'd'},
    {"ability", required_argument, nullptr, 'a'},
    {"bundle", required_argument, nullptr, 'b'},
};

const std::string SHORT_OPTIONS_DUMP = "has:m:lud::e::";
const struct option LONG_OPTIONS_DUMP[] = {
    {"help", no_argument, nullptr, 'h'},
    {"all", no_argument, nullptr, 'a'},
    {"stack", required_argument, nullptr, 's'},
    {"mission", required_argument, nullptr, 'm'},
    {"stack-list", no_argument, nullptr, 'l'},
    {"ui", no_argument, nullptr, 'u'},
    {"data", no_argument, nullptr, 'd'},
    {"serv", no_argument, nullptr, 'e'},
};
}  // namespace

AbilityManagerShellCommand::AbilityManagerShellCommand(int argc, char *argv[]) : ShellCommand(argc, argv, TOOL_NAME)
{}

ErrCode AbilityManagerShellCommand::CreateCommandMap()
{
    commandMap_ = {
        {"help", std::bind(&AbilityManagerShellCommand::RunAsHelpCommand, this)},
        {"start", std::bind(&AbilityManagerShellCommand::RunAsStartAbility, this)},
        {"stop-service", std::bind(&AbilityManagerShellCommand::RunAsStopService, this)},
        {"dump", std::bind(&AbilityManagerShellCommand::RunAsDumpCommand, this)},
    };

    return OHOS::ERR_OK;
}

ErrCode AbilityManagerShellCommand::CreateMessageMap()
{
    messageMap_ = {
        //  code + message
        {
            RESOLVE_ABILITY_ERR,
            "error: resolve ability err.",
        },
        {
            GET_ABILITY_SERVICE_FAILED,
            "error: get ability service failed.",
        },
        {
            ABILITY_SERVICE_NOT_CONNECTED,
            "error: ability service not connected.",
        },
        {
            RESOLVE_APP_ERR,
            "error: resolve app err.",
        },
        {
            STACK_MANAGER_NOT_EXIST,
            "error: stack manager not exist.",
        },
        {
            ABILITY_EXISTED,
            "error: ability existed.",
        },
        {
            CREATE_MISSION_STACK_FAILED,
            "error: create mission stack failed.",
        },
        {
            CREATE_MISSION_RECORD_FAILED,
            "error: create mission record failed.",
        },
        {
            CREATE_ABILITY_RECORD_FAILED,
            "error: create ability record failed.",
        },
        {
            START_ABILITY_WAITING,
            "error: start ability waiting.",
        },
        {
            TERMINATE_LAUNCHER_DENIED,
            "error: terminate launcher denied.",
        },
        {
            CONNECTION_NOT_EXIST,
            "error: connection not exist.",
        },
        {
            INVALID_CONNECTION_STATE,
            "error: invalid connection state.",
        },
        {
            LOAD_ABILITY_TIMEOUT,
            "error: load ability timeout.",
        },
        {
            CONNECTION_TIMEOUT,
            "error: connection timeout.",
        },
        {
            GET_BUNDLE_MANAGER_SERVICE_FAILED,
            "error: get bundle manager service failed.",
        },
        {
            REMOVE_MISSION_ID_NOT_EXIST,
            "error: remove mission id not exist.",
        },
        {
            REMOVE_MISSION_LAUNCHER_DENIED,
            "error: remove mission launcher denied.",
        },
        {
            REMOVE_MISSION_ACTIVE_DENIED,
            "error: remove mission active denied.",
        },
        {
            REMOVE_MISSION_FAILED,
            "error: remove mission failed.",
        },
        {
            INNER_ERR,
            "error: inner err.",
        },
        {
            REMOVE_STACK_LAUNCHER_DENIED,
            "error: remove stack launcher denied.",
        },
        {
            REMOVE_STACK_FAILED,
            "error: remove stack failed.",
        },
        {
            MISSION_STACK_LIST_IS_EMPTY,
            "error: mission stack list is empty.",
        },
        {
            REMOVE_STACK_ID_NOT_EXIST,
            "error: remove stack id not exist.",
        },
        {
            TARGET_ABILITY_NOT_SERVICE,
            "error: target ability not service.",
        },
        {
            TERMINATE_SERVICE_IS_CONNECTED,
            "error: terminate service is connected.",
        },
        {
            START_SERVICE_ABILITY_ACTIVING,
            "error: start service ability activing.",
        },
        {
            MOVE_MISSION_FAILED,
            "error: move mission failed.",
        },
        {
            KILL_PROCESS_FAILED,
            "error: kill process failed.",
        },
        {
            UNINSTALL_APP_FAILED,
            "error: uninstall app failed.",
        },
    };

    return OHOS::ERR_OK;
}

ErrCode AbilityManagerShellCommand::init()
{
    ErrCode result = AbilityManagerClient::GetInstance()->Connect();

    return result;
}

ErrCode AbilityManagerShellCommand::RunAsHelpCommand()
{
    resultReceiver_.append(HELP_MSG);

    return OHOS::ERR_OK;
}

ErrCode AbilityManagerShellCommand::RunAsStartAbility()
{
    Want want;
    ErrCode result = MakeWantFromCmd(want);
    if (result == OHOS::ERR_OK) {
        result = AbilityManagerClient::GetInstance()->StartAbility(want);
        if (result == OHOS::ERR_OK) {
            HILOG_INFO("%{public}s", STRING_START_ABILITY_OK.c_str());
            resultReceiver_ = STRING_START_ABILITY_OK + "\n";
        } else {
            HILOG_INFO("%{public}s result = %{public}d", STRING_START_ABILITY_NG.c_str(), result);
            resultReceiver_ = STRING_START_ABILITY_NG + "\n";

            resultReceiver_.append(GetMessageFromCode(result));
        }
    } else {
        resultReceiver_.append(HELP_MSG_START);
        result = OHOS::ERR_INVALID_VALUE;
    }

    return result;
}

ErrCode AbilityManagerShellCommand::RunAsStopService()
{
    ErrCode result = OHOS::ERR_OK;

    Want want;
    result = MakeWantFromCmd(want);
    if (result == OHOS::ERR_OK) {
        result = AbilityManagerClient::GetInstance()->StopServiceAbility(want);
        if (result == OHOS::ERR_OK) {
            HILOG_INFO("%{public}s", STRING_STOP_SERVICE_ABILITY_OK.c_str());
            resultReceiver_ = STRING_STOP_SERVICE_ABILITY_OK + "\n";
        } else {
            HILOG_INFO("%{public}s result = %{public}d", STRING_STOP_SERVICE_ABILITY_NG.c_str(), result);
            resultReceiver_ = STRING_STOP_SERVICE_ABILITY_NG + "\n";

            resultReceiver_.append(GetMessageFromCode(result));
        }
    } else {
        resultReceiver_.append(HELP_MSG_STOP_SERVICE);
        result = OHOS::ERR_INVALID_VALUE;
    }

    return result;
}

ErrCode AbilityManagerShellCommand::RunAsDumpCommand()
{
    ErrCode result = OHOS::ERR_OK;

    std::string args;
    for (auto arg : argList_) {
        args += arg;
        args += " ";
    }

    int option = getopt_long(argc_, argv_, SHORT_OPTIONS_DUMP.c_str(), LONG_OPTIONS_DUMP, nullptr);

    HILOG_INFO("option: %{public}d, optopt: %{public}d, optind: %{public}d", option, optopt, optind);

    if (optind < 0 || optind > argc_) {
        return OHOS::ERR_INVALID_VALUE;
    }

    for (int i = 0; i < argc_; i++) {
        HILOG_INFO("argv_[%{public}d]: %{public}s", i, argv_[i]);
    }

    switch (option) {
        case 'h': {
            // 'aa dump -h'
            // 'aa dump --help'
            result = OHOS::ERR_INVALID_VALUE;
            break;
        }
        case 'a': {
            // 'aa dump -a'
            // 'aa dump --all'
            break;
        }
        case 's': {
            // 'aa dump -s xxx'
            // 'aa dump --stack xxx'
            break;
        }
        case 'm': {
            // 'aa dump -m xxx'
            // 'aa dump --mission xxx'
            break;
        }
        case 'l': {
            // 'aa dump -l'
            // 'aa dump --stack-list'
            break;
        }
        case 'u': {
            // 'aa dump -u'
            // 'aa dump --ui'
            break;
        }
        case 'd': {
            // 'aa dump -d'
            // 'aa dump --data'
            break;
        }
        case 'e': {
            // 'aa dump -e'
            // 'aa dump --serv'
            break;
        }
        case '?': {
            result = RunAsDumpCommandOptopt();
            break;
        }
        default: {
            if (strcmp(argv_[optind], cmd_.c_str()) == 0) {
                // 'aa dump' with no option: aa dump
                // 'aa dump' with a wrong argument: aa dump xxx
                HILOG_INFO("'aa dump' with no option.");

                resultReceiver_.append(HELP_MSG_NO_OPTION + "\n");
                result = OHOS::ERR_INVALID_VALUE;
            }
            break;
        }
    }

    if (result != OHOS::ERR_OK) {
        resultReceiver_.append(HELP_MSG_DUMP);
    } else {
        std::vector<std::string> dumpResults;
        result = AbilityManagerClient::GetInstance()->DumpState(args, dumpResults);
        if (result == OHOS::ERR_OK) {
            for (auto it : dumpResults) {
                resultReceiver_ += it + "\n";
            }
        } else {
            HILOG_INFO("failed to dump state.");
        }
    }

    return result;
}

ErrCode AbilityManagerShellCommand::RunAsDumpCommandOptopt()
{
    ErrCode result = OHOS::ERR_OK;

    switch (optopt) {
        case 's': {
            // 'aa dump -s' with no argument: aa dump -s
            // 'aa dump --stack' with no argument: aa dump --stack
            HILOG_INFO("'aa dump -s' with no argument.");

            resultReceiver_.append("error: option '");
            resultReceiver_.append(argv_[optind - 1]);
            resultReceiver_.append("' requires a value.\n");
            result = OHOS::ERR_INVALID_VALUE;
            break;
        }
        case 'm': {
            // 'aa dump -m' with no argument: aa dump -m
            // 'aa dump --mission' with no argument: aa dump --mission
            HILOG_INFO("'aa dump -m' with no argument.");

            resultReceiver_.append("error: option '");
            resultReceiver_.append(argv_[optind - 1]);
            resultReceiver_.append("' requires a value.\n");
            result = OHOS::ERR_INVALID_VALUE;
            break;
        }
        case 0: {
            // 'aa dump' with a unknown option: aa dump --x
            // 'aa dump' with a unknown option: aa dump --xxx
            std::string unknownOption = "";
            std::string unknownOptionMsg = GetUnknownOptionMsg(unknownOption);

            HILOG_INFO("'aa dump' with a unknown option: %{public}s", unknownOption.c_str());

            resultReceiver_.append(unknownOptionMsg);
            result = OHOS::ERR_INVALID_VALUE;
            break;
        }
        default: {
            // 'aa dump' with a unknown option: aa dump -x
            // 'aa dump' with a unknown option: aa dump -xxx
            std::string unknownOption = "";
            std::string unknownOptionMsg = GetUnknownOptionMsg(unknownOption);

            HILOG_INFO("'aa dump' with a unknown option: %{public}s", unknownOption.c_str());

            resultReceiver_.append(unknownOptionMsg);
            result = OHOS::ERR_INVALID_VALUE;
            break;
        }
    }

    return result;
}

ErrCode AbilityManagerShellCommand::MakeWantFromCmd(Want &want)
{
    int result = OHOS::ERR_OK;

    int option = -1;
    int counter = 0;

    std::string deviceId = "";
    std::string bundleName = "";
    std::string abilityName = "";

    while (true) {
        counter++;

        option = getopt_long(argc_, argv_, SHORT_OPTIONS.c_str(), LONG_OPTIONS, nullptr);

        HILOG_INFO("option: %{public}d, optopt: %{public}d, optind: %{public}d", option, optopt, optind);

        if (optind < 0 || optind > argc_) {
            return OHOS::ERR_INVALID_VALUE;
        }

        for (int i = 0; i < argc_; i++) {
            HILOG_INFO("argv_[%{public}d]: %{public}s", i, argv_[i]);
        }

        if (option == -1) {
            if (counter == 1) {
                // When scanning the first argument
                if (strcmp(argv_[optind], cmd_.c_str()) == 0) {
                    // 'aa start' with no option: aa start
                    // 'aa start' with a wrong argument: aa start xxx
                    // 'aa stop-service' with no option: aa stop-service
                    // 'aa stop-service' with a wrong argument: aa stop-service xxx
                    HILOG_INFO("'aa %{public}s' %{public}s", HELP_MSG_NO_OPTION.c_str(), cmd_.c_str());

                    resultReceiver_.append(HELP_MSG_NO_OPTION + "\n");
                    result = OHOS::ERR_INVALID_VALUE;
                }
            }
            break;
        }

        if (option == '?') {
            switch (optopt) {
                case 'd': {
                    // 'aa start -d' with no argument
                    // 'aa stop-service -d' with no argument
                    HILOG_INFO("'aa %{public}s -d' with no argument.", cmd_.c_str());

                    resultReceiver_.append("error: option '");
                    resultReceiver_.append(argv_[optind - 1]);
                    resultReceiver_.append("' requires a value.\n");

                    result = OHOS::ERR_INVALID_VALUE;
                    break;
                }
                case 'a': {
                    // 'aa start -a' with no argument
                    // 'aa stop-service -a' with no argument
                    HILOG_INFO("'aa %{public}s -d' with no argument.", cmd_.c_str());

                    resultReceiver_.append("error: option '");
                    resultReceiver_.append(argv_[optind - 1]);
                    resultReceiver_.append("' requires a value.\n");

                    result = OHOS::ERR_INVALID_VALUE;
                    break;
                }
                case 'b': {
                    // 'aa start -b' with no argument
                    // 'aa stop-service -b' with no argument
                    HILOG_INFO("'aa %{public}s -d' with no argument.", cmd_.c_str());

                    resultReceiver_.append("error: option '");
                    resultReceiver_.append(argv_[optind - 1]);
                    resultReceiver_.append("' requires a value.\n");

                    result = OHOS::ERR_INVALID_VALUE;
                    break;
                }
                case 0: {
                    // 'aa start' with a unknown option: aa start --x
                    // 'aa start' with a unknown option: aa start --xxx
                    // 'aa stop-service' with a unknown option: aa stop-service --x
                    // 'aa stop-service' with a unknown option: aa stop-service --xxx
                    std::string unknownOption = "";
                    std::string unknownOptionMsg = GetUnknownOptionMsg(unknownOption);

                    HILOG_INFO(
                        "'aa %{public}s' with a unknown option: %{public}s", unknownOption.c_str(), cmd_.c_str());

                    resultReceiver_.append(unknownOptionMsg);
                    result = OHOS::ERR_INVALID_VALUE;
                    break;
                }
                default: {
                    // 'aa start' with a unknown option: aa start -x
                    // 'aa start' with a unknown option: aa start -xxx
                    // 'aa stop-service' with a unknown option: aa stop-service -x
                    // 'aa stop-service' with a unknown option: aa stop-service -xxx
                    std::string unknownOption = "";
                    std::string unknownOptionMsg = GetUnknownOptionMsg(unknownOption);

                    HILOG_INFO(
                        "'aa %{public}s' with a unknown option: %{public}s", unknownOption.c_str(), cmd_.c_str());

                    resultReceiver_.append(unknownOptionMsg);
                    result = OHOS::ERR_INVALID_VALUE;
                    break;
                }
            }
            break;
        }

        switch (option) {
            case 'h': {
                // 'aa start -h'
                // 'aa start --help'
                // 'aa stop-service -h'
                // 'aa stop-service --help'
                result = OHOS::ERR_INVALID_VALUE;
                break;
            }
            case 'd': {
                // 'aa start -d xxx'
                // 'aa stop-service -d xxx'

                // save device ID
                if (optarg != nullptr) {
                    deviceId = optarg;
                }
                break;
            }
            case 'a': {
                // 'aa start -a xxx'
                // 'aa stop-service -a xxx'

                // save ability name
                abilityName = optarg;
                break;
            }
            case 'b': {
                // 'aa start -b xxx'
                // 'aa stop-service -b xxx'

                // save bundle name
                bundleName = optarg;
                break;
            }
            case 0: {
                break;
            }
            default: {
                break;
            }
        }
    }

    if (result == OHOS::ERR_OK) {
        if (abilityName.size() == 0 || bundleName.size() == 0) {
            // 'aa start [-d <device-id>] -a <ability-name> -b <bundle-name>'
            // 'aa stop-service [-d <device-id>] -a <ability-name> -b <bundle-name>'
            HILOG_INFO("'aa %{public}s' without enough options.", cmd_.c_str());

            if (abilityName.size() == 0) {
                resultReceiver_.append(HELP_MSG_NO_ABILITY_NAME_OPTION + "\n");
            }

            if (bundleName.size() == 0) {
                resultReceiver_.append(HELP_MSG_NO_BUNDLE_NAME_OPTION + "\n");
            }

            result = OHOS::ERR_INVALID_VALUE;
        } else {
            ElementName element(deviceId, bundleName, abilityName);
            want.SetElement(element);
        }
    }

    return result;
}

}  // namespace AAFwk
}  // namespace OHOS