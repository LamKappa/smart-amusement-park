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

#ifndef FOUNDATION_APPEXECFWK_OHOS_APPLICATION_LOADER_H
#define FOUNDATION_APPEXECFWK_OHOS_APPLICATION_LOADER_H

#include "ohos_application.h"

#include <functional>
#include <string>
#include <unordered_map>

namespace OHOS {
namespace AppExecFwk {
using CreateApplication = std::function<OHOSApplication *(void)>;

class ApplicationLoader {
public:
    /**
     * @description: Gets the ApplicationLoader object to application register
     * @param None
     * @return ApplicationLoader
     */
    static ApplicationLoader &GetInstance();

    /**
     * @description: Ddefault deconstructor
     * @param None
     * @return None
     */
    ~ApplicationLoader() = default;

    /**
     * @description: Gets the ApplicationLoader object to register application
     * @param bundleName the bundle name of the application.
     * @param createFunc constructor function of application class.
     * @return None
     */
    void RegisterApplication(const std::string &bundleName, const CreateApplication &createFunc);

    /**
     * @description: Gets the {@link OHOSApplication} object
     * @param bundleName the bundle name of the application.
     * @return Return {@link OHOSApplication} object which is registered by developer.
     */
    OHOSApplication *GetApplicationByName(const std::string &bundleName = "OHOSApplication");

private:
    ApplicationLoader() = default;
    ApplicationLoader(const ApplicationLoader &) = delete;
    ApplicationLoader &operator=(const ApplicationLoader &) = delete;
    ApplicationLoader(ApplicationLoader &&) = delete;
    ApplicationLoader &operator=(ApplicationLoader &&) = delete;

    std::unordered_map<std::string, CreateApplication> applications_;
};

/**
 * @brief Registers the class name of an {@link OHOSApplication} child class.
 *
 * After implementing your own {@link OHOSApplication} class, you should call this function so that the
 * OHOSApplication management framework can create <b>OHOSApplication</b> instances when loading your
 * <b>OHOSApplication</b> class.
 *
 * @param className Indicates the {@link OHOSApplication} class name to register.
 */
#define REGISTER_APPLICATION(bundleName, className)                                           \
    __attribute__((constructor)) void REGISTER_APPLICATION##className()                       \
    {                                                                                         \
        ApplicationLoader::GetInstance().RegisterApplication(                                 \
            #bundleName, []() -> OHOSApplication * { return new (std::nothrow) className; }); \
    }

}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_OHOS_APPLICATION_LOADER_H
