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

#ifndef OHOS_APPLICATION_ENV_IMPL_H
#define OHOS_APPLICATION_ENV_IMPL_H

#include <string>
#include "nocopyable.h"

/**
 * L1 App_info define
 */
#include <list>
#include <string>

typedef struct {
    std::string bundleName;
    std::string srcPath;
    std::string dataPath;
    bool isNativeApp;
    std::list<std::string> moduleNames;
} AppInfo;

namespace OHOS {
namespace AppExecFwk {

struct ApplicationInfo;
class ApplicationEnvImpl : public NoCopyable {

public:
    /**
     * @brief Gets an instance of the applicationenvimpl class
     * @param -
     * @return instance indicates
     */
    static ApplicationEnvImpl *GetInstance()
    {
        static ApplicationEnvImpl instance;
        return &instance;
    }
    /**
     * @brief destructor
     * @param -
     * @return -
     */
    ~ApplicationEnvImpl() override = default;

    /**
     * @brief Sets L1 information about the runtime environment of the application to which the
     *        ability belongs, including the bundle name, source code path, and data path.
     * @param appInfo
     * @return void
     */
    void SetAppInfo(const AppInfo &appInfo);

    /**
     * @brief Sets information about the runtime environment of the application to which the
     *        ability belongs, including the bundle name, source code path, and data path.
     * @param appInfo
     * @return void
     */
    void SetAppInfo(const ApplicationInfo &appInfo);

    /**
     * @brief Gets the bundlename of the application's runtime environment
     * @param -
     * @return bundleName
     */
    const std::string &GetBundleName() const;

    /**
     * @brief Gets the SrcPath of the application's runtime environment
     * @param -
     * @return SrcPath
     */
    const std::string &GetSrcPath() const;

    /**
     * @brief Gets the DataPath of the application's runtime environment
     * @param -
     * @return DataPath
     */
    const std::string &GetDataPath() const;

private:
    ApplicationEnvImpl() = default;

    std::string bundleName_;

    std::string srcPath_;

    std::string dataPath_;

    static ApplicationEnvImpl instance_;
};

}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // OHOS_APPLICATION_ENV_IMPL_H
