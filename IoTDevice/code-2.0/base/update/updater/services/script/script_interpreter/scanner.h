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

#ifndef FLEX_BISON_SCANNER_H
#define FLEX_BISON_SCANNER_H

#include <cstring>
#include "script_utils.h"

/* 重要 */
#if !defined(yyFlexLexerOnce)
#undef yyFlexLexer
#define yyFlexLexer script_FlexLexer // 根据prefix修改
#include "FlexLexer.h"
#endif
#include "parser.hpp"

/* 替换默认的get_next_token定义 */
#undef YY_DECL
#define YY_DECL uscript::Parser::symbol_type uscript::Scanner::nextToken()

namespace uscript {
class ScriptInterpreter;

class Scanner : public yyFlexLexer {
public:
    explicit Scanner(ScriptInterpreter* interpreter)
    {
        loc = location();
    }
    virtual ~Scanner() {}
    // 不需要手动实现这个函数，Flex会生成YY_DECL宏定义的代码来实现这个函数
    virtual uscript::Parser::symbol_type nextToken();
    virtual int yywrap()
    {
        return 1;
    }

    virtual int LexerInput(char* buf, int maxSize);

    void SetPkgStream(hpackage::PkgManager::StreamPtr pkgStream)
    {
        pkgStream_ = pkgStream;
    }

private:
    hpackage::PkgManager::StreamPtr pkgStream_ = nullptr;
    size_t currPos = 0;
    location loc {};
};
} // namespace uscript
#endif // FLEX_BISON_SCANNER_H
