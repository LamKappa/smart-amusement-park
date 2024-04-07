/* Copyright JS Foundation and other contributors, http://js.foundation
 *
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

#include "jerryscript-port.h"
#include "jerryscript-port-default.h"

#ifdef JERRY_FOR_IAR_CONFIG

#include "generate-bytecode.h"
#include "los_task.h"

/**
 * use static Array to record the correspondence between task id and jerry-heap/context
 */
#define MAX_CONTEXT_NUM 8
#define CONTEXT_NO_EXIST -1

static uint8_t g_contextRecordCount = 0;
static ContextRecord g_contextRecords[MAX_CONTEXT_NUM] = {0};

uint8_t getContextRecordCount() {
  return g_contextRecordCount;
}

ContextRecord* getContextRecord() {
  return g_contextRecords;
}

/**
 * set context function: store task id and context
 */
void
jerry_port_default_set_current_context (jerry_context_t *context_p) /**< store created context */
{
  LOS_TaskLock();
  uint32_t curTaskId = LOS_CurTaskIDGet();
  g_contextRecordCount++;
  g_contextRecords[g_contextRecordCount-1].task_id = curTaskId;
  g_contextRecords[g_contextRecordCount-1].context_p = context_p;
  LOS_TaskUnlock();
}

int get_context_record_index(void)
{
  uint32_t curTaskId = LOS_CurTaskIDGet();

  for (int i = 0; i < g_contextRecordCount; i++) {
    if (g_contextRecords[i].task_id == curTaskId) {
      return i;
    }
  }
  return CONTEXT_NO_EXIST;
}

/**
 * when task over, delete its record in array
 */
void
jerry_port_default_remove_current_context_record () /**< remove current task's context record in Array */
{
  LOS_TaskLock();

  int index = get_context_record_index();

  for (int i = index; i < g_contextRecordCount-1; i++) {
    g_contextRecords[i].task_id = g_contextRecords[i+1].task_id;
    g_contextRecords[i].context_p = g_contextRecords[i+1].context_p;
  }
  memset_s(&g_contextRecords[g_contextRecordCount-1], 0, sizeof(g_contextRecords[g_contextRecordCount-1]));
  g_contextRecordCount--;
  LOS_TaskUnlock();
}

/**
 * key: rewrite get_current_context function; get the context pointer according to task id
 */
jerry_context_t *
jerry_port_get_current_context (void) /**< points to current task's context */
{
  jerry_context_t * context = NULL;
  LOS_TaskLock();

  int index = get_context_record_index();

  context = g_contextRecords[index].context_p;

  LOS_TaskUnlock();
  return context;
}

#else // not defined JERRY_FOR_IAR_CONFIG

/**
 * Pointer to the current context.
 * Note that it is a global variable, and is not a thread safe implementation.
 */
static jerry_context_t *current_context_p = NULL;

/**
 * Set the current_context_p as the passed pointer.
 */
void
jerry_port_default_set_current_context (jerry_context_t *context_p) /**< points to the created context */
{
  current_context_p = context_p;
} /* jerry_port_default_set_current_context */

/**
 * Get the current context.
 *
 * @return the pointer to the current context
 */
jerry_context_t *
jerry_port_get_current_context (void)
{
  return current_context_p;
} /* jerry_port_get_current_context */

#endif
