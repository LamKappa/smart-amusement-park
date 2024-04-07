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

#ifndef _AMS_KIT_SYSTEM_TEST_SECOND_ABILITY_H_
#define _AMS_KIT_SYSTEM_TEST_SECOND_ABILITY_H_
#include "ability_loader.h"
#include "common_event.h"
#include "common_event_manager.h"
#include "kit_test_common_info.h"

namespace OHOS {
namespace AppExecFwk {
class SecondEventSubscriber;
class SecondAbility : public Ability {
public:
    void InsertWantCopyApi()
    {
        std::vector<std::function<void(int)>> funs = {
            [this](int code) { WantCopyCase1(code); },
        };
        mapCase_[(int)WantApi::WantCopy] = funs;
    }

    void InsertWantAssignApi()
    {
        std::vector<std::function<void(int)>> funs = {
            [this](int code) { WantAssignCase1(code); },
        };
        mapCase_[(int)WantApi::WantAssign] = funs;
    }

    void InsertAddEntityApi()
    {
        std::vector<std::function<void(int)>> funs = {
            [this](int code) { WantAddEntityCase1(code); },
            [this](int code) { WantAddEntityCase2(code); },
            [this](int code) { WantAddEntityCase3(code); },
        };
        mapCase_[(int)WantApi::AddEntity] = funs;
    }

    void InsertAddFlagsApi()
    {
        std::vector<std::function<void(int)>> funs = {
            [this](int code) { WantAddFlagsCase1(code); },
            [this](int code) { WantAddFlagsCase2(code); },
            [this](int code) { WantAddFlagsCase3(code); },
        };
        mapCase_[(int)WantApi::AddFlags] = funs;
    }

    void InsertClearWantApi()
    {
        std::vector<std::function<void(int)>> funs = {
            [this](int code) { WantClearWantCase1(code); },
            [this](int code) { WantClearWantCase2(code); },
        };
        mapCase_[(int)WantApi::ClearWant] = funs;
    }

    void InsertCountEntitiesApi()
    {
        std::vector<std::function<void(int)>> funs = {
            [this](int code) { WantCountEntitiesCase1(code); },
            [this](int code) { WantCountEntitiesCase2(code); },
        };
        mapCase_[(int)WantApi::CountEntities] = funs;
    }

    void InsertFormatMimeTypeApi()
    {
        std::vector<std::function<void(int)>> funs = {
            [this](int code) { WantFormatMimeTypeCase1(code); },
            [this](int code) { WantFormatMimeTypeCase2(code); },
            [this](int code) { WantFormatMimeTypeCase3(code); },
            [this](int code) { WantFormatMimeTypeCase4(code); },
        };
        mapCase_[(int)WantApi::FormatMimeType] = funs;
    }

    void InsertFormatTypeApi()
    {
        std::vector<std::function<void(int)>> funs = {
            [this](int code) { WantFormatTypeCase1(code); },
            [this](int code) { WantFormatTypeCase2(code); },
            [this](int code) { WantFormatTypeCase3(code); },
            [this](int code) { WantFormatTypeCase4(code); },
        };
        mapCase_[(int)WantApi::FormatType] = funs;
    }

    void InsertFormatUriApi()
    {
        std::vector<std::function<void(int)>> funs = {
            [this](int code) { WantFormatUriCase1(code); },
            [this](int code) { WantFormatUriCase2(code); },
            [this](int code) { WantFormatUriCase3(code); },
            [this](int code) { WantFormatUriCase4(code); },
        };
        mapCase_[(int)WantApi::FormatUri] = funs;
    }

    void InsertFormatUriAndTypeApi()
    {
        std::vector<std::function<void(int)>> funs = {
            [this](int code) { WantFormatUriAndTypeCase1(code); },
            [this](int code) { WantFormatUriAndTypeCase2(code); },
            [this](int code) { WantFormatUriAndTypeCase3(code); },
            [this](int code) { WantFormatUriAndTypeCase4(code); },
        };
        mapCase_[(int)WantApi::FormatUriAndType] = funs;
    }

    void InsertGetActionApi()
    {
        std::vector<std::function<void(int)>> funs = {
            [this](int code) { WantGetActionCase1(code); },
            [this](int code) { WantGetActionCase2(code); },
            [this](int code) { WantGetActionCase3(code); },
        };
        mapCase_[(int)WantApi::GetAction] = funs;
    }

    void InsertGetBundleApi()
    {
        std::vector<std::function<void(int)>> funs = {
            [this](int code) { WantGetBundleCase1(code); },
            [this](int code) { WantGetBundleCase2(code); },
            [this](int code) { WantGetBundleCase3(code); },
        };
        mapCase_[(int)WantApi::GetBundle] = funs;
    }

    void InsertGetEntitiesApi()
    {
        std::vector<std::function<void(int)>> funs = {
            [this](int code) { WantGetEntitiesCase1(code); },
            [this](int code) { WantGetEntitiesCase2(code); },
            [this](int code) { WantGetEntitiesCase3(code); },
        };
        mapCase_[(int)WantApi::GetEntities] = funs;
    }

    void InsertGetElementApi()
    {
        std::vector<std::function<void(int)>> funs = {
            [this](int code) { WantGetElementCase1(code); },
            [this](int code) { WantGetElementCase2(code); },
            [this](int code) { WantGetElementCase3(code); },
        };
        mapCase_[(int)WantApi::GetElement] = funs;
    }

    void InsertGetUriApi()
    {
        std::vector<std::function<void(int)>> funs = {
            [this](int code) { WantGetUriCase1(code); },
            [this](int code) { WantGetUriCase2(code); },
            [this](int code) { WantGetUriCase3(code); },
        };
        mapCase_[(int)WantApi::GetUri] = funs;
    }

    void InsertGetUriStringApi()
    {
        std::vector<std::function<void(int)>> funs = {
            [this](int code) { WantGetUriStringCase1(code); },
            [this](int code) { WantGetUriStringCase2(code); },
            [this](int code) { WantGetUriStringCase3(code); },
            [this](int code) { WantGetUriStringCase4(code); },
        };
        mapCase_[(int)WantApi::GetUriString] = funs;
    }

    void InsertGetFlagsApi()
    {
        std::vector<std::function<void(int)>> funs = {
            [this](int code) { WantGetFlagsCase1(code); },
            [this](int code) { WantGetFlagsCase2(code); },
            [this](int code) { WantGetFlagsCase3(code); },
        };
        mapCase_[(int)WantApi::GetFlags] = funs;
    }

    void InsertGetSchemeApi()
    {
        std::vector<std::function<void(int)>> funs = {
            [this](int code) { WantGetSchemeCase1(code); },
            [this](int code) { WantGetSchemeCase2(code); },
            [this](int code) { WantGetSchemeCase3(code); },
            [this](int code) { WantGetSchemeCase4(code); },
        };
        mapCase_[(int)WantApi::GetScheme] = funs;
    }

    void InsertGetTypeApi()
    {
        std::vector<std::function<void(int)>> funs = {
            [this](int code) { WantGetTypeCase1(code); },
            [this](int code) { WantGetTypeCase2(code); },
            [this](int code) { WantGetTypeCase3(code); },
        };
        mapCase_[(int)WantApi::GetType] = funs;
    }

    void InsertHasEntityApi()
    {
        std::vector<std::function<void(int)>> funs = {
            [this](int code) { WantHasEntityCase1(code); },
            [this](int code) { WantHasEntityCase2(code); },
            [this](int code) { WantHasEntityCase3(code); },
            [this](int code) { WantHasEntityCase4(code); },
        };
        mapCase_[(int)WantApi::HasEntity] = funs;
    }

    void InsertMakeMainAbilityApi()
    {
        std::vector<std::function<void(int)>> funs = {
            [this](int code) { WantMakeMainAbilityCase1(code); },
            [this](int code) { WantMakeMainAbilityCase2(code); },
        };
        mapCase_[(int)WantApi::MakeMainAbility] = funs;
    }

    void InsertMarshallingApi()
    {
        std::vector<std::function<void(int)>> funs = {
            [this](int code) { WantMarshallingCase1(code); },
            [this](int code) { WantMarshallingCase2(code); },
            [this](int code) { WantMarshallingCase3(code); },
            [this](int code) { WantMarshallingCase4(code); },
            [this](int code) { WantMarshallingCase5(code); },
            [this](int code) { WantMarshallingCase6(code); },
            [this](int code) { WantMarshallingCase7(code); },
            [this](int code) { WantMarshallingCase8(code); },
            [this](int code) { WantMarshallingCase9(code); },
            [this](int code) { WantMarshallingCase10(code); },
            [this](int code) { WantMarshallingCase11(code); },
            [this](int code) { WantMarshallingCase12(code); },
            [this](int code) { WantMarshallingCase13(code); },
            [this](int code) { WantMarshallingCase14(code); },
            [this](int code) { WantMarshallingCase15(code); },
            [this](int code) { WantMarshallingCase16(code); },
            [this](int code) { WantMarshallingCase17(code); },
            [this](int code) { WantMarshallingCase18(code); },
            [this](int code) { WantMarshallingCase19(code); },
            [this](int code) { WantMarshallingCase20(code); },
            [this](int code) { WantMarshallingCase21(code); },
            [this](int code) { WantMarshallingCase22(code); },
            [this](int code) { WantMarshallingCase23(code); },
            [this](int code) { WantMarshallingCase24(code); },
        };
        mapCase_[(int)WantApi::Marshalling] = funs;
    }

    void InsertParseUriApi()
    {
        std::vector<std::function<void(int)>> funs = {
            [this](int code) { WantParseUriCase1(code); },
            [this](int code) { WantParseUriCase2(code); },
            [this](int code) { WantParseUriCase3(code); },
            [this](int code) { WantParseUriCase4(code); },
            [this](int code) { WantParseUriCase5(code); },
            [this](int code) { WantParseUriCase6(code); },
            [this](int code) { WantParseUriCase7(code); },
            [this](int code) { WantParseUriCase8(code); },
            [this](int code) { WantParseUriCase9(code); },
            [this](int code) { WantParseUriCase10(code); },
            [this](int code) { WantParseUriCase11(code); },
            [this](int code) { WantParseUriCase12(code); },
            [this](int code) { WantParseUriCase13(code); },
            [this](int code) { WantParseUriCase14(code); },
            [this](int code) { WantParseUriCase15(code); },
            [this](int code) { WantParseUriCase16(code); },
            [this](int code) { WantParseUriCase17(code); },
            [this](int code) { WantParseUriCase18(code); },
            [this](int code) { WantParseUriCase19(code); },
            [this](int code) { WantParseUriCase20(code); },
            [this](int code) { WantParseUriCase21(code); },
            [this](int code) { WantParseUriCase22(code); },
            [this](int code) { WantParseUriCase23(code); },
        };
        mapCase_[(int)WantApi::ParseUri] = funs;
    }

    void InsertRemoveEntityApi()
    {
        std::vector<std::function<void(int)>> funs = {
            [this](int code) { WantRemoveEntityCase1(code); },
            [this](int code) { WantRemoveEntityCase2(code); },
            [this](int code) { WantRemoveEntityCase3(code); },
            [this](int code) { WantRemoveEntityCase4(code); },
            [this](int code) { WantRemoveEntityCase5(code); },
        };
        mapCase_[(int)WantApi::RemoveEntity] = funs;
    }

    void InsertRemoveFlagsApi()
    {
        std::vector<std::function<void(int)>> funs = {
            [this](int code) { WantRemoveFlagsCase1(code); },
            [this](int code) { WantRemoveFlagsCase2(code); },
            [this](int code) { WantRemoveFlagsCase3(code); },
            [this](int code) { WantRemoveFlagsCase4(code); },
            [this](int code) { WantRemoveFlagsCase5(code); },
        };
        mapCase_[(int)WantApi::RemoveFlags] = funs;
    }

    void InsertSetActionApi()
    {
        std::vector<std::function<void(int)>> funs = {
            [this](int code) { WantSetActionCase1(code); },
            [this](int code) { WantSetActionCase2(code); },
            [this](int code) { WantSetActionCase3(code); },
            [this](int code) { WantSetActionCase4(code); },
            [this](int code) { WantSetActionCase5(code); },
        };
        mapCase_[(int)WantApi::SetAction] = funs;
    }

    void InsertSetBundleApi()
    {
        std::vector<std::function<void(int)>> funs = {
            [this](int code) { WantSetBundleCase1(code); },
            [this](int code) { WantSetBundleCase2(code); },
            [this](int code) { WantSetBundleCase3(code); },
            [this](int code) { WantSetBundleCase4(code); },
            [this](int code) { WantSetBundleCase5(code); },
        };
        mapCase_[(int)WantApi::SetBundle] = funs;
    }

    void InsertSetElementApi()
    {
        std::vector<std::function<void(int)>> funs = {
            [this](int code) { WantSetElementCase1(code); },
            [this](int code) { WantSetElementCase2(code); },
            [this](int code) { WantSetElementCase3(code); },
        };
        mapCase_[(int)WantApi::SetElement] = funs;
    }

    void InsertSetElementNameStringStringApi()
    {
        std::vector<std::function<void(int)>> funs = {
            [this](int code) { WantSetElementNameStringStringCase1(code); },
            [this](int code) { WantSetElementNameStringStringCase2(code); },
            [this](int code) { WantSetElementNameStringStringCase3(code); },
            [this](int code) { WantSetElementNameStringStringCase4(code); },
            [this](int code) { WantSetElementNameStringStringCase5(code); },
        };
        mapCase_[(int)WantApi::SetElementName_String_String] = funs;
    }

    void InsertSetElementNameStringStringStringApi()
    {
        std::vector<std::function<void(int)>> funs = {
            [this](int code) { WantSetElementNameStringStringStringCase1(code); },
            [this](int code) { WantSetElementNameStringStringStringCase2(code); },
            [this](int code) { WantSetElementNameStringStringStringCase3(code); },
            [this](int code) { WantSetElementNameStringStringStringCase4(code); },
            [this](int code) { WantSetElementNameStringStringStringCase5(code); },
        };
        mapCase_[(int)WantApi::SetElementName_String_String_String] = funs;
    }

    void InsertSetFlagsApi()
    {
        std::vector<std::function<void(int)>> funs = {
            [this](int code) { WantSetFlagsCase1(code); },
            [this](int code) { WantSetFlagsCase2(code); },
            [this](int code) { WantSetFlagsCase3(code); },
            [this](int code) { WantSetFlagsCase4(code); },
            [this](int code) { WantSetFlagsCase5(code); },
            [this](int code) { WantSetFlagsCase6(code); },
            [this](int code) { WantSetFlagsCase7(code); },
            [this](int code) { WantSetFlagsCase8(code); },
            [this](int code) { WantSetFlagsCase9(code); },
            [this](int code) { WantSetFlagsCase10(code); },
            [this](int code) { WantSetFlagsCase11(code); },
        };
        mapCase_[(int)WantApi::SetFlags] = funs;
    }

    void InsertSetTypeApi()
    {
        std::vector<std::function<void(int)>> funs = {
            [this](int code) { WantSetTypeCase1(code); },
            [this](int code) { WantSetTypeCase2(code); },
            [this](int code) { WantSetTypeCase3(code); },
            [this](int code) { WantSetTypeCase4(code); },
        };
        mapCase_[(int)WantApi::SetType] = funs;
    }

    void InsertSetUriApi()
    {
        std::vector<std::function<void(int)>> funs = {
            [this](int code) { WantSetUriCase1(code); },
            [this](int code) { WantSetUriCase2(code); },
            [this](int code) { WantSetUriCase3(code); },
            [this](int code) { WantSetUriCase4(code); },
        };
        mapCase_[(int)WantApi::SetUri] = funs;
    }

    void InsertSetUriAndTypeApi()
    {
        std::vector<std::function<void(int)>> funs = {
            [this](int code) { WantSetUriAndTypeCase1(code); },
            [this](int code) { WantSetUriAndTypeCase2(code); },
            [this](int code) { WantSetUriAndTypeCase3(code); },
            [this](int code) { WantSetUriAndTypeCase4(code); },
        };
        mapCase_[(int)WantApi::SetUriAndType] = funs;
    }

    void InsertToUriApi()
    {
        std::vector<std::function<void(int)>> funs = {
            [this](int code) { WantToUriCase1(code); },
            [this](int code) { WantToUriCase2(code); },
            [this](int code) { WantToUriCase3(code); },
            [this](int code) { WantToUriCase4(code); },
            [this](int code) { WantToUriCase5(code); },
            [this](int code) { WantToUriCase6(code); },
            [this](int code) { WantToUriCase7(code); },
            [this](int code) { WantToUriCase8(code); },
            [this](int code) { WantToUriCase9(code); },
            [this](int code) { WantToUriCase10(code); },
            [this](int code) { WantToUriCase11(code); },
            [this](int code) { WantToUriCase12(code); },
            [this](int code) { WantToUriCase13(code); },
            [this](int code) { WantToUriCase14(code); },
            [this](int code) { WantToUriCase15(code); },
            [this](int code) { WantToUriCase16(code); },
            [this](int code) { WantToUriCase17(code); },
            [this](int code) { WantToUriCase18(code); },
            [this](int code) { WantToUriCase19(code); },
            [this](int code) { WantToUriCase20(code); },
            [this](int code) { WantToUriCase21(code); },
            [this](int code) { WantToUriCase22(code); },
            [this](int code) { WantToUriCase23(code); },
        };
        mapCase_[(int)WantApi::ToUri] = funs;
    }

    void InsertWantParseUriApi()
    {
        std::vector<std::function<void(int)>> funs = {
            [this](int code) { WantWantParseUriCase1(code); },
            [this](int code) { WantWantParseUriCase2(code); },
            [this](int code) { WantWantParseUriCase3(code); },
        };
        mapCase_[(int)WantApi::WantParseUri] = funs;
    }

    void InsertGetParamsApi()
    {
        std::vector<std::function<void(int)>> funs = {
            [this](int code) { WantGetParamsCase1(code); },
            [this](int code) { WantGetParamsCase2(code); },
            [this](int code) { WantGetParamsCase3(code); },
        };
        mapCase_[(int)WantApi::GetParams] = funs;
    }

    void InsertGetByteParamApi()
    {
        std::vector<std::function<void(int)>> funs = {
            [this](int code) { WantGetByteParamCase1(code); },
            [this](int code) { WantGetByteParamCase2(code); },
            [this](int code) { WantGetByteParamCase3(code); },
        };
        mapCase_[(int)WantApi::GetByteParam] = funs;
    }

    void InsertGetByteArrayParamApi()
    {
        std::vector<std::function<void(int)>> funs = {
            [this](int code) { WantGetByteArrayParamCase1(code); },
            [this](int code) { WantGetByteArrayParamCase2(code); },
            [this](int code) { WantGetByteArrayParamCase3(code); },
        };
        mapCase_[(int)WantApi::GetByteArrayParam] = funs;
    }

    void InsertGetBoolParamApi()
    {
        std::vector<std::function<void(int)>> funs = {
            [this](int code) { WantGetBoolParamCase1(code); },
            [this](int code) { WantGetBoolParamCase2(code); },
            [this](int code) { WantGetBoolParamCase3(code); },
        };
        mapCase_[(int)WantApi::GetBoolParam] = funs;
    }

    void InsertGetBoolArrayParamApi()
    {
        std::vector<std::function<void(int)>> funs = {
            [this](int code) { WantGetBoolArrayParamCase1(code); },
            [this](int code) { WantGetBoolArrayParamCase2(code); },
            [this](int code) { WantGetBoolArrayParamCase3(code); },
        };
        mapCase_[(int)WantApi::GetBoolArrayParam] = funs;
    }

    void InsertGetCharParamApi()
    {
        std::vector<std::function<void(int)>> funs = {
            [this](int code) { WantGetCharParamCase1(code); },
            [this](int code) { WantGetCharParamCase2(code); },
            [this](int code) { WantGetCharParamCase3(code); },
        };
        mapCase_[(int)WantApi::GetCharParam] = funs;
    }

    void InsertGetCharArrayParamApi()
    {
        std::vector<std::function<void(int)>> funs = {
            [this](int code) { WantGetCharArrayParamCase1(code); },
            [this](int code) { WantGetCharArrayParamCase2(code); },
            [this](int code) { WantGetCharArrayParamCase3(code); },
        };
        mapCase_[(int)WantApi::GetCharArrayParam] = funs;
    }

    void InsertGetIntParamApi()
    {
        std::vector<std::function<void(int)>> funs = {
            [this](int code) { WantGetIntParamCase1(code); },
            [this](int code) { WantGetIntParamCase2(code); },
            [this](int code) { WantGetIntParamCase3(code); },
        };
        mapCase_[(int)WantApi::GetIntParam] = funs;
    }

    void InsertGetIntArrayParamApi()
    {
        std::vector<std::function<void(int)>> funs = {
            [this](int code) { WantGetIntArrayParamCase1(code); },
            [this](int code) { WantGetIntArrayParamCase2(code); },
            [this](int code) { WantGetIntArrayParamCase3(code); },
        };
        mapCase_[(int)WantApi::GetIntArrayParam] = funs;
    }

    void InsertGetDoubleParamApi()
    {
        std::vector<std::function<void(int)>> funs = {
            [this](int code) { WantGetDoubleParamCase1(code); },
            [this](int code) { WantGetDoubleParamCase2(code); },
            [this](int code) { WantGetDoubleParamCase3(code); },
        };
        mapCase_[(int)WantApi::GetDoubleParam] = funs;
    }

    void InsertGetDoubleArrayParamApi()
    {
        std::vector<std::function<void(int)>> funs = {
            [this](int code) { WantGetDoubleArrayParamCase1(code); },
            [this](int code) { WantGetDoubleArrayParamCase2(code); },
            [this](int code) { WantGetDoubleArrayParamCase3(code); },
        };
        mapCase_[(int)WantApi::GetDoubleArrayParam] = funs;
    }

    void InsertGetFloatParamApi()
    {
        std::vector<std::function<void(int)>> funs = {
            [this](int code) { WantGetFloatParamCase1(code); },
            [this](int code) { WantGetFloatParamCase2(code); },
            [this](int code) { WantGetFloatParamCase3(code); },
        };
        mapCase_[(int)WantApi::GetFloatParam] = funs;
    }

    void InsertGetFloatArrayParamApi()
    {
        std::vector<std::function<void(int)>> funs = {
            [this](int code) { WantGetFloatArrayParamCase1(code); },
            [this](int code) { WantGetFloatArrayParamCase2(code); },
            [this](int code) { WantGetFloatArrayParamCase3(code); },
        };
        mapCase_[(int)WantApi::GetFloatArrayParam] = funs;
    }

    void InsertGetLongParamApi()
    {
        std::vector<std::function<void(int)>> funs = {
            [this](int code) { WantGetLongParamCase1(code); },
            [this](int code) { WantGetLongParamCase2(code); },
            [this](int code) { WantGetLongParamCase3(code); },
        };
        mapCase_[(int)WantApi::GetLongParam] = funs;
    }

    void InsertGetLongArrayParamApi()
    {
        std::vector<std::function<void(int)>> funs = {
            [this](int code) { WantGetLongArrayParamCase1(code); },
            [this](int code) { WantGetLongArrayParamCase2(code); },
            [this](int code) { WantGetLongArrayParamCase3(code); },
        };
        mapCase_[(int)WantApi::GetLongArrayParam] = funs;
    }

    void InsertGetShortParamApi()
    {
        std::vector<std::function<void(int)>> funs = {
            [this](int code) { WantGetShortParamCase1(code); },
            [this](int code) { WantGetShortParamCase2(code); },
            [this](int code) { WantGetShortParamCase3(code); },
        };
        mapCase_[(int)WantApi::GetShortParam] = funs;
    }

    void InsertGetShortArrayParamApi()
    {
        std::vector<std::function<void(int)>> funs = {
            [this](int code) { WantGetShortArrayParamCase1(code); },
            [this](int code) { WantGetShortArrayParamCase2(code); },
            [this](int code) { WantGetShortArrayParamCase3(code); },
        };
        mapCase_[(int)WantApi::GetShortArrayParam] = funs;
    }

    void InsertGetStringParamApi()
    {
        std::vector<std::function<void(int)>> funs = {
            [this](int code) { WantGetStringParamCase1(code); },
            [this](int code) { WantGetStringParamCase2(code); },
            [this](int code) { WantGetStringParamCase3(code); },
        };
        mapCase_[(int)WantApi::GetStringParam] = funs;
    }

    void InsertGetStringArrayParamApi()
    {
        std::vector<std::function<void(int)>> funs = {
            [this](int code) { WantGetStringArrayParamCase1(code); },
            [this](int code) { WantGetStringArrayParamCase2(code); },
            [this](int code) { WantGetStringArrayParamCase3(code); },
        };
        mapCase_[(int)WantApi::GetStringArrayParam] = funs;
    }

    void InsertSetParamByteApi()
    {
        std::vector<std::function<void(int)>> funs = {
            [this](int code) { WantSetParamByteCase1(code); },
            [this](int code) { WantSetParamByteCase2(code); },
            [this](int code) { WantSetParamByteCase3(code); },
            [this](int code) { WantSetParamByteCase4(code); },
            [this](int code) { WantSetParamByteCase5(code); },
        };
        mapCase_[(int)WantApi::SetParam_byte] = funs;
    }

    void InsertSetParamByteArrayApi()
    {
        std::vector<std::function<void(int)>> funs = {
            [this](int code) { WantSetParamByteArrayCase1(code); },
            [this](int code) { WantSetParamByteArrayCase2(code); },
            [this](int code) { WantSetParamByteArrayCase3(code); },
            [this](int code) { WantSetParamByteArrayCase4(code); },
            [this](int code) { WantSetParamByteArrayCase5(code); },
        };
        mapCase_[(int)WantApi::SetParam_byte_array] = funs;
    }

    void InsertSetParamBoolApi()
    {
        std::vector<std::function<void(int)>> funs = {
            [this](int code) { WantSetParamBoolCase1(code); },
            [this](int code) { WantSetParamBoolCase2(code); },
            [this](int code) { WantSetParamBoolCase3(code); },
            [this](int code) { WantSetParamBoolCase4(code); },
            [this](int code) { WantSetParamBoolCase5(code); },
        };
        mapCase_[(int)WantApi::SetParam_bool] = funs;
    }

    void InsertSetParamBoolArrayApi()
    {
        std::vector<std::function<void(int)>> funs = {
            [this](int code) { WantSetParamBoolArrayCase1(code); },
            [this](int code) { WantSetParamBoolArrayCase2(code); },
            [this](int code) { WantSetParamBoolArrayCase3(code); },
            [this](int code) { WantSetParamBoolArrayCase4(code); },
            [this](int code) { WantSetParamBoolArrayCase5(code); },
        };
        mapCase_[(int)WantApi::SetParam_bool_array] = funs;
    }

    void InsertSetParamCharApi()
    {
        std::vector<std::function<void(int)>> funs = {
            [this](int code) { WantSetParamCharCase1(code); },
            [this](int code) { WantSetParamCharCase2(code); },
            [this](int code) { WantSetParamCharCase3(code); },
            [this](int code) { WantSetParamCharCase4(code); },
            [this](int code) { WantSetParamCharCase5(code); },
        };
        mapCase_[(int)WantApi::SetParam_char] = funs;
    }

    void InsertSetParamCharArrayApi()
    {
        std::vector<std::function<void(int)>> funs = {
            [this](int code) { WantSetParamCharArrayCase1(code); },
            [this](int code) { WantSetParamCharArrayCase2(code); },
            [this](int code) { WantSetParamCharArrayCase3(code); },
            [this](int code) { WantSetParamCharArrayCase4(code); },
            [this](int code) { WantSetParamCharArrayCase5(code); },
        };
        mapCase_[(int)WantApi::SetParam_char_array] = funs;
    }

    void InsertSetParamIntApi()
    {
        std::vector<std::function<void(int)>> funs = {
            [this](int code) { WantSetParamIntCase1(code); },
            [this](int code) { WantSetParamIntCase2(code); },
            [this](int code) { WantSetParamIntCase3(code); },
            [this](int code) { WantSetParamIntCase4(code); },
            [this](int code) { WantSetParamIntCase5(code); },
        };
        mapCase_[(int)WantApi::SetParam_int] = funs;
    }

    void InsertSetParamIntArrayApi()
    {
        std::vector<std::function<void(int)>> funs = {
            [this](int code) { WantSetParamIntArrayCase1(code); },
            [this](int code) { WantSetParamIntArrayCase2(code); },
            [this](int code) { WantSetParamIntArrayCase3(code); },
            [this](int code) { WantSetParamIntArrayCase4(code); },
            [this](int code) { WantSetParamIntArrayCase5(code); },
        };
        mapCase_[(int)WantApi::SetParam_int_array] = funs;
    }

    void InsertSetParamDoubleApi()
    {
        std::vector<std::function<void(int)>> funs = {
            [this](int code) { WantSetParamDoubleCase1(code); },
            [this](int code) { WantSetParamDoubleCase2(code); },
            [this](int code) { WantSetParamDoubleCase3(code); },
            [this](int code) { WantSetParamDoubleCase4(code); },
            [this](int code) { WantSetParamDoubleCase5(code); },
        };
        mapCase_[(int)WantApi::SetParam_double] = funs;
    }

    void InsertSetParamDoubleArrayApi()
    {
        std::vector<std::function<void(int)>> funs = {
            [this](int code) { WantSetParamDoubleArrayCase1(code); },
            [this](int code) { WantSetParamDoubleArrayCase2(code); },
            [this](int code) { WantSetParamDoubleArrayCase3(code); },
            [this](int code) { WantSetParamDoubleArrayCase4(code); },
            [this](int code) { WantSetParamDoubleArrayCase5(code); },
        };
        mapCase_[(int)WantApi::SetParam_double_array] = funs;
    }

    void InsertSetParamFloatApi()
    {
        std::vector<std::function<void(int)>> funs = {
            [this](int code) { WantSetParamFloatCase1(code); },
            [this](int code) { WantSetParamFloatCase2(code); },
            [this](int code) { WantSetParamFloatCase3(code); },
            [this](int code) { WantSetParamFloatCase4(code); },
            [this](int code) { WantSetParamFloatCase5(code); },
        };
        mapCase_[(int)WantApi::SetParam_float] = funs;
    }

    void InsertSetParamFloatArrayApi()
    {
        std::vector<std::function<void(int)>> funs = {
            [this](int code) { WantSetParamFloatArrayCase1(code); },
            [this](int code) { WantSetParamFloatArrayCase2(code); },
            [this](int code) { WantSetParamFloatArrayCase3(code); },
            [this](int code) { WantSetParamFloatArrayCase4(code); },
            [this](int code) { WantSetParamFloatArrayCase5(code); },
        };
        mapCase_[(int)WantApi::SetParam_float_array] = funs;
    }

    void InsertSetParamLongApi()
    {
        std::vector<std::function<void(int)>> funs = {
            [this](int code) { WantSetParamLongCase1(code); },
            [this](int code) { WantSetParamLongCase2(code); },
            [this](int code) { WantSetParamLongCase3(code); },
            [this](int code) { WantSetParamLongCase4(code); },
            [this](int code) { WantSetParamLongCase5(code); },
        };
        mapCase_[(int)WantApi::SetParam_long] = funs;
    }

    void InsertSetParamLongArrayApi()
    {
        std::vector<std::function<void(int)>> funs = {
            [this](int code) { WantSetParamLongArrayCase1(code); },
            [this](int code) { WantSetParamLongArrayCase2(code); },
            [this](int code) { WantSetParamLongArrayCase3(code); },
            [this](int code) { WantSetParamLongArrayCase4(code); },
            [this](int code) { WantSetParamLongArrayCase5(code); },
        };
        mapCase_[(int)WantApi::SetParam_long_array] = funs;
    }

    void InsertSetParamShortApi()
    {
        std::vector<std::function<void(int)>> funs = {
            [this](int code) { WantSetParamShortCase1(code); },
            [this](int code) { WantSetParamShortCase2(code); },
            [this](int code) { WantSetParamShortCase3(code); },
            [this](int code) { WantSetParamShortCase4(code); },
            [this](int code) { WantSetParamShortCase5(code); },
        };
        mapCase_[(int)WantApi::SetParam_short] = funs;
    }

    void InsertSetParamShortArrayApi()
    {
        std::vector<std::function<void(int)>> funs = {
            [this](int code) { WantSetParamShortArrayCase1(code); },
            [this](int code) { WantSetParamShortArrayCase2(code); },
            [this](int code) { WantSetParamShortArrayCase3(code); },
            [this](int code) { WantSetParamShortArrayCase4(code); },
            [this](int code) { WantSetParamShortArrayCase5(code); },
        };
        mapCase_[(int)WantApi::SetParam_short_array] = funs;
    }

    void InsertSetParamStringApi()
    {
        std::vector<std::function<void(int)>> funs = {
            [this](int code) { WantSetParamStringCase1(code); },
            [this](int code) { WantSetParamStringCase2(code); },
            [this](int code) { WantSetParamStringCase3(code); },
            [this](int code) { WantSetParamStringCase4(code); },
            [this](int code) { WantSetParamStringCase5(code); },
        };
        mapCase_[(int)WantApi::SetParam_string] = funs;
    }

    void InsertSetParamStringArrayApi()
    {
        std::vector<std::function<void(int)>> funs = {
            [this](int code) { WantSetParamStringArrayCase1(code); },
            [this](int code) { WantSetParamStringArrayCase2(code); },
            [this](int code) { WantSetParamStringArrayCase3(code); },
            [this](int code) { WantSetParamStringArrayCase4(code); },
            [this](int code) { WantSetParamStringArrayCase5(code); },
        };
        mapCase_[(int)WantApi::SetParam_string_array] = funs;
    }

    void InsertHasParameterApi()
    {
        std::vector<std::function<void(int)>> funs = {
            [this](int code) { WantHasParameterCase1(code); },
            [this](int code) { WantHasParameterCase2(code); },
            [this](int code) { WantHasParameterCase3(code); },
            [this](int code) { WantHasParameterCase4(code); },
            [this](int code) { WantHasParameterCase5(code); },
        };
        mapCase_[(int)WantApi::HasParameter] = funs;
    }

    void InsertReplaceParamsWantParamsApi()
    {
        std::vector<std::function<void(int)>> funs = {
            [this](int code) { WantReplaceParamsWantParamsCase1(code); },
            [this](int code) { WantReplaceParamsWantParamsCase2(code); },
            [this](int code) { WantReplaceParamsWantParamsCase3(code); },
            [this](int code) { WantReplaceParamsWantParamsCase4(code); },
            [this](int code) { WantReplaceParamsWantParamsCase5(code); },
        };
        mapCase_[(int)WantApi::ReplaceParams_WantParams] = funs;
    }

    void InsertReplaceParamsWantApi()
    {
        std::vector<std::function<void(int)>> funs = {
            [this](int code) { WantReplaceParamsWantCase1(code); },
            [this](int code) { WantReplaceParamsWantCase2(code); },
            [this](int code) { WantReplaceParamsWantCase3(code); },
            [this](int code) { WantReplaceParamsWantCase4(code); },
            [this](int code) { WantReplaceParamsWantCase5(code); },
        };
        mapCase_[(int)WantApi::ReplaceParams_Want] = funs;
    }

    void InsertRemoveParamApi()
    {
        std::vector<std::function<void(int)>> funs = {
            [this](int code) { WantRemoveParamCase1(code); },
            [this](int code) { WantRemoveParamCase2(code); },
            [this](int code) { WantRemoveParamCase3(code); },
        };
        mapCase_[(int)WantApi::RemoveParam] = funs;
    }

    void InsertGetOperationApi()
    {
        std::vector<std::function<void(int)>> funs = {
            [this](int code) { WantGetOperationCase1(code); },
            [this](int code) { WantGetOperationCase2(code); },
            [this](int code) { WantGetOperationCase3(code); },
        };
        mapCase_[(int)WantApi::GetOperation] = funs;
    }

    void InsertOperationEqualsApi()
    {
        std::vector<std::function<void(int)>> funs = {
            [this](int code) { WantOperationEqualsCase1(code); },
            [this](int code) { WantOperationEqualsCase2(code); },
            [this](int code) { WantOperationEqualsCase3(code); },
            [this](int code) { WantOperationEqualsCase4(code); },
        };
        mapCase_[(int)WantApi::OperationEquals] = funs;
    }

    void InsertCloneOperationApi()
    {
        std::vector<std::function<void(int)>> funs = {
            [this](int code) { WantCloneOperationCase1(code); },
            [this](int code) { WantCloneOperationCase2(code); },
            [this](int code) { WantCloneOperationCase3(code); },
        };
        mapCase_[(int)WantApi::CloneOperation] = funs;
    }

    SecondAbility()
    {
        InitData();
        InitDataAgain();
    }

    void InitData()
    {
        InsertWantCopyApi();
        InsertWantAssignApi();
        InsertAddEntityApi();
        InsertAddFlagsApi();
        InsertClearWantApi();
        InsertCountEntitiesApi();
        InsertFormatMimeTypeApi();
        InsertFormatTypeApi();
        InsertFormatUriApi();
        InsertFormatUriAndTypeApi();
        InsertGetActionApi();
        InsertGetBundleApi();
        InsertGetEntitiesApi();
        InsertGetElementApi();
        InsertGetUriApi();
        InsertGetUriStringApi();
        InsertGetFlagsApi();
        InsertGetSchemeApi();
        InsertGetTypeApi();
        InsertHasEntityApi();
        InsertMakeMainAbilityApi();
        InsertMarshallingApi();
        InsertParseUriApi();
        InsertRemoveEntityApi();
        InsertRemoveFlagsApi();
        InsertSetActionApi();
        InsertSetBundleApi();
        InsertSetElementApi();
        InsertSetElementNameStringStringApi();
        InsertSetElementNameStringStringStringApi();
        InsertSetFlagsApi();
        InsertSetTypeApi();
        InsertSetUriApi();
        InsertSetUriAndTypeApi();
        InsertToUriApi();
        InsertWantParseUriApi();
        InsertGetParamsApi();
        InsertGetByteParamApi();
        InsertGetByteArrayParamApi();
        InsertGetBoolParamApi();
        InsertGetBoolArrayParamApi();
        InsertGetCharParamApi();
        InsertGetCharArrayParamApi();
        InsertGetIntParamApi();
        InsertGetIntArrayParamApi();
    }

    void InitDataAgain()
    {
        InsertGetDoubleParamApi();
        InsertGetDoubleArrayParamApi();
        InsertGetFloatParamApi();
        InsertGetFloatArrayParamApi();
        InsertGetLongParamApi();
        InsertGetLongArrayParamApi();
        InsertGetShortParamApi();
        InsertGetShortArrayParamApi();
        InsertGetStringParamApi();
        InsertGetStringArrayParamApi();
        InsertSetParamByteApi();
        InsertSetParamByteArrayApi();
        InsertSetParamBoolApi();
        InsertSetParamBoolArrayApi();
        InsertSetParamCharApi();
        InsertSetParamCharArrayApi();
        InsertSetParamIntApi();
        InsertSetParamIntArrayApi();
        InsertSetParamDoubleApi();
        InsertSetParamDoubleArrayApi();
        InsertSetParamFloatApi();
        InsertSetParamFloatArrayApi();
        InsertSetParamLongApi();
        InsertSetParamLongArrayApi();
        InsertSetParamShortApi();
        InsertSetParamShortArrayApi();
        InsertSetParamStringApi();
        InsertSetParamStringArrayApi();
        InsertHasParameterApi();
        InsertReplaceParamsWantParamsApi();
        InsertReplaceParamsWantApi();
        InsertRemoveParamApi();
        InsertGetOperationApi();
        InsertOperationEqualsApi();
        InsertCloneOperationApi();
    }

    virtual ~SecondAbility() = default;
    void SubscribeEvent();
    void TestWant(int apiIndex, int caseIndex, int code);
    bool CompareWantNoParams(const Want &source, const Want &target);

    void WantCopyCase1(int code);
    void WantAssignCase1(int code);

    void WantAddEntityCase1(int code);
    void WantAddEntityCase2(int code);
    void WantAddEntityCase3(int code);

    void WantAddFlagsCase1(int code);
    void WantAddFlagsCase2(int code);
    void WantAddFlagsCase3(int code);

    void WantClearWantCase1(int code);
    void WantClearWantCase2(int code);

    void WantCountEntitiesCase1(int code);
    void WantCountEntitiesCase2(int code);

    void WantFormatMimeTypeCase1(int code);
    void WantFormatMimeTypeCase2(int code);
    void WantFormatMimeTypeCase3(int code);
    void WantFormatMimeTypeCase4(int code);

    void WantFormatTypeCase1(int code);
    void WantFormatTypeCase2(int code);
    void WantFormatTypeCase3(int code);
    void WantFormatTypeCase4(int code);

    void WantParseUriCase1(int code);
    void WantParseUriCase2(int code);
    void WantParseUriCase3(int code);
    void WantParseUriCase4(int code);
    void WantParseUriCase5(int code);
    void WantParseUriCase6(int code);
    void WantParseUriCase7(int code);
    void WantParseUriCase8(int code);
    void WantParseUriCase9(int code);
    void WantParseUriCase10(int code);
    void WantParseUriCase11(int code);
    void WantParseUriCase12(int code);
    void WantParseUriCase13(int code);
    void WantParseUriCase14(int code);
    void WantParseUriCase15(int code);
    void WantParseUriCase16(int code);
    void WantParseUriCase17(int code);
    void WantParseUriCase18(int code);
    void WantParseUriCase19(int code);
    void WantParseUriCase20(int code);
    void WantParseUriCase21(int code);
    void WantParseUriCase22(int code);
    void WantParseUriCase23(int code);

    void WantFormatUriCase1(int code);
    void WantFormatUriCase2(int code);
    void WantFormatUriCase3(int code);
    void WantFormatUriCase4(int code);

    void WantFormatUriAndTypeCase1(int code);
    void WantFormatUriAndTypeCase2(int code);
    void WantFormatUriAndTypeCase3(int code);
    void WantFormatUriAndTypeCase4(int code);

    void WantGetActionCase1(int code);
    void WantGetActionCase2(int code);
    void WantGetActionCase3(int code);

    void WantGetBundleCase1(int code);
    void WantGetBundleCase2(int code);
    void WantGetBundleCase3(int code);

    void WantGetEntitiesCase1(int code);
    void WantGetEntitiesCase2(int code);
    void WantGetEntitiesCase3(int code);

    void WantGetElementCase1(int code);
    void WantGetElementCase2(int code);
    void WantGetElementCase3(int code);

    void WantGetUriCase1(int code);
    void WantGetUriCase2(int code);
    void WantGetUriCase3(int code);

    void WantGetUriStringCase1(int code);
    void WantGetUriStringCase2(int code);
    void WantGetUriStringCase3(int code);
    void WantGetUriStringCase4(int code);

    void WantGetFlagsCase1(int code);
    void WantGetFlagsCase2(int code);
    void WantGetFlagsCase3(int code);

    void WantGetSchemeCase1(int code);
    void WantGetSchemeCase2(int code);
    void WantGetSchemeCase3(int code);
    void WantGetSchemeCase4(int code);

    void WantGetTypeCase1(int code);
    void WantGetTypeCase2(int code);
    void WantGetTypeCase3(int code);

    void WantHasEntityCase1(int code);
    void WantHasEntityCase2(int code);
    void WantHasEntityCase3(int code);
    void WantHasEntityCase4(int code);

    void WantMakeMainAbilityCase1(int code);
    void WantMakeMainAbilityCase2(int code);

    void WantMarshallingCase1(int code);
    void WantMarshallingCase2(int code);
    void WantMarshallingCase3(int code);
    void WantMarshallingCase4(int code);
    void WantMarshallingCase5(int code);
    void WantMarshallingCase6(int code);
    void WantMarshallingCase7(int code);
    void WantMarshallingCase8(int code);
    void WantMarshallingCase9(int code);
    void WantMarshallingCase10(int code);
    void WantMarshallingCase11(int code);
    void WantMarshallingCase12(int code);
    void WantMarshallingCase13(int code);
    void WantMarshallingCase14(int code);
    void WantMarshallingCase15(int code);
    void WantMarshallingCase16(int code);
    void WantMarshallingCase17(int code);
    void WantMarshallingCase18(int code);
    void WantMarshallingCase19(int code);
    void WantMarshallingCase20(int code);
    void WantMarshallingCase21(int code);
    void WantMarshallingCase22(int code);
    void WantMarshallingCase23(int code);
    void WantMarshallingCase24(int code);

    void WantRemoveEntityCase1(int code);
    void WantRemoveEntityCase2(int code);
    void WantRemoveEntityCase3(int code);
    void WantRemoveEntityCase4(int code);
    void WantRemoveEntityCase5(int code);

    void WantRemoveFlagsCase1(int code);
    void WantRemoveFlagsCase2(int code);
    void WantRemoveFlagsCase3(int code);
    void WantRemoveFlagsCase4(int code);
    void WantRemoveFlagsCase5(int code);

    void WantSetActionCase1(int code);
    void WantSetActionCase2(int code);
    void WantSetActionCase3(int code);
    void WantSetActionCase4(int code);
    void WantSetActionCase5(int code);

    void WantSetBundleCase1(int code);
    void WantSetBundleCase2(int code);
    void WantSetBundleCase3(int code);
    void WantSetBundleCase4(int code);
    void WantSetBundleCase5(int code);

    void WantSetElementCase1(int code);
    void WantSetElementCase2(int code);
    void WantSetElementCase3(int code);

    void WantSetElementNameStringStringCase1(int code);
    void WantSetElementNameStringStringCase2(int code);
    void WantSetElementNameStringStringCase3(int code);
    void WantSetElementNameStringStringCase4(int code);
    void WantSetElementNameStringStringCase5(int code);

    void WantSetElementNameStringStringStringCase1(int code);
    void WantSetElementNameStringStringStringCase2(int code);
    void WantSetElementNameStringStringStringCase3(int code);
    void WantSetElementNameStringStringStringCase4(int code);
    void WantSetElementNameStringStringStringCase5(int code);

    void WantSetFlagsCase1(int code);
    void WantSetFlagsCase2(int code);
    void WantSetFlagsCase3(int code);
    void WantSetFlagsCase4(int code);
    void WantSetFlagsCase5(int code);
    void WantSetFlagsCase6(int code);
    void WantSetFlagsCase7(int code);
    void WantSetFlagsCase8(int code);
    void WantSetFlagsCase9(int code);
    void WantSetFlagsCase10(int code);
    void WantSetFlagsCase11(int code);

    void WantSetTypeCase1(int code);
    void WantSetTypeCase2(int code);
    void WantSetTypeCase3(int code);
    void WantSetTypeCase4(int code);

    void WantSetUriCase1(int code);
    void WantSetUriCase2(int code);
    void WantSetUriCase3(int code);
    void WantSetUriCase4(int code);

    void WantSetUriAndTypeCase1(int code);
    void WantSetUriAndTypeCase2(int code);
    void WantSetUriAndTypeCase3(int code);
    void WantSetUriAndTypeCase4(int code);

    void WantToUriCase1(int code);
    void WantToUriCase2(int code);
    void WantToUriCase3(int code);
    void WantToUriCase4(int code);
    void WantToUriCase5(int code);
    void WantToUriCase6(int code);
    void WantToUriCase7(int code);
    void WantToUriCase8(int code);
    void WantToUriCase9(int code);
    void WantToUriCase10(int code);
    void WantToUriCase11(int code);
    void WantToUriCase12(int code);
    void WantToUriCase13(int code);
    void WantToUriCase14(int code);
    void WantToUriCase15(int code);
    void WantToUriCase16(int code);
    void WantToUriCase17(int code);
    void WantToUriCase18(int code);
    void WantToUriCase19(int code);
    void WantToUriCase20(int code);
    void WantToUriCase21(int code);
    void WantToUriCase22(int code);
    void WantToUriCase23(int code);

    void WantWantParseUriCase1(int code);
    void WantWantParseUriCase2(int code);
    void WantWantParseUriCase3(int code);

    void WantGetParamsCase1(int code);
    void WantGetParamsCase2(int code);
    void WantGetParamsCase3(int code);

    void WantGetByteParamCase1(int code);
    void WantGetByteParamCase2(int code);
    void WantGetByteParamCase3(int code);

    void WantGetByteArrayParamCase1(int code);
    void WantGetByteArrayParamCase2(int code);
    void WantGetByteArrayParamCase3(int code);

    void WantGetBoolParamCase1(int code);
    void WantGetBoolParamCase2(int code);
    void WantGetBoolParamCase3(int code);

    void WantGetBoolArrayParamCase1(int code);
    void WantGetBoolArrayParamCase2(int code);
    void WantGetBoolArrayParamCase3(int code);

    void WantGetCharParamCase1(int code);
    void WantGetCharParamCase2(int code);
    void WantGetCharParamCase3(int code);

    void WantGetCharArrayParamCase1(int code);
    void WantGetCharArrayParamCase2(int code);
    void WantGetCharArrayParamCase3(int code);

    void WantGetIntParamCase1(int code);
    void WantGetIntParamCase2(int code);
    void WantGetIntParamCase3(int code);

    void WantGetIntArrayParamCase1(int code);
    void WantGetIntArrayParamCase2(int code);
    void WantGetIntArrayParamCase3(int code);

    void WantGetDoubleParamCase1(int code);
    void WantGetDoubleParamCase2(int code);
    void WantGetDoubleParamCase3(int code);

    void WantGetDoubleArrayParamCase1(int code);
    void WantGetDoubleArrayParamCase2(int code);
    void WantGetDoubleArrayParamCase3(int code);

    void WantGetFloatParamCase1(int code);
    void WantGetFloatParamCase2(int code);
    void WantGetFloatParamCase3(int code);

    void WantGetFloatArrayParamCase1(int code);
    void WantGetFloatArrayParamCase2(int code);
    void WantGetFloatArrayParamCase3(int code);

    void WantGetLongParamCase1(int code);
    void WantGetLongParamCase2(int code);
    void WantGetLongParamCase3(int code);

    void WantGetLongArrayParamCase1(int code);
    void WantGetLongArrayParamCase2(int code);
    void WantGetLongArrayParamCase3(int code);

    void WantGetShortParamCase1(int code);
    void WantGetShortParamCase2(int code);
    void WantGetShortParamCase3(int code);

    void WantGetShortArrayParamCase1(int code);
    void WantGetShortArrayParamCase2(int code);
    void WantGetShortArrayParamCase3(int code);

    void WantGetStringParamCase1(int code);
    void WantGetStringParamCase2(int code);
    void WantGetStringParamCase3(int code);

    void WantGetStringArrayParamCase1(int code);
    void WantGetStringArrayParamCase2(int code);
    void WantGetStringArrayParamCase3(int code);

    void WantSetParamByteCase1(int code);
    void WantSetParamByteCase2(int code);
    void WantSetParamByteCase3(int code);
    void WantSetParamByteCase4(int code);
    void WantSetParamByteCase5(int code);

    void WantSetParamByteArrayCase1(int code);
    void WantSetParamByteArrayCase2(int code);
    void WantSetParamByteArrayCase3(int code);
    void WantSetParamByteArrayCase4(int code);
    void WantSetParamByteArrayCase5(int code);

    void WantSetParamBoolCase1(int code);
    void WantSetParamBoolCase2(int code);
    void WantSetParamBoolCase3(int code);
    void WantSetParamBoolCase4(int code);
    void WantSetParamBoolCase5(int code);

    void WantSetParamBoolArrayCase1(int code);
    void WantSetParamBoolArrayCase2(int code);
    void WantSetParamBoolArrayCase3(int code);
    void WantSetParamBoolArrayCase4(int code);
    void WantSetParamBoolArrayCase5(int code);

    void WantSetParamCharCase1(int code);
    void WantSetParamCharCase2(int code);
    void WantSetParamCharCase3(int code);
    void WantSetParamCharCase4(int code);
    void WantSetParamCharCase5(int code);

    void WantSetParamCharArrayCase1(int code);
    void WantSetParamCharArrayCase2(int code);
    void WantSetParamCharArrayCase3(int code);
    void WantSetParamCharArrayCase4(int code);
    void WantSetParamCharArrayCase5(int code);

    void WantSetParamIntCase1(int code);
    void WantSetParamIntCase2(int code);
    void WantSetParamIntCase3(int code);
    void WantSetParamIntCase4(int code);
    void WantSetParamIntCase5(int code);

    void WantSetParamIntArrayCase1(int code);
    void WantSetParamIntArrayCase2(int code);
    void WantSetParamIntArrayCase3(int code);
    void WantSetParamIntArrayCase4(int code);
    void WantSetParamIntArrayCase5(int code);

    void WantSetParamDoubleCase1(int code);
    void WantSetParamDoubleCase2(int code);
    void WantSetParamDoubleCase3(int code);
    void WantSetParamDoubleCase4(int code);
    void WantSetParamDoubleCase5(int code);

    void WantSetParamDoubleArrayCase1(int code);
    void WantSetParamDoubleArrayCase2(int code);
    void WantSetParamDoubleArrayCase3(int code);
    void WantSetParamDoubleArrayCase4(int code);
    void WantSetParamDoubleArrayCase5(int code);

    void WantSetParamFloatCase1(int code);
    void WantSetParamFloatCase2(int code);
    void WantSetParamFloatCase3(int code);
    void WantSetParamFloatCase4(int code);
    void WantSetParamFloatCase5(int code);

    void WantSetParamFloatArrayCase1(int code);
    void WantSetParamFloatArrayCase2(int code);
    void WantSetParamFloatArrayCase3(int code);
    void WantSetParamFloatArrayCase4(int code);
    void WantSetParamFloatArrayCase5(int code);

    void WantSetParamLongCase1(int code);
    void WantSetParamLongCase2(int code);
    void WantSetParamLongCase3(int code);
    void WantSetParamLongCase4(int code);
    void WantSetParamLongCase5(int code);

    void WantSetParamLongArrayCase1(int code);
    void WantSetParamLongArrayCase2(int code);
    void WantSetParamLongArrayCase3(int code);
    void WantSetParamLongArrayCase4(int code);
    void WantSetParamLongArrayCase5(int code);

    void WantSetParamShortCase1(int code);
    void WantSetParamShortCase2(int code);
    void WantSetParamShortCase3(int code);
    void WantSetParamShortCase4(int code);
    void WantSetParamShortCase5(int code);

    void WantSetParamShortArrayCase1(int code);
    void WantSetParamShortArrayCase2(int code);
    void WantSetParamShortArrayCase3(int code);
    void WantSetParamShortArrayCase4(int code);
    void WantSetParamShortArrayCase5(int code);

    void WantSetParamStringCase1(int code);
    void WantSetParamStringCase2(int code);
    void WantSetParamStringCase3(int code);
    void WantSetParamStringCase4(int code);
    void WantSetParamStringCase5(int code);

    void WantSetParamStringArrayCase1(int code);
    void WantSetParamStringArrayCase2(int code);
    void WantSetParamStringArrayCase3(int code);
    void WantSetParamStringArrayCase4(int code);
    void WantSetParamStringArrayCase5(int code);

    void WantHasParameterCase1(int code);
    void WantHasParameterCase2(int code);
    void WantHasParameterCase3(int code);
    void WantHasParameterCase4(int code);
    void WantHasParameterCase5(int code);

    void WantReplaceParamsWantParamsCase1(int code);
    void WantReplaceParamsWantParamsCase2(int code);
    void WantReplaceParamsWantParamsCase3(int code);
    void WantReplaceParamsWantParamsCase4(int code);
    void WantReplaceParamsWantParamsCase5(int code);

    void WantReplaceParamsWantCase1(int code);
    void WantReplaceParamsWantCase2(int code);
    void WantReplaceParamsWantCase3(int code);
    void WantReplaceParamsWantCase4(int code);
    void WantReplaceParamsWantCase5(int code);

    void WantRemoveParamCase1(int code);
    void WantRemoveParamCase2(int code);
    void WantRemoveParamCase3(int code);

    void WantGetOperationCase1(int code);
    void WantGetOperationCase2(int code);
    void WantGetOperationCase3(int code);

    void WantOperationEqualsCase1(int code);
    void WantOperationEqualsCase2(int code);
    void WantOperationEqualsCase3(int code);
    void WantOperationEqualsCase4(int code);

    void WantCloneOperationCase1(int code);
    void WantCloneOperationCase2(int code);
    void WantCloneOperationCase3(int code);

    std::unordered_map<int, std::vector<std::function<void(int)>>> mapCase_;
    std::shared_ptr<SecondEventSubscriber> subscriber;

protected:
    void Init(const std::shared_ptr<AbilityInfo> &abilityInfo, const std::shared_ptr<OHOSApplication> &application,
        std::shared_ptr<AbilityHandler> &handler, const sptr<IRemoteObject> &token) override;
    virtual void OnStart(const Want &want) override;
    virtual void OnStop() override;
    virtual void OnActive() override;
    virtual void OnInactive() override;
    virtual void OnBackground() override;
    virtual void OnForeground(const Want &want) override;
    virtual void OnAbilityResult(int requestCode, int resultCode, const Want &resultData) override;
    virtual void OnBackPressed() override;
    virtual void OnNewWant(const Want &want) override;
};

class SecondEventSubscriber : public OHOS::EventFwk::CommonEventSubscriber {
public:
    SecondEventSubscriber(const OHOS::EventFwk::CommonEventSubscribeInfo &sp, SecondAbility *ability)
        : CommonEventSubscriber(sp)
    {
        mapTestFunc_ = {
            {"Want", [this](int apiIndex, int caseIndex, int code) { TestWant(apiIndex, caseIndex, code); }},
        };
        secondAbility = ability;
    }
    ~SecondEventSubscriber() = default;
    void TestWant(int apiIndex, int caseIndex, int code)
    {
        secondAbility->TestWant(apiIndex, caseIndex, code);
    }
    virtual void OnReceiveEvent(const OHOS::EventFwk::CommonEventData &data);

    SecondAbility *secondAbility;
    std::unordered_map<std::string, std::function<void(int, int, int)>> mapTestFunc_;
};

}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // _AMS_KIT_SYSTEM_TEST_SECOND_ABILITY_H_