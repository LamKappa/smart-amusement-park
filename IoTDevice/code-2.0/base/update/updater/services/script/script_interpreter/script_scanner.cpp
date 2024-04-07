/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "scanner.h"
#include "pkg_manager.h"

using namespace hpackage;

namespace uscript {
int Scanner::LexerInput(char *buf, int maxSize)
{
    size_t readLen = 0;
    PkgBuffer data = {reinterpret_cast<uint8_t*>(buf), static_cast<size_t>(maxSize)};
    (void)pkgStream_->Read(data, currPos, maxSize, readLen);
    currPos += readLen;
    return readLen;
}
} // namespace uscript
