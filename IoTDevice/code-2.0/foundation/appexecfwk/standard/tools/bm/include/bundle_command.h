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

#ifndef FOUNDATION_APPEXECFWK_TOOLS_BM_INCLUDE_BUNDLE_COMMAND_H
#define FOUNDATION_APPEXECFWK_TOOLS_BM_INCLUDE_BUNDLE_COMMAND_H

#include "shell_command.h"
#include "bundle_mgr_interface.h"
#include "bundle_installer_interface.h"

namespace OHOS {
namespace AppExecFwk {
namespace {
const std::string TOOL_NAME = "bm";

const std::string HELP_MSG = "usage: bm <command> <options>\n"
                             "These are common bm commands list:\n"
                             "  help         list available commands\n"
                             "  install      install a bundle with options\n"
                             "  uninstall    uninstall a bundle with options\n"
                             "  dump         dump the bundle info\n";

const std::string HELP_MSG_INSTALL =
    "usage: bm install <options>\n"
    "options list:\n"
    "  -h, --help                                   list available commands\n"
    "  -p, --bundle-path <bundle-file-path>         install a bundle in a specified path\n"
    "  -r -p <bundle-file-path>                     replace an existing bundle\n"
    "  -r --bundle-path <bundle-file-path>          replace an existing bundle\n";

const std::string HELP_MSG_UNINSTALL = "usage: bm uninstall <options>\n"
                                       "options list:\n"
                                       "  -h, --help                           list available commands\n"
                                       "  -n, --bundle-name <bundle-name> [-m, --module-name <module-name>]\n"
                                       "                                       uninstall a bundle by a bundle name\n";

const std::string HELP_MSG_DUMP = "usage: bm dump <options>\n"
                                  "options list:\n"
                                  "  -h, --help                           list available commands\n"
                                  "  -a, --all                            list all bundles in system\n"
                                  "  -i, --bundle-info                    list all bundles info in system\n"
                                  "  -n, --bundle-name <bundle-name>      list the bundle info by a bundle name\n";

const std::string HELP_MSG_NO_BUNDLE_PATH_OPTION =
    "error: you must specify a bundle path with '-p' or '--bundle-path'.";

const std::string HELP_MSG_NO_BUNDLE_NAME_OPTION =
    "error: you must specify a bundle name with '-n' or '--bundle-name'.";

const std::string STRING_INSTALL_BUNDLE_OK = "install bundle successfully.";
const std::string STRING_INSTALL_BUNDLE_NG = "error: failed to install bundle.";

const std::string STRING_UNINSTALL_BUNDLE_OK = "uninstall bundle successfully.";
const std::string STRING_UNINSTALL_BUNDLE_NG = "error: failed to uninstall bundle.";
}  // namespace

class BundleManagerShellCommand : public OHOS::AAFwk::ShellCommand {
public:
    BundleManagerShellCommand(int argc, char *argv[]);
    ~BundleManagerShellCommand() override
    {}

private:
    ErrCode CreateCommandMap() override;
    ErrCode CreateMessageMap() override;
    ErrCode init() override;

    sptr<IBundleMgr> GetBundleMgrProxy() const;
    sptr<IBundleInstaller> GetInstallerProxy() const;

    ErrCode RunAsHelpCommand();
    ErrCode RunAsInstallCommand();
    ErrCode RunAsUninstallCommand();
    ErrCode RunAsDumpCommand();

    std::string DumpBundleList() const;
    std::string DumpBundleInfo() const;
    std::string DumpBundleInfos() const;

    int32_t InstallOperation(const std::string bundlePath, const InstallFlag installFlag) const;
    int32_t UninstallOperation(const std::string bundleName, const std::string moduleName) const;

    sptr<IBundleMgr> bundleMgrProxy_;
    sptr<IBundleInstaller> bundleInstallerProxy_;
};

}  // namespace AppExecFwk
}  // namespace OHOS

#endif  // FOUNDATION_APPEXECFWK_TOOLS_BM_INCLUDE_BUNDLE_COMMAND_H