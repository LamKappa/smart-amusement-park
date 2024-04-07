/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

#include "js_ability.h"

#include <cstring>
#include "ace_event_error_code.h"
#include "ace_log.h"
#include "acelite_config.h"
#include "async_task_manager.h"
#ifdef OHOS_ACELITE_PRODUCT_WATCH
#include "dft_impl.h"
#include "js_async_work.h"
#endif // OHOS_ACELITE_PRODUCT_WATCH
#include "fatal_handler.h"
#include "js_ability_impl.h"
#include "js_profiler.h"
#include "product_adapter.h"

namespace OHOS {
namespace ACELite {
/**
 * This is a helper function to cast void* to JSAbilityImpl*, for header separating purpose.
 */
static JSAbilityImpl *CastAbilityImpl(void *abilityImpl)
{
    if (abilityImpl == nullptr) {
        return nullptr;
    }

    return static_cast<JSAbilityImpl *>(abilityImpl);
}

static void DumpNativeMemoryUsage()
{
#ifdef OHOS_ACELITE_PRODUCT_WATCH
    NativeMemInfo memInfo;
    ProductAdapter::GetNativeMemInfo(&memInfo);
    HILOG_DEBUG(HILOG_MODULE_ACE, "available free size: %d", memInfo.freeSize);
#endif // OHOS_ACELITE_PRODUCT_WATCH
}

void JSAbility::Launch(const char * const abilityPath, const char * const bundleName, uint16_t token,
                       const char *pageInfo)
{
    if (jsAbilityImpl_ != nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Launch only can be triggered once");
        ACE_ERROR_CODE_PRINT(EXCE_ACE_FWK_LAUNCH_FAILED, EXCE_ACE_APP_ALREADY_LAUNCHED);
        return;
    }

    if ((abilityPath == nullptr) || (strlen(abilityPath) == 0)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "invalid app path");
        ACE_ERROR_CODE_PRINT(EXCE_ACE_FWK_LAUNCH_FAILED, EXCE_ACE_INVALID_APP_PATH);
        return;
    }

    if ((bundleName == nullptr) || (strlen(bundleName) == 0)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "invalid bundle name");
        ACE_ERROR_CODE_PRINT(EXCE_ACE_FWK_LAUNCH_FAILED, EXCE_ACE_INVALID_BUNDLE_NAME);
        return;
    }

    DumpNativeMemoryUsage();
    jsAbilityImpl_ = new JSAbilityImpl();
    if (jsAbilityImpl_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Create JSAbilityRuntime failed");
        return;
    }
    START_TRACING(LAUNCH);
    JSAbilityImpl *jsAbilityImpl = CastAbilityImpl(jsAbilityImpl_);
    jsAbilityImpl->InitEnvironment(abilityPath, bundleName, token);
    FatalHandler::GetInstance().RegisterFatalHandler(this);
    jsAbilityImpl->DeliverCreate(pageInfo);
    STOP_TRACING();
    OUTPUT_TRACE();
}

void JSAbility::Show()
{
    if (jsAbilityImpl_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Must trigger Launch first");
        return;
    }

    JSAbilityImpl *jsAbilityImpl = CastAbilityImpl(jsAbilityImpl_);
    jsAbilityImpl->Show();
    AsyncTaskManager::GetInstance().SetFront(true);
    ProductAdapter::UpdateShowingState(true);
    isActived_ = true;
}

void JSAbility::Hide()
{
    if (jsAbilityImpl_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Must trigger Launch first");
        return;
    }

    JSAbilityImpl *jsAbilityImpl = CastAbilityImpl(jsAbilityImpl_);
    jsAbilityImpl->Hide();
    AsyncTaskManager::GetInstance().SetFront(false);
    ProductAdapter::UpdateShowingState(false);
    isActived_ = false;
}

void JSAbility::TransferToDestroy()
{
    if (jsAbilityImpl_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Must trigger Launch first");
        return;
    }

    JSAbilityImpl *jsAbilityImpl = CastAbilityImpl(jsAbilityImpl_);
    jsAbilityImpl->CleanUp();
    // Reset render flag or low layer task mutex in case we are during the rendering process,
    // this situation might happen if the destroy function is called outside of JS thread, such as AMS.
    ProductAdapter::UpdateShowingState(false);
    FatalHandler::GetInstance().ResetRendering();
    FatalHandler::GetInstance().SetExitingFlag(false);
#ifdef OHOS_ACELITE_PRODUCT_WATCH
    JsAsyncWork::SetAppQueueHandler(nullptr);
    DftImpl::GetInstance()->RegisterPageReplaced(nullptr);
#endif // OHOS_ACELITE_PRODUCT_WATCH
    delete reinterpret_cast<JSAbilityImpl *>(jsAbilityImpl_);
    jsAbilityImpl_ = nullptr;
    DumpNativeMemoryUsage();
}

void JSAbility::BackPressed()
{
    if (jsAbilityImpl_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Must trigger Launch first");
        return;
    }

    JSAbilityImpl *jsAbilityImpl = CastAbilityImpl(jsAbilityImpl_);
    jsAbilityImpl->NotifyBackPressed();
}

const char *JSAbility::GetPackageName()
{
    return JsAppContext::GetInstance()->GetCurrentBundleName();
}

// this public interface will be deprecated, only fatal scenario can trigger force destroy
void JSAbility::ForceDestroy()
{
    HILOG_ERROR(HILOG_MODULE_ACE, "ForceDestroy interface is deprecated as JS engine can not run on other task");
}

bool JSAbility::IsRecycled()
{
    return (jsAbilityImpl_ == nullptr);
}

LazyLoadManager *GetLazyLoadManager()
{
    JsAppContext *context = JsAppContext::GetInstance();
    return const_cast<LazyLoadManager *>(context->GetLazyLoadManager());
}

LazyLoadState GetLazyLoadManagerState()
{
    LazyLoadManager *lazyLoadManager = GetLazyLoadManager();
    return lazyLoadManager->GetState();
}

void JSAbility::LazyLoadHandleRenderTick(void *data)
{
    UNUSED(data);
    // double check, if state reseted, break
    LazyLoadState state = GetLazyLoadManagerState();
    if (state == LazyLoadState::INIT || state == LazyLoadState::DONE) {
        return;
    }

    GetLazyLoadManager()->RenderLazyLoadWatcher();
}

void JSAbility::HandleRenderTick()
{
    if (!isActived_) {
        // skip the TE tick if we are not forground
        ProductAdapter::NotifyRenderEnd();
        errorTickCount_++;
        if (errorTickCount_ %  ERR_TICK_COUNT_TRACE_CTRL == 1) {
            HILOG_WARN(HILOG_MODULE_ACE, "skip one render tick process since not actived, count[%d]", errorTickCount_);
        }
        if (errorTickCount_ == UINT32_MAX) {
            errorTickCount_ = 0;
        }
        return;
    }

    // reset error tick tracing count
    errorTickCount_ = 0;

#if defined(TARGET_SIMULATOR) && defined(FEATURE_LAZY_LOADING_MODULE)
    LazyLoadHandleRenderTick(nullptr);
#endif

#ifdef OHOS_ACELITE_PRODUCT_WATCH
    if ((ProductAdapter::IsTEHandlersRegisted()) && !(FatalHandler::GetInstance().IsAppExiting())) {
        FatalHandler::GetInstance().SetTEHandlingFlag(true);
        ProductAdapter::ProcessOneTE();
        // check if state is ready
        if (GetLazyLoadManagerState() == LazyLoadState::READY) {
            JsAsyncWork::DispatchAsyncWork(LazyLoadHandleRenderTick, nullptr);
        }
        FatalHandler::GetInstance().SetTEHandlingFlag(false);
    }
#endif // OHOS_ACELITE_PRODUCT_WATCH
}
} // namespace ACELite
} // namespace OHOS
