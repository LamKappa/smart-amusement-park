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
#include <cstring>
#include <vector>
#include "want_wrapper.h"
#include "securec.h"
#include "hilog_wrapper.h"
namespace OHOS {
namespace AppExecFwk {
/**
 * @brief Parse the want parameters.
 *
 * @param param Indicates the want parameters saved the parse result.
 * @param env The environment that the Node-API call is invoked under.
 * @param args Indicates the arguments passed into the callback.
 *
 * @return The return value from NAPI C++ to JS for the module.
 */
napi_value UnwrapWant(Want &param, napi_env env, napi_value args)
{
    HILOG_INFO("%{public}s,called", __func__);
    // unwrap the param
    napi_valuetype valueType = napi_undefined;
    NAPI_CALL(env, napi_typeof(env, args, &valueType));
    NAPI_ASSERT(env, valueType == napi_object, "param type mismatch!");
    // unwrap the param : want object
    napi_value wantProp = nullptr;
    NAPI_CALL(env, napi_get_named_property(env, args, "want", &wantProp));
    NAPI_CALL(env, napi_typeof(env, wantProp, &valueType));
    NAPI_ASSERT(env, valueType == napi_object, "property type mismatch!");
    // get want action property
    napi_value property = nullptr;
    NAPI_CALL(env, napi_get_named_property(env, wantProp, "action", &property));
    NAPI_CALL(env, napi_typeof(env, property, &valueType));
    NAPI_ASSERT(env, valueType == napi_string, "property type mismatch!");
    param.SetAction(NapiValueToStringUtf8(env, property));
    // get want entities property
    property = nullptr;
    NAPI_CALL(env, napi_get_named_property(env, wantProp, "entities", &property));
    NAPI_CALL(env, napi_typeof(env, property, &valueType));
    NAPI_ASSERT(env, valueType == napi_object, "property type mismatch!");
    size_t index = 0;
    napi_value prop = nullptr;
    bool hasElement = false;
    while (napi_has_element(env, property, index, &hasElement) == napi_ok) {
        if (!hasElement) {
            break;
        }
        NAPI_CALL(env, napi_get_element(env, property, index, &prop));
        param.AddEntity(NapiValueToStringUtf8(env, prop));
        index++;
    }
    // get want type property
    property = nullptr;
    NAPI_CALL(env, napi_get_named_property(env, wantProp, "type", &property));
    NAPI_CALL(env, napi_typeof(env, property, &valueType));
    NAPI_ASSERT(env, valueType == napi_string, "property type mismatch!");
    param.SetType(NapiValueToStringUtf8(env, property));
    // get want flags property
    property = nullptr;
    NAPI_CALL(env, napi_get_named_property(env, wantProp, "flags", &property));
    NAPI_CALL(env, napi_typeof(env, property, &valueType));
    NAPI_ASSERT(env, valueType == napi_number, "property type mismatch!");
    unsigned int flags;
    NAPI_CALL(env, napi_get_value_uint32(env, property, &flags));
    param.SetFlags(flags);
    // get elementName property
    if (UnwrapElementName(param, env, wantProp) == nullptr) {
        HILOG_ERROR("%{public}s, unwrapElementName == nullptr", __func__);
        return nullptr;
    }
    // get want param (optional)
    property = nullptr;
    if (napi_get_named_property(env, wantProp, "want_param", &property) == napi_ok) {
        NAPI_CALL(env, napi_typeof(env, property, &valueType));
        if (valueType == napi_object) {
            if (UnwrapWantParam(param, env, property) == nullptr) {
                HILOG_ERROR("%{public}s, UnwrapWantParam == nullptr", __func__);
                return nullptr;
            }
        }
    }
    napi_value result;
    NAPI_CALL(env, napi_create_int32(env, 1, &result));
    return result;
}

/**
 * @brief Parse the elementName parameters.
 *
 * @param param Indicates the elementName parameters saved the parse result.
 * @param env The environment that the Node-API call is invoked under.
 * @param args Indicates the arguments passed into the callback.
 *
 * @return The return value from NAPI C++ to JS for the module.
 */
napi_value UnwrapElementName(Want &param, napi_env env, napi_value args)
{
    HILOG_INFO("%{public}s,called", __func__);
    // get elementName property
    std::string deviceId = "";
    std::string bundleName = "";
    std::string abilityName = "";
    napi_value property = nullptr;
    napi_valuetype valueType = napi_undefined;
    NAPI_CALL(env, napi_get_named_property(env, args, "elementName", &property));
    NAPI_CALL(env, napi_typeof(env, property, &valueType));
    NAPI_ASSERT(env, valueType == napi_object, "property type mismatch!");
    // get elementName:deviceId_ property
    napi_value prop = nullptr;
    NAPI_CALL(env, napi_get_named_property(env, property, "deviceId", &prop));
    NAPI_CALL(env, napi_typeof(env, prop, &valueType));
    NAPI_ASSERT(env, valueType == napi_string, "property type mismatch!");
    deviceId = NapiValueToStringUtf8(env, prop);
    // get elementName:bundleName_ property
    prop = nullptr;
    NAPI_CALL(env, napi_get_named_property(env, property, "bundleName", &prop));
    NAPI_CALL(env, napi_typeof(env, prop, &valueType));
    NAPI_ASSERT(env, valueType == napi_string, "property type mismatch!");
    bundleName = NapiValueToStringUtf8(env, prop);
    // get elementName:abilityName_ property
    prop = nullptr;
    NAPI_CALL(env, napi_get_named_property(env, property, "abilityName", &prop));
    NAPI_CALL(env, napi_typeof(env, prop, &valueType));
    NAPI_ASSERT(env, valueType == napi_string, "property type mismatch!");
    abilityName = NapiValueToStringUtf8(env, prop);
    param.SetElementName(deviceId, bundleName, abilityName);
    napi_value result;
    NAPI_CALL(env, napi_create_int32(env, 1, &result));
    return result;
}

/**
 * @brief Parse the wantParam parameters.
 *
 * @param param Indicates the wantParam parameters saved the parse result.
 * @param env The environment that the Node-API call is invoked under.
 * @param args Indicates the arguments passed into the callback.
 *
 * @return The return value from NAPI C++ to JS for the module.
 */
napi_value UnwrapWantParam(Want &want, napi_env env, napi_value wantParam)
{
    HILOG_INFO("%{public}s,called", __func__);
    napi_valuetype valueType = napi_undefined;
    napi_value proNameList = 0;
    uint32_t proCount = 0;

    NAPI_CALL(env, napi_get_property_names(env, wantParam, &proNameList));
    NAPI_CALL(env, napi_get_array_length(env, proNameList, &proCount));
    HILOG_INFO("UnwrapWantParam property size=%{public}d", proCount);

    napi_value proName = 0;
    napi_value proValue = 0;
    for (uint32_t index = 0; index < proCount; index++) {
        NAPI_CALL(env, napi_get_element(env, proNameList, index, &proName));
        std::string strProName = NapiValueToStringUtf8(env, proName);
        HILOG_INFO("UnwrapWantParam proName=%{public}s", strProName.c_str());
        NAPI_CALL(env, napi_get_named_property(env, wantParam, strProName.c_str(), &proValue));
        NAPI_CALL(env, napi_typeof(env, proValue, &valueType));

        switch (valueType) {
            case napi_string: {
                std::string str_pro_value = NapiValueToStringUtf8(env, proValue);
                HILOG_INFO("UnwrapWantParam proValue=%{public}s", str_pro_value.c_str());
                want.SetParam(strProName, str_pro_value);
                break;
            }
            case napi_boolean: {
                bool c_pro_value = false;
                NAPI_CALL(env, napi_get_value_bool(env, proValue, &c_pro_value));
                HILOG_INFO("UnwrapWantParam proValue=%{public}d", c_pro_value);
                want.SetParam(strProName, c_pro_value);
                break;
            }
            case napi_number: {
                int32_t c_pro_value32 = 0;
                double c_pro_value_double = 0.0;

                if (napi_get_value_int32(env, proValue, &c_pro_value32) == napi_ok) {
                    HILOG_INFO("UnwrapWantParam proValue=%{public}d", c_pro_value32);
                    want.SetParam(strProName, c_pro_value32);
                    break;
                }
                if (napi_get_value_double(env, proValue, &c_pro_value_double) == napi_ok) {
                    HILOG_INFO("UnwrapWantParam proValue=%{public}lf", c_pro_value_double);
                    want.SetParam(strProName, c_pro_value_double);
                    break;
                }
                HILOG_INFO("UnwrapWantParam unknown proValue of Number");
                break;
            }
            default: {
                if (UnwrapWantParamArray(want, env, strProName, proValue) == nullptr) {
                    return nullptr;
                }
            }
        }
    }
    napi_value result;
    NAPI_CALL(env, napi_create_int32(env, 1, &result));
    return result;
}

/**
 * @brief Parse the wantParamArray parameters.
 *
 * @param param Indicates the wantParamArray parameters saved the parse result.
 * @param env The environment that the Node-API call is invoked under.
 * @param args Indicates the arguments passed into the callback.
 *
 * @return The return value from NAPI C++ to JS for the module.
 */
napi_value UnwrapWantParamArray(Want &want, napi_env env, std::string strProName, napi_value proValue)
{
    HILOG_INFO("%{public}s,called", __func__);
    bool isArray = false;
    uint32_t arrayLength = 0;
    napi_value valueAry = 0;
    napi_valuetype valueAryType = napi_undefined;

    NAPI_CALL(env, napi_is_array(env, proValue, &isArray));
    NAPI_CALL(env, napi_get_array_length(env, proValue, &arrayLength));
    HILOG_INFO("UnwrapWantParam proValue is array, length=%{public}d", arrayLength);

    std::vector<std::string> stringList;
    std::vector<int> intList;
    std::vector<long> longList;
    std::vector<bool> boolList;
    std::vector<double> doubleList;
    bool isDouble = false;
    for (uint32_t j = 0; j < arrayLength; j++) {
        NAPI_CALL(env, napi_get_element(env, proValue, j, &valueAry));
        NAPI_CALL(env, napi_typeof(env, valueAry, &valueAryType));
        switch (valueAryType) {
            case napi_string: {
                std::string str_ary_value = NapiValueToStringUtf8(env, valueAry);
                HILOG_INFO("UnwrapWantParam string array proValue=%{public}s", str_ary_value.c_str());
                stringList.push_back(str_ary_value);
                break;
            }
            case napi_boolean: {
                bool c_ary_value = false;
                NAPI_CALL(env, napi_get_value_bool(env, valueAry, &c_ary_value));
                HILOG_INFO("UnwrapWantParam bool array proValue=%{public}d", c_ary_value);
                boolList.push_back(c_ary_value);
                break;
            }
            case napi_number: {
                int32_t c_ary_value32 = 0;
                int64_t c_ary_value64 = 0;
                double c_ary_value_double = 0.0;
                if (isDouble) {
                    if (napi_get_value_double(env, valueAry, &c_ary_value_double) == napi_ok) {
                        HILOG_INFO("UnwrapWantParam double array proValue=%{public}lf", c_ary_value_double);
                        doubleList.push_back(c_ary_value_double);
                    }
                    break;
                } else {
                    if (napi_get_value_int32(env, valueAry, &c_ary_value32) == napi_ok) {
                        HILOG_INFO("UnwrapWantParam int array proValue=%{public}d", c_ary_value32);
                        intList.push_back(c_ary_value32);
                        break;
                    }
                }

                if (napi_get_value_int64(env, valueAry, &c_ary_value64) == napi_ok) {
                    HILOG_INFO("UnwrapWantParam int64 array proValue=%{public}lld", c_ary_value64);
                    longList.push_back(c_ary_value64);
                    break;
                }
                if (napi_get_value_double(env, valueAry, &c_ary_value_double) == napi_ok) {
                    HILOG_INFO("UnwrapWantParam double array proValue=%{public}lf", c_ary_value_double);
                    isDouble = true;
                    if (intList.size() > 0) {
                        for (int k = 0; k < (int)intList.size(); k++) {
                            doubleList.push_back(intList[k]);
                        }
                        intList.clear();
                    }
                    doubleList.push_back(c_ary_value_double);
                    break;
                }
                HILOG_INFO("UnwrapWantParam array unkown Number");
                break;
            }
            default:
                HILOG_INFO("UnwrapWantParam array unkown");
                break;
        }
    }
    if (stringList.size() > 0) {
        want.SetParam(strProName, stringList);
    }
    if (intList.size() > 0) {
        want.SetParam(strProName, intList);
    }
    if (longList.size() > 0) {
        want.SetParam(strProName, longList);
    }
    if (boolList.size() > 0) {
        want.SetParam(strProName, boolList);
    }
    if (doubleList.size() > 0) {
        want.SetParam(strProName, doubleList);
    }
    napi_value result;
    NAPI_CALL(env, napi_create_int32(env, 1, &result));
    return result;
}
}  // namespace AppExecFwk
}  // namespace OHOS