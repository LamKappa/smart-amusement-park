TEMPLATE = lib
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

LOSCFG_TEST_JS_BUILD = false
is_debug = debug

DESTDIR = ../libs

DEFINES  -= UNICODE
DEFINES  -= FEATURE_TIMER_MODULE
DEFINES += TARGET_SIMULATOR
DEFINES += JS_ENGINE_EXTERNAL_CONTEXT
DEFINES += SCREENSIZE_SPECIFIED
DEFINES += JS_PROFILER=0
DEFINES += QT_SIMULATOR

eval(is_debug == release) {
    DEFINES += JS_PROFILER=1
}

LOSCFG_TEST_JS_BUILD {
    DEFINES += JSFWK_TEST
}

ROOT_PATH = ../../../../../../../..
ACELITE_FRAMEWORK_PATH = ../../../..
ACELITE_CORE_PATH = $${ACELITE_FRAMEWORK_PATH}/src/core

SOURCES += \
        $${ACELITE_CORE_PATH}/animation/transition_impl.cpp \
        #$${ACELITE_CORE_PATH}/async/js_async_work.cpp \
        #$${ACELITE_CORE_PATH}/async/message_queue_utils.cpp \
        $${ACELITE_CORE_PATH}/base/ace_log.cpp \
        $${ACELITE_CORE_PATH}/base/ace_mem_base.cpp \
        $${ACELITE_CORE_PATH}/base/dft_impl.cpp \
        $${ACELITE_CORE_PATH}/base/async_task_manager.cpp \
        $${ACELITE_CORE_PATH}/base/cache/cache_manager.cpp \
        $${ACELITE_CORE_PATH}/base/dfx_assist.cpp \
        $${ACELITE_CORE_PATH}/base/event_util.cpp \
        $${ACELITE_CORE_PATH}/base/js_debugger_config.cpp \
        $${ACELITE_CORE_PATH}/base/js_fwk_common.cpp \
        $${ACELITE_CORE_PATH}/base/key_parser.cpp \
        $${ACELITE_CORE_PATH}/base/lazy_load_manager.cpp \
        $${ACELITE_CORE_PATH}/base/lazy_load_watcher.cpp \
        $${ACELITE_CORE_PATH}/base/locale_util.cpp \
        $${ACELITE_CORE_PATH}/base/mem_proc.cpp \
        $${ACELITE_CORE_PATH}/base/memory_heap.cpp \
        $${ACELITE_CORE_PATH}/base/number_parser.cpp \
        $${ACELITE_CORE_PATH}/base/product_adapter.cpp \
        $${ACELITE_CORE_PATH}/base/string_util.cpp \
        $${ACELITE_CORE_PATH}/base/time_util.cpp \
        $${ACELITE_CORE_PATH}/components/analog_clock_component.cpp \
        $${ACELITE_CORE_PATH}/components/camera_component.cpp \
        $${ACELITE_CORE_PATH}/components/canvas_component.cpp \
        $${ACELITE_CORE_PATH}/components/chart_component.cpp \
        $${ACELITE_CORE_PATH}/components/circle_progress_component.cpp \
        $${ACELITE_CORE_PATH}/components/clock_hand_component.cpp \
        $${ACELITE_CORE_PATH}/components/component.cpp \
        $${ACELITE_CORE_PATH}/components/component_utils.cpp \
        $${ACELITE_CORE_PATH}/components/div_component.cpp \
        $${ACELITE_CORE_PATH}/components/event_listener.cpp \
        $${ACELITE_CORE_PATH}/components/horizon_progress_component.cpp \
        $${ACELITE_CORE_PATH}/components/image_animator_component.cpp \
        $${ACELITE_CORE_PATH}/components/image_component.cpp \
        $${ACELITE_CORE_PATH}/components/input_button_component.cpp \
        $${ACELITE_CORE_PATH}/components/input_checkbox_component.cpp \
        $${ACELITE_CORE_PATH}/components/input_radio_component.cpp \
        $${ACELITE_CORE_PATH}/components/list_adapter.cpp \
        $${ACELITE_CORE_PATH}/components/list_component.cpp \
        $${ACELITE_CORE_PATH}/components/marquee_component.cpp \
        $${ACELITE_CORE_PATH}/components/panel_view.cpp \
        $${ACELITE_CORE_PATH}/components/picker_view_component.cpp \
        $${ACELITE_CORE_PATH}/components/qrcode_component.cpp \
        $${ACELITE_CORE_PATH}/components/scroll_layer.cpp \
        $${ACELITE_CORE_PATH}/components/slider_component.cpp \
        $${ACELITE_CORE_PATH}/components/stack_component.cpp \
        $${ACELITE_CORE_PATH}/components/swiper_component.cpp \
        $${ACELITE_CORE_PATH}/components/switch_component.cpp \
        $${ACELITE_CORE_PATH}/components/tab_bar_component.cpp \
        $${ACELITE_CORE_PATH}/components/tab_content_component.cpp \
        $${ACELITE_CORE_PATH}/components/tabs_component.cpp \
        $${ACELITE_CORE_PATH}/components/text_component.cpp \
        $${ACELITE_CORE_PATH}/components/video_component.cpp \
        $${ACELITE_CORE_PATH}/components/video_view.cpp \
        #$${ACELITE_CORE_PATH}/context/ace_ability.cpp \
        $${ACELITE_CORE_PATH}/context/ace_event_error_code.cpp \
        $${ACELITE_CORE_PATH}/context/fatal_handler.cpp \
        $${ACELITE_CORE_PATH}/context/js_ability.cpp \
        $${ACELITE_CORE_PATH}/context/js_ability_impl.cpp \
        $${ACELITE_CORE_PATH}/context/js_app_context.cpp \
        $${ACELITE_CORE_PATH}/context/js_app_environment.cpp \
        $${ACELITE_CORE_PATH}/context/js_framework_raw.cpp \
        $${ACELITE_CORE_PATH}/context/js_profiler.cpp \
        $${ACELITE_CORE_PATH}/context/js_timer_list.cpp \
        $${ACELITE_CORE_PATH}/dialog/js_dialog.cpp \
        $${ACELITE_CORE_PATH}/directive/descriptor_utils.cpp \
        $${ACELITE_CORE_PATH}/directive/directive_watcher_callback.cpp \
        $${ACELITE_CORE_PATH}/jsi/jsi.cpp \
        $${ACELITE_CORE_PATH}/modules/app_module.cpp \
        $${ACELITE_CORE_PATH}/modules/dfx_module.cpp \
        $${ACELITE_CORE_PATH}/modules/dialog_module.cpp \
        $${ACELITE_CORE_PATH}/modules/module_manager.cpp \
        $${ACELITE_CORE_PATH}/modules/presets/cjson_parser.cpp \
        $${ACELITE_CORE_PATH}/modules/presets/console_log_impl.cpp \
        $${ACELITE_CORE_PATH}/modules/presets/console_module.cpp \
        $${ACELITE_CORE_PATH}/modules/presets/date_time_format_module.cpp \
        $${ACELITE_CORE_PATH}/modules/presets/intl_module.cpp \
        $${ACELITE_CORE_PATH}/modules/presets/localization_module.cpp \
        $${ACELITE_CORE_PATH}/modules/presets/number_format_module.cpp \
        $${ACELITE_CORE_PATH}/modules/presets/preset_module.cpp \
        $${ACELITE_CORE_PATH}/modules/presets/profiler_module.cpp \
        $${ACELITE_CORE_PATH}/modules/presets/render_module.cpp \
        $${ACELITE_CORE_PATH}/modules/presets/require_module.cpp \
        $${ACELITE_CORE_PATH}/modules/presets/timer_module.cpp \
        $${ACELITE_CORE_PATH}/modules/presets/version_module.cpp \
        $${ACELITE_CORE_PATH}/modules/router_module.cpp \
        $${ACELITE_CORE_PATH}/modules/sample_module.cpp \
        $${ACELITE_CORE_PATH}/resource/video_muted_image_res.cpp \
        $${ACELITE_CORE_PATH}/resource/video_play_image_res.cpp \
        $${ACELITE_CORE_PATH}/router/js_page_state.cpp \
        $${ACELITE_CORE_PATH}/router/js_page_state_machine.cpp \
        $${ACELITE_CORE_PATH}/router/js_router.cpp \
        $${ACELITE_CORE_PATH}/stylemgr/app_style.cpp \
        $${ACELITE_CORE_PATH}/stylemgr/app_style_item.cpp \
        $${ACELITE_CORE_PATH}/stylemgr/app_style_list.cpp \
        $${ACELITE_CORE_PATH}/stylemgr/app_style_manager.cpp \
        $${ACELITE_CORE_PATH}/stylemgr/app_style_sheet.cpp \
        $${ACELITE_CORE_PATH}/wrapper/js.cpp \
        $${ACELITE_FRAMEWORK_PATH}/targets/platform_adapter.cpp \
        #$${ACELITE_CORE_PATH}/modules/presets/feature_ability_module.cpp \
        targets/simulator/mock/async_work_manager.cpp \
        targets/simulator/mock/hal_sys_param.cpp \
        targets/simulator/mock/js_async_work.cpp

HEADERS += \
        $${ROOT_PATH}/foundation/graphic/ui/interfaces/kits/components/ui_view.h \
        targets/simulator/mock/async_work_manager.h


INCLUDEPATH += \
            $${ROOT_PATH}/foundation/graphic/ui/interfaces/innerkits \
            $${ROOT_PATH}/foundation/graphic/ui/interfaces/innerkits/common \
            $${ROOT_PATH}/foundation/graphic/ui/interfaces/innerkits/dock \
            $${ROOT_PATH}/foundation/graphic/ui/interfaces/innerkits/font \
            $${ROOT_PATH}/foundation/graphic/ui/interfaces/kits \
            $${ROOT_PATH}/foundation/graphic/ui/interfaces/kits/animator \
            $${ROOT_PATH}/foundation/graphic/ui/interfaces/kits/common \
            $${ROOT_PATH}/foundation/graphic/ui/interfaces/kits/components \
            $${ROOT_PATH}/foundation/graphic/ui/interfaces/kits/dfx \
            $${ROOT_PATH}/foundation/graphic/ui/interfaces/kits/events \
            $${ROOT_PATH}/foundation/graphic/ui/interfaces/kits/font \
            $${ROOT_PATH}/foundation/graphic/ui/interfaces/kits/layout \
            $${ROOT_PATH}/foundation/graphic/ui/interfaces/kits/themes \
            $${ROOT_PATH}/foundation/graphic/utils/frameworks/windows \
            $${ROOT_PATH}/foundation/graphic/utils/interfaces/innerkits \
            $${ROOT_PATH}/foundation/graphic/utils/interfaces/kits \
            $${ROOT_PATH}/foundation/graphic/ui/interfaces/innerkits/ui/common \
            $${ROOT_PATH}/foundation/communication/ipc_lite/interfaces/kits \
            $${ROOT_PATH}/base/hiviewdfx/hilog_lite/interfaces/native/kits \
            $${ROOT_PATH}/foundation/aafwk/aafwk_lite/interfaces/kits/ability_lite \
            $${ROOT_PATH}/foundation/aafwk/aafwk_lite/interfaces/kits/want_lite \
            $${ROOT_PATH}/foundation/aafwk/aafwk_lite/interfaces/innerkits/abilitymgr_lite \
            $${ROOT_PATH}/foundation/appexecfwk/appexecfwk_lite/interfaces/kits/bundle_lite \
            $${ROOT_PATH}/foundation/appexecfwk/appexecfwk_lite/utils/bundle_lite \
            $${ROOT_PATH}/foundation/communication/ipc_lite/liteipc/include \
            $${ROOT_PATH}/third_party/bounds_checking_function/include \
            $${ROOT_PATH}/foundation/ace/ace_engine_lite/interfaces/innerkits/builtin/async \
            $${ROOT_PATH}/foundation/ace/ace_engine_lite/interfaces/innerkits/builtin/base \
            $${ROOT_PATH}/foundation/ace/ace_engine_lite/interfaces/innerkits/builtin/jsi \
            $${ACELITE_FRAMEWORK_PATH}/include/context \
            $${ACELITE_FRAMEWORK_PATH}/include/base \
            $${ACELITE_FRAMEWORK_PATH}/include/modules \
            $${ACELITE_FRAMEWORK_PATH}/include/resource \
            $${ACELITE_CORE_PATH} \
            $${ACELITE_CORE_PATH}/animation \
            $${ACELITE_CORE_PATH}/base \
            $${ACELITE_CORE_PATH}/base/cache \
            $${ACELITE_CORE_PATH}/components \
            $${ACELITE_CORE_PATH}/context \
            $${ACELITE_CORE_PATH}/directive \
            $${ACELITE_CORE_PATH}/router \
            $${ACELITE_CORE_PATH}/jsi \
            $${ACELITE_CORE_PATH}/modules \
            $${ACELITE_CORE_PATH}/router \
            $${ACELITE_CORE_PATH}/stylemgr \
            $${ACELITE_CORE_PATH}/wrapper \
            $${ACELITE_FRAMEWORK_PATH}/targets \
            targets/simulator \
            targets/simulator/mock \
            $${ROOT_PATH}/kernel/liteos_a/kernel/include/ \
            $${ROOT_PATH}/kernel/liteos_a/kernel/common \
            $${ROOT_PATH}/third_party/jerryscript/jerry-core/include \
            $${ROOT_PATH}/third_party/jerryscript/jerry-ext/include/jerryscript-ext \
            $${ROOT_PATH}//third_party/jerryscript/jerry-port/default/include \
            $${ROOT_PATH}/third_party/cJSON \
            $${ROOT_PATH}/utils/native/lite/timer_task/include/ \
            $${ROOT_PATH}/third_party/freetype/include \
            $${ROOT_PATH}/base/global/resmgr_lite/interfaces/innerkits/include \
            $${ROOT_PATH}/base/global/i18n_lite/interfaces/kits/i18n/include \
            $${ROOT_PATH}/foundation/graphic/ui/interfaces/kits/ui \
            $${ROOT_PATH}/foundation/graphic/ui/interfaces/kits/config \
            $${ROOT_PATH}/foundation/graphic/ui/interfaces/kits/ui/animator \
            $${ROOT_PATH}/foundation/graphic/ui/interfaces/kits/ui/common \
            $${ROOT_PATH}/foundation/graphic/ui/interfaces/kits/ui/components \
            $${ROOT_PATH}/foundation/graphic/ui/interfaces/kits/ui/dfx \
            $${ROOT_PATH}/foundation/graphic/ui/interfaces/kits/ui/events \
            $${ROOT_PATH}/foundation/graphic/ui/interfaces/kits/ui/font \
            $${ROOT_PATH}/foundation/graphic/ui/interfaces/kits/ui/layout \
            $${ROOT_PATH}/foundation/graphic/ui/interfaces/kits/ui/themes \
            $${ROOT_PATH}/foundation/graphic/ui/interfaces/innerkits/ui/common \
            $${ROOT_PATH}/foundation/communication/ipc_lite/interfaces/kits \
            $${ROOT_PATH}/base/hiviewdfx/hilog_lite/interfaces/native/kits \
            $${ROOT_PATH}/foundation/aafwk/aafwk_lite/frameworks/abilitymgr_lite/include \
            $${ROOT_PATH}/foundation/communication/ipc_lite/liteipc/include \

LIBS += $$DESTDIR/jerryScript.dll
LIBS += $$OUT_PWD/../../../../../../../graphic/ui/tools/qt/simulator/libs/libui.dll