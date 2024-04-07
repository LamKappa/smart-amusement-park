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

#include "bundle_command.h"

#include <getopt.h>
#include <unistd.h>
#include "if_system_ability_manager.h"
#include "system_ability_definition.h"
#include "iservice_registry.h"
#include "app_log_wrapper.h"
#include "status_receiver_impl.h"

using namespace OHOS::AAFwk;

namespace OHOS {
namespace AppExecFwk {
namespace {
const std::string BUNDLE_NAME_EMPTY = "";

const std::string SHORT_OPTIONS = "hp:rn:m:ai";
const struct option LONG_OPTIONS[] = {
    {"help", no_argument, nullptr, 'h'},
    {"bundle-path", required_argument, nullptr, 'p'},
    {"replace", no_argument, nullptr, 'r'},
    {"bundle-name", required_argument, nullptr, 'n'},
    {"module-name", required_argument, nullptr, 'm'},
    {"all", no_argument, nullptr, 'a'},
    {"bundle-info", no_argument, nullptr, 'i'},
};
}  // namespace

BundleManagerShellCommand::BundleManagerShellCommand(int argc, char *argv[]) : ShellCommand(argc, argv, TOOL_NAME)
{}

ErrCode BundleManagerShellCommand::CreateCommandMap()
{
    commandMap_ = {
        {"help", std::bind(&BundleManagerShellCommand::RunAsHelpCommand, this)},
        {"install", std::bind(&BundleManagerShellCommand::RunAsInstallCommand, this)},
        {"uninstall", std::bind(&BundleManagerShellCommand::RunAsUninstallCommand, this)},
        {"dump", std::bind(&BundleManagerShellCommand::RunAsDumpCommand, this)},
    };

    return OHOS::ERR_OK;
}

ErrCode BundleManagerShellCommand::CreateMessageMap()
{
    messageMap_ = {
        //  error + message
        {
            IStatusReceiver::ERR_INSTALL_INTERNAL_ERROR,
            "error: install internal error.",
        },
        {
            IStatusReceiver::ERR_INSTALL_HOST_INSTALLER_FAILED,
            "error: install host installer failed.",
        },
        {
            IStatusReceiver::ERR_INSTALL_PARSE_FAILED,
            "error: install parse failed.",
        },
        {
            IStatusReceiver::ERR_INSTALL_VERSION_DOWNGRADE,
            "error: install version downgrade.",
        },
        {
            IStatusReceiver::ERR_INSTALL_VERIFICATION_FAILED,
            "error: install verification failed.",
        },
        {
            IStatusReceiver::ERR_INSTALL_NO_SIGNATURE_INFO,
            "error: install no signature info.",
        },
        {
            IStatusReceiver::ERR_INSTALL_UPDATE_INCOMPATIBLE,
            "error: install update incompatible.",
        },
        {
            IStatusReceiver::ERR_INSTALL_PARAM_ERROR,
            "error: install param error.",
        },
        {
            IStatusReceiver::ERR_INSTALL_PERMISSION_DENIED,
            "error: install permission denied.",
        },
        {
            IStatusReceiver::ERR_INSTALL_ENTRY_ALREADY_EXIST,
            "error: install entry already exist.",
        },
        {
            IStatusReceiver::ERR_INSTALL_STATE_ERROR,
            "error: install state error.",
        },
        {
            IStatusReceiver::ERR_INSTALL_FILE_PATH_INVALID,
            "error: install file path invalid.",
        },
        {
            IStatusReceiver::ERR_INSTALL_INVALID_HAP_NAME,
            "error: install invalid hap name.",
        },
        {
            IStatusReceiver::ERR_INSTALL_INVALID_BUNDLE_FILE,
            "error: install invalid bundle file.",
        },
        {
            IStatusReceiver::ERR_INSTALL_GENERATE_UID_ERROR,
            "error: install generate uid error.",
        },
        {
            IStatusReceiver::ERR_INSTALL_INSTALLD_SERVICE_ERROR,
            "error: install installd service error.",
        },
        {
            IStatusReceiver::ERR_INSTALL_BUNDLE_MGR_SERVICE_ERROR,
            "error: install bundle mgr service error.",
        },
        {
            IStatusReceiver::ERR_INSTALL_ALREADY_EXIST,
            "error: install already exist.",
        },

        {
            IStatusReceiver::ERR_INSTALL_PARSE_UNEXPECTED,
            "error: install parse unexpected.",
        },
        {
            IStatusReceiver::ERR_INSTALL_PARSE_MISSING_BUNDLE,
            "error: install parse missing bundle.",
        },
        {
            IStatusReceiver::ERR_INSTALL_PARSE_MISSING_ABILITY,
            "error: install parse missing ability.",
        },
        {
            IStatusReceiver::ERR_INSTALL_PARSE_NO_PROFILE,
            "error: install parse no profile.",
        },
        {
            IStatusReceiver::ERR_INSTALL_PARSE_BAD_PROFILE,
            "error: install parse bad profile.",
        },
        {
            IStatusReceiver::ERR_INSTALL_PARSE_PROFILE_PROP_TYPE_ERROR,
            "error: install parse profile prop type error.",
        },
        {
            IStatusReceiver::ERR_INSTALL_PARSE_PROFILE_MISSING_PROP,
            "error: install parse profile missing prop.",
        },
        {
            IStatusReceiver::ERR_INSTALL_PARSE_PERMISSION_ERROR,
            "error: install parse permission error.",
        },

        {
            IStatusReceiver::ERR_INSTALLD_PARAM_ERROR,
            "error: installd param error.",
        },
        {
            IStatusReceiver::ERR_INSTALLD_GET_PROXY_ERROR,
            "error: installd get proxy error.",
        },
        {
            IStatusReceiver::ERR_INSTALLD_CREATE_DIR_FAILED,
            "error: installd create dir failed.",
        },
        {
            IStatusReceiver::ERR_INSTALLD_CREATE_DIR_EXIST,
            "error: installd create dir exist.",
        },
        {
            IStatusReceiver::ERR_INSTALLD_CHOWN_FAILED,
            "error: installd chown failed.",
        },
        {
            IStatusReceiver::ERR_INSTALLD_REMOVE_DIR_FAILED,
            "error: installd remove dir failed.",
        },
        {
            IStatusReceiver::ERR_INSTALLD_EXTRACT_FILES_FAILED,
            "error: installd extract files failed.",
        },
        {
            IStatusReceiver::ERR_INSTALLD_RNAME_DIR_FAILED,
            "error: installd rename dir failed.",
        },
        {
            IStatusReceiver::ERR_INSTALLD_CLEAN_DIR_FAILED,
            "error: installd clean dir failed.",
        },

        {
            IStatusReceiver::ERR_UNINSTALL_SYSTEM_APP_ERROR,
            "error: uninstall system app error.",
        },
        {
            IStatusReceiver::ERR_UNINSTALL_KILLING_APP_ERROR,
            "error: uninstall killing app error.",
        },
        {
            IStatusReceiver::ERR_UNINSTALL_INVALID_NAME,
            "error: uninstall invalid name.",
        },
        {
            IStatusReceiver::ERR_UNINSTALL_PARAM_ERROR,
            "error: uninstall param error.",
        },
        {
            IStatusReceiver::ERR_UNINSTALL_PERMISSION_DENIED,
            "error: uninstall permission denied.",
        },
        {
            IStatusReceiver::ERR_UNINSTALL_BUNDLE_MGR_SERVICE_ERROR,
            "error: uninstall bundle mgr service error.",
        },
        {
            IStatusReceiver::ERR_UNINSTALL_MISSING_INSTALLED_BUNDLE,
            "error: uninstall missing installed bundle.",
        },
        {
            IStatusReceiver::ERR_UNINSTALL_MISSING_INSTALLED_MODULE,
            "error: uninstall missing installed module.",
        },
        {
            IStatusReceiver::ERR_UNKNOWN,
            "error: unknown.",
        }
    };

    return OHOS::ERR_OK;
}

ErrCode BundleManagerShellCommand::init()
{
    ErrCode result = OHOS::ERR_OK;

    if (!bundleMgrProxy_) {
        bundleMgrProxy_ = GetBundleMgrProxy();

        if (bundleMgrProxy_) {
            if (!bundleInstallerProxy_) {
                bundleInstallerProxy_ = bundleMgrProxy_->GetBundleInstaller();
            }
        }
    }

    if (!bundleMgrProxy_ || !bundleInstallerProxy_) {
        result = OHOS::ERR_INVALID_VALUE;
    }

    return result;
}

sptr<IBundleMgr> BundleManagerShellCommand::GetBundleMgrProxy() const
{
    sptr<ISystemAbilityManager> systemAbilityManager =
        SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (!systemAbilityManager) {
        APP_LOGE("failed to get system ability mgr.");
        return nullptr;
    }

    sptr<IRemoteObject> remoteObject = systemAbilityManager->GetSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    if (!remoteObject) {
        APP_LOGE("failed to get bundle manager proxy.");
        return nullptr;
    }

    APP_LOGI("get bundle manager proxy success.");
    return iface_cast<IBundleMgr>(remoteObject);
}

sptr<IBundleInstaller> BundleManagerShellCommand::GetInstallerProxy() const
{
    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (!bundleMgrProxy) {
        APP_LOGE("bundle mgr proxy is nullptr.");
        return nullptr;
    }

    sptr<IBundleInstaller> installerProxy = bundleMgrProxy->GetBundleInstaller();
    if (!installerProxy) {
        APP_LOGE("failed to get bundle installer proxy.");
        return nullptr;
    }

    APP_LOGI("get bundle installer proxy success.");
    return installerProxy;
}

ErrCode BundleManagerShellCommand::RunAsHelpCommand()
{
    resultReceiver_.append(HELP_MSG);

    return OHOS::ERR_OK;
}

ErrCode BundleManagerShellCommand::RunAsInstallCommand()
{
    int result = OHOS::ERR_OK;
    InstallFlag installFlag = InstallFlag::NORMAL;

    int option = -1;
    int counter = 0;
    std::string bundlePath = "";

    while (true) {
        counter++;

        option = getopt_long(argc_, argv_, SHORT_OPTIONS.c_str(), LONG_OPTIONS, nullptr);

        APP_LOGI("option: %{public}d, optopt: %{public}d, optind: %{public}d", option, optopt, optind);

        if (optind < 0 || optind > argc_) {
            return OHOS::ERR_INVALID_VALUE;
        }

        for (int i = 0; i < argc_; i++) {
            APP_LOGI("argv_[%{public}d]: %{public}s", i, argv_[i]);
        }

        if (option == -1) {
            if (counter == 1) {
                // When scanning the first argument
                if (strcmp(argv_[optind], cmd_.c_str()) == 0) {
                    // 'bm install' with no option: bm install
                    // 'bm install' with a wrong argument: bm install xxx
                    APP_LOGI("'bm install' with no option.");

                    resultReceiver_.append(HELP_MSG_NO_OPTION + "\n");
                    result = OHOS::ERR_INVALID_VALUE;
                }
            }
            break;
        }

        if (option == '?') {
            switch (optopt) {
                case 'p': {
                    // 'bm install -p' with no argument: bm install -p
                    // 'bm install --bundle-path' with no argument: bm install --bundle-path
                    APP_LOGI("'bm install %{public}s' with no argument.", argv_[optind - 1]);

                    resultReceiver_.append("error: option '");
                    resultReceiver_.append(argv_[optind - 1]);
                    resultReceiver_.append("' requires a value.\n");

                    result = OHOS::ERR_INVALID_VALUE;
                    break;
                }
                case 0: {
                    // 'bm install' with a unknown option: bm install --x
                    // 'bm install' with a unknown option: bm install --xxx
                    std::string unknownOption = "";
                    std::string unknownOptionMsg = GetUnknownOptionMsg(unknownOption);

                    APP_LOGI("'bm install' with a unknown option: %{public}s", unknownOption.c_str());

                    resultReceiver_.append(unknownOptionMsg);
                    result = OHOS::ERR_INVALID_VALUE;
                    break;
                }
                default: {
                    // 'bm install' with a unknown option: bm install -x
                    // 'bm install' with a unknown option: bm install -xxx
                    std::string unknownOption = "";
                    std::string unknownOptionMsg = GetUnknownOptionMsg(unknownOption);

                    APP_LOGI("'bm install' with a unknown option: %{public}s", unknownOption.c_str());

                    resultReceiver_.append(unknownOptionMsg);
                    result = OHOS::ERR_INVALID_VALUE;
                    break;
                }
            }
            break;
        }

        switch (option) {
            case 'h': {
                // 'bm install -h'
                // 'bm install --help'
                APP_LOGI("'bm install %{public}s'", argv_[optind - 1]);

                result = OHOS::ERR_INVALID_VALUE;
                break;
            }
            case 'p': {
                // 'bm install -p <bundle-file-path>'
                // 'bm install --bundle-path <bundle-file-path>'
                bundlePath = optarg;
                break;
            }
            case 'r': {
                // 'bm install -r'
                // 'bm install -r'
                installFlag = InstallFlag::REPLACE_EXISTING;
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
        if (resultReceiver_ == "" && bundlePath.size() == 0) {
            // 'bm install ...' with no bundle path option
            APP_LOGI("'bm install' with no bundle path option.");

            resultReceiver_.append(HELP_MSG_NO_BUNDLE_PATH_OPTION + "\n");
            result = OHOS::ERR_INVALID_VALUE;
        }
    }

    if (result != OHOS::ERR_OK) {
        resultReceiver_.append(HELP_MSG_INSTALL);
    } else {
        int32_t installResult = InstallOperation(bundlePath, installFlag);
        if (installResult == OHOS::ERR_OK) {
            resultReceiver_ = STRING_INSTALL_BUNDLE_OK + "\n";
        } else {
            resultReceiver_ = STRING_INSTALL_BUNDLE_NG + "\n";
            resultReceiver_.append(GetMessageFromCode(installResult));
        }
    }

    return result;
}

ErrCode BundleManagerShellCommand::RunAsUninstallCommand()
{
    int result = OHOS::ERR_OK;

    int option = -1;
    int counter = 0;
    std::string bundleName = "";
    std::string moduleName = "";

    while (true) {
        counter++;

        option = getopt_long(argc_, argv_, SHORT_OPTIONS.c_str(), LONG_OPTIONS, nullptr);

        APP_LOGI("option: %{public}d, optopt: %{public}d, optind: %{public}d", option, optopt, optind);

        if (optind < 0 || optind > argc_) {
            return OHOS::ERR_INVALID_VALUE;
        }

        for (int i = 0; i < argc_; i++) {
            APP_LOGI("argv_[%{public}d]: %{public}s", i, argv_[i]);
        }

        if (option == -1) {
            if (counter == 1) {
                // When scanning the first argument
                if (strcmp(argv_[optind], cmd_.c_str()) == 0) {
                    // 'bm uninstall' with no option: bm uninstall
                    // 'bm uninstall' with a wrong argument: bm uninstall xxx
                    APP_LOGI("'bm uninstall' %{public}s", HELP_MSG_NO_OPTION.c_str());

                    resultReceiver_.append(HELP_MSG_NO_OPTION + "\n");
                    result = OHOS::ERR_INVALID_VALUE;
                }
            }
            break;
        }

        if (option == '?') {
            switch (optopt) {
                case 'n': {
                    // 'bm uninstall -n' with no argument: bm uninstall -n
                    // 'bm uninstall --bundle-name' with no argument: bm uninstall --bundle-name
                    APP_LOGI("'bm uninstall -n' with no argument.");

                    resultReceiver_.append("error: option '");
                    resultReceiver_.append(argv_[optind - 1]);
                    resultReceiver_.append("' requires a value.\n");
                    result = OHOS::ERR_INVALID_VALUE;
                    break;
                }
                case 'm': {
                    // 'bm uninstall -m' with no argument: bm uninstall -m
                    // 'bm uninstall --module-name' with no argument: bm uninstall --module-name
                    APP_LOGI("'bm uninstall -m' with no argument.");

                    resultReceiver_.append("error: option '");
                    resultReceiver_.append(argv_[optind - 1]);
                    resultReceiver_.append("' requires a value.\n");
                    result = OHOS::ERR_INVALID_VALUE;
                    break;
                }
                case 0: {
                    // 'bm uninstall' with a unknown option: bm uninstall --x
                    // 'bm uninstall' with a unknown option: bm uninstall --xxx
                    std::string unknownOption = "";
                    std::string unknownOptionMsg = GetUnknownOptionMsg(unknownOption);

                    APP_LOGI("'bm uninstall' with a unknown option: %{public}s", unknownOption.c_str());

                    resultReceiver_.append(unknownOptionMsg);
                    result = OHOS::ERR_INVALID_VALUE;
                    break;
                }
                default: {
                    // 'bm uninstall' with a unknown option: bm uninstall -x
                    // 'bm uninstall' with a unknown option: bm uninstall -xxx
                    std::string unknownOption = "";
                    std::string unknownOptionMsg = GetUnknownOptionMsg(unknownOption);

                    APP_LOGI("'bm uninstall' with a unknown option: %{public}s", unknownOption.c_str());

                    resultReceiver_.append(unknownOptionMsg);
                    result = OHOS::ERR_INVALID_VALUE;
                    break;
                }
            }
            break;
        }

        switch (option) {
            case 'h': {
                // 'bm uninstall -h'
                // 'bm uninstall --help'
                APP_LOGI("'bm uninstall %{public}s'", argv_[optind - 1]);

                result = OHOS::ERR_INVALID_VALUE;
                break;
            }
            case 'n': {
                // 'bm uninstall -n xxx'
                // 'bm uninstall --bundle-name xxx'
                APP_LOGI("'bm uninstall %{public}s %{public}s'", argv_[optind - OFFSET_REQUIRED_ARGUMENT], optarg);

                bundleName = optarg;
                break;
            }
            case 'm': {
                // 'bm uninstall -m xxx'
                // 'bm uninstall --module-name xxx'
                APP_LOGI("'bm uninstall %{public}s %{public}s'", argv_[optind - OFFSET_REQUIRED_ARGUMENT], optarg);

                moduleName = optarg;
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
        if (resultReceiver_ == "" && bundleName.size() == 0) {
            // 'bm uninstall ...' with no bundle name option
            APP_LOGI("'bm uninstall' with bundle name option.");

            resultReceiver_.append(HELP_MSG_NO_BUNDLE_NAME_OPTION + "\n");
            result = OHOS::ERR_INVALID_VALUE;
        }
    }

    if (result != OHOS::ERR_OK) {
        resultReceiver_.append(HELP_MSG_UNINSTALL);
    } else {
        int32_t uninstallResult = UninstallOperation(bundleName, moduleName);
        if (uninstallResult == OHOS::ERR_OK) {
            resultReceiver_ = STRING_UNINSTALL_BUNDLE_OK + "\n";
        } else {
            resultReceiver_ = STRING_UNINSTALL_BUNDLE_NG + "\n";
            resultReceiver_.append(GetMessageFromCode(uninstallResult));
        }
    }

    return result;
}

ErrCode BundleManagerShellCommand::RunAsDumpCommand()
{
    int result = OHOS::ERR_OK;
    std::string dumpResults = "";

    int option = getopt_long(argc_, argv_, SHORT_OPTIONS.c_str(), LONG_OPTIONS, nullptr);

    APP_LOGI("option: %{public}d, optopt: %{public}d, optind: %{public}d", option, optopt, optind);

    if (optind < 0 || optind > argc_) {
        return OHOS::ERR_INVALID_VALUE;
    }

    for (int i = 0; i < argc_; i++) {
        APP_LOGI("argv_[%{public}d]: %{public}s", i, argv_[i]);
    }

    switch (option) {
        case 'h': {
            // 'bm dump -h'
            // 'bm dump --help'
            APP_LOGI("'bm dump %{public}s'", argv_[optind - 1]);

            result = OHOS::ERR_INVALID_VALUE;
            break;
        }
        case 'a': {
            // 'bm dump -a'
            // 'bm dump --all'
            APP_LOGI("'bm dump %{public}s'", argv_[optind - 1]);

            dumpResults = DumpBundleList();
            break;
        }
        case 'i': {
            // 'bm dump -i'
            // 'bm dump --bundle-info'
            APP_LOGI("'bm dump %{public}s'", argv_[optind - 1]);

            dumpResults = DumpBundleInfos();
            break;
        }
        case 'n': {
            // 'bm dump -n xxx'
            // 'bm dump --bundle-name xxx'
            APP_LOGI("'bm dump %{public}s %{public}s'", argv_[optind - OFFSET_REQUIRED_ARGUMENT], optarg);

            dumpResults = DumpBundleInfo();
            break;
        }
        case '?': {
            switch (optopt) {
                case 'n': {
                    // 'bm dump -n' with no argument: bm dump -n
                    // 'bm dump --bundle-name' with no argument: bm dump --bundle-name
                    APP_LOGI("'bm dump -n' with no argument.");

                    resultReceiver_.append("error: option '");
                    resultReceiver_.append(argv_[optind - 1]);
                    resultReceiver_.append("' requires a value.\n");
                    result = OHOS::ERR_INVALID_VALUE;
                    break;
                }
                case 0: {
                    // 'bm dump' with a unknown option: bm dump --x
                    // 'bm dump' with a unknown option: bm dump --xxx
                    std::string unknownOption = "";
                    std::string unknownOptionMsg = GetUnknownOptionMsg(unknownOption);

                    APP_LOGI("'bm dump' with a unknown option: %{public}s", unknownOption.c_str());

                    resultReceiver_.append(unknownOptionMsg);
                    result = OHOS::ERR_INVALID_VALUE;
                    break;
                }
                default: {
                    // 'bm dump' with a unknown option: bm dump -x
                    // 'bm dump' with a unknown option: bm dump -xxx
                    std::string unknownOption = "";
                    std::string unknownOptionMsg = GetUnknownOptionMsg(unknownOption);

                    APP_LOGI("'bm dump' with a unknown option: %{public}s", unknownOption.c_str());

                    resultReceiver_.append(unknownOptionMsg);
                    result = OHOS::ERR_INVALID_VALUE;
                    break;
                }
            }
            break;
        }
        default: {
            if (strcmp(argv_[optind], cmd_.c_str()) == 0) {
                // 'bm dump' with no option: bm dump
                // 'bm dump' with a wrong argument: bm dump xxx
                APP_LOGI("'bm dump' %{public}s", HELP_MSG_NO_OPTION.c_str());

                resultReceiver_.append(HELP_MSG_NO_OPTION + "\n");
                result = OHOS::ERR_INVALID_VALUE;
            }
            break;
        }
    }

    if (result != OHOS::ERR_OK) {
        resultReceiver_.append(HELP_MSG_DUMP);
    } else {
        APP_LOGI("dumpResults: %{public}s", dumpResults.c_str());

        resultReceiver_.append(dumpResults);
    }

    return result;
}

std::string BundleManagerShellCommand::DumpBundleList() const
{
    std::string dumpResults;
    bool dumpRet = bundleMgrProxy_->DumpInfos(DumpFlag::DUMP_BUNDLE_LIST, BUNDLE_NAME_EMPTY, dumpResults);
    if (!dumpRet) {
        APP_LOGE("failed to dump bundle list.");
    }
    return dumpResults;
}

std::string BundleManagerShellCommand::DumpBundleInfos() const
{
    std::string dumpResults;
    bool dumpRet = bundleMgrProxy_->DumpInfos(DumpFlag::DUMP_ALL_BUNDLE_INFO, BUNDLE_NAME_EMPTY, dumpResults);
    if (!dumpRet) {
        APP_LOGE("failed to dump bundle infos.");
    }
    return dumpResults;
}

std::string BundleManagerShellCommand::DumpBundleInfo() const
{
    std::string bundleName = optarg;
    std::string dumpResults;
    bool dumpRet = bundleMgrProxy_->DumpInfos(DumpFlag::DUMP_BUNDLE_INFO, bundleName, dumpResults);
    if (!dumpRet) {
        APP_LOGE("failed to dump bundle info.");
    }
    return dumpResults;
}

int32_t BundleManagerShellCommand::InstallOperation(const std::string bundlePath, const InstallFlag installFlag) const
{
    std::string absoluteBundlePath = "";
    if (bundlePath.size() > 0) {
        if (bundlePath.at(0) == '/') {
            // absolute path
            absoluteBundlePath.append(bundlePath);
        } else {
            // relative path
            char *currentPathPtr = getcwd(nullptr, 0);

            if (currentPathPtr != nullptr) {
                absoluteBundlePath.append(currentPathPtr);
                absoluteBundlePath.append('/' + bundlePath);

                free(currentPathPtr);
                currentPathPtr = nullptr;
            }
        }
    }

    APP_LOGI("bundlePath: %{public}s", bundlePath.c_str());
    APP_LOGI("absoluteBundlePath: %{public}s", absoluteBundlePath.c_str());

    sptr<StatusReceiverImpl> statusReceiver(new StatusReceiverImpl());
    InstallParam installParam;
    installParam.installFlag = installFlag;
    installParam.userId = 0;

    bundleInstallerProxy_->Install(absoluteBundlePath, installParam, statusReceiver);

    return statusReceiver->GetResultCode();
}

int32_t BundleManagerShellCommand::UninstallOperation(const std::string bundleName, const std::string moduleName) const
{
    sptr<StatusReceiverImpl> statusReceiver(new StatusReceiverImpl());
    InstallParam installParam;
    installParam.userId = 0;

    APP_LOGI("bundleName: %{public}s", bundleName.c_str());
    APP_LOGI("moduleName: %{public}s", moduleName.c_str());

    if (moduleName.size() != 0) {
        bundleInstallerProxy_->Uninstall(bundleName, moduleName, installParam, statusReceiver);
    } else {
        bundleInstallerProxy_->Uninstall(bundleName, installParam, statusReceiver);
    }

    return statusReceiver->GetResultCode();
}

}  // namespace AppExecFwk
}  // namespace OHOS
