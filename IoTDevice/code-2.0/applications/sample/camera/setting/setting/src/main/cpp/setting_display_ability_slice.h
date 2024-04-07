/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
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

#ifndef OHOS_SETTING_DISPLAY_ABILITY_SLICE_H
#define OHOS_SETTING_DISPLAY_ABILITY_SLICE_H

#include <cstddef>
#include <cstdio>
#include <cstring>
#include <iproxy_client.h>

#include "ability_loader.h"
#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "components/ui_list.h"
#include "components/ui_scroll_view.h"
#include "components/ui_toggle_button.h"
#include "event_listener.h"
#include "gfx_utils/list.h"
#include "parameter.h"
#include "samgr_lite.h"
#include "setting_utils.h"

namespace OHOS {
/*
0： on
1： off
2：return 0:off, return 1:on
*/
enum ComDisplay {
    COM_SET_DISPLAY_STEADY_ON,
    COM_SET_DISPLAY_NO_STEADY_ON,
    COM_GET_DISPLAY_STATUS,
};
const int MAX_DATA_LEN = 0x100;

class DisBtnOnStateChangeListener : public OHOS::UICheckBox::OnChangeListener, public OHOS::UIView::OnClickListener {
public:
    ~DisBtnOnStateChangeListener() {}
    explicit DisBtnOnStateChangeListener(IClientProxy* iClientProxy, UIToggleButton* togglebutton)
        : myIClientProxy(iClientProxy), myTogglebutton(togglebutton) {}

    bool OnChange(UICheckBox::UICheckBoxState state) override
    {
        return true;
    }
    bool OnClick(UIView& view, const ClickEvent& event) override
    {
        int com;
        bool status = myTogglebutton->GetState();
        if (status == true) {
            com = COM_SET_DISPLAY_NO_STEADY_ON;
        } else {
            com = COM_SET_DISPLAY_STEADY_ON;
        }
        IpcIo request;
        char data[MAX_DATA_LEN];
        IpcIoInit(&request, data, sizeof(data), 0);
        if (myIClientProxy != NULL) {
            myIClientProxy->Invoke(myIClientProxy, com, &request, NULL, NULL);
        }
        return true;
    }
private:
    int funcId_ = 0;
    IClientProxy* myIClientProxy;
    UIToggleButton* myTogglebutton;
};

class SettingDisplayAbilitySlice : public AbilitySlice {
public:
    SettingDisplayAbilitySlice()
        : headView_(nullptr), toggleButtonView_(nullptr), rootView_(nullptr),
          buttonBackListener_(nullptr), changeListener_(nullptr) {}

    virtual ~SettingDisplayAbilitySlice();
protected:
    void OnStart(const Want& want) override;
    void OnInactive() override;
    void OnActive(const Want& want) override;
    void OnBackground() override;
    void OnStop() override;

private:
    void SetButtonListener();
    void SetHead();
    void SetToggleButton();

    IClientProxy* remoteApi_ = nullptr;
    UIViewGroup* headView_;
    UIViewGroup* toggleButtonView_;
    RootView* rootView_;
    EventListener* buttonBackListener_;
    DisBtnOnStateChangeListener* changeListener_;
    constexpr static int TOGGLE_X = 36;
    constexpr static int TOGGLE_Y = 72;

    constexpr static int USE_WIFI_FONT_X = 54;
    constexpr static int USE_WIFI_FONT_Y = 187;

    constexpr static int ADD_WIFI_X = 0;
    constexpr static int SCROLL_WIFI_X = 36;
    constexpr static int SCROLL_WIFI_Y = 242;
    constexpr static int SCROLL_WIFI_WIDTH = 960;
    constexpr static int SCROLL_WIFI_HEIGHT = 238;
};
} // namespace OHOS
#endif
