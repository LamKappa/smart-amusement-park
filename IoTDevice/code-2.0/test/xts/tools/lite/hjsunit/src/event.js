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
class SpecEvent {
  constructor (attr) {
    this.id = attr.id
    this.coreContext = attr.coreContext
  }

  specStart (specContext) {
    console.info('start running case \'' + specContext.description + '\'')
  }

  specDone (specContext) {
    // 获取报告打印服务
    let reportService = this.coreContext.getDefaultService('report')
    if (specContext.error) {
      reportService.formatPrint('fail', specContext.description)
      reportService.formatPrint('failDetail', specContext.error)
    } else if (specContext.result) {
      if (specContext.result.failExpects.length > 0) {
        reportService.formatPrint('fail', specContext.description)
        specContext.result.failExpects.forEach(failExpect => {
          const msg = failExpect.message || ('expect ' + failExpect.actualValue + ' ' + failExpect.checkFunc + ' ' + (failExpect.expectValue || ''))
          reportService.formatPrint('failDetail', msg)
        })
      } else {
        reportService.formatPrint('pass', specContext.description)
      }
    }
  }
}

class SuiteEvent {
  constructor (attr) {
    this.id = attr.id
  }

  suiteStart (suiteContext) {
    console.info('[suite start]' + suiteContext.description)
  }

  suiteDone () {
    console.info('[suite end]')
  }
}

class TaskEvent {
  constructor (attr) {
    this.id = attr.id
  }

  taskStart () {
    console.info('[start] start run suites')
  }

  taskDone () {

    console.info('[end] run suites end')
  }
}

export { SpecEvent, TaskEvent, SuiteEvent }
