/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifdef JERRY_IAR_JUPITER

#include "jerryscript.h"
#include "jmem.h"
#include "config-jupiter.h"

uint8_t* input_buffer;
uint8_t* snapshot_buffer;
uint8_t* bms_context_and_heap;
uint8_t* js_context_and_heap;

void JerryPsRamMemInit()
{
  // memory for input_js file and snapshot file
  input_buffer = OhosMalloc(MEM_TYPE_JERRY_LSRAM, INPUTJS_BUFFER_SIZE);
  snapshot_buffer = OhosMalloc(MEM_TYPE_JERRY_LSRAM, SNAPSHOT_BUFFER_SIZE);
  bms_context_and_heap = OhosMalloc(MEM_TYPE_JERRY_LSRAM, BMS_TASK_CONTEXT_AND_HEAP_SIZE * CONVERTION_RATIO);
  js_context_and_heap = OhosMalloc(MEM_TYPE_JERRY_LSRAM, JS_TASK_CONTEXT_AND_HEAP_SIZE_BYTE);
}

#endif // JERRY_IAR_JUPITER
