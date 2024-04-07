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

%{
#include <iostream>
#include <cerrno>
#include <climits>
#include <cstdlib>
#include <string>

#include "parser.hpp"  // 包含由parser.y生成的头文件
#include "scanner.h"   // 包含yyFlexLexer子类的头文件
#include "location.hh" // 包含位置调试信息头文件

/* 定义了YY_USER_ACTION，该宏在每个记号的语义动作之前被调用，来根据记号的长度设置位置的信息 */
#define YY_USER_ACTION  loc.columns (yyleng);

using namespace uscript;
#define yyterminate() Parser::make_END(loc);
%}

/* 声明使用C++版本FLEXER */
%option c++

%option noyywrap

/* 使用Scanner::yylex() */
%option yyclass="Scanner"

/* 一些与编译常量使用该前缀否则为yy */
%option prefix="script_"

%%
%{
  // C++ 兼容的词法分析器的规则，step函数把位置的起始值设置为与结束值相等
  loc.step();
%}

"#".*    {
            loc.step(); // 注释
}

"/*"([^\*]|(\*)*[^\*/])*(\*)*"*/" {
            loc.step(); // 注释
}

"//".* |
[ \t]     {
            /* 跳过注释和空白符号 
             * step函数把位置的起始值设置为与结束值相等，这样位置就指向了上一个极少的结束位置。
             * 由于注释和空白符号识别后并不会返回，而前一个step的调用是在上一次yylex返回时，所以此处需要手动更新记号的起始位置
             */
            loc.step();
          }
\n        {
            loc.lines(yyleng); // 使用lines函数来更新位置信息中的行号
            loc.step();
          }

"function"  { return Parser::make_FUNCTION(yytext, loc); }
"for"       { return Parser::make_FOR(yytext, loc); }
"while"     { return Parser::make_WHILE(yytext, loc); }
"if"        { return Parser::make_IF(yytext, loc); } //return IF;
"else"      { return Parser::make_ELSE(yytext, loc); } //return ELSE;
"+"         { return Parser::make_ADD(yytext, loc); } //return ADD;
"-"         { return Parser::make_SUB(yytext, loc); } //return SUB;
"*"         { return Parser::make_MUL(yytext, loc); } //return MUL;
"/"         { return Parser::make_DIV(yytext, loc); } //return DIV;
"="         { return Parser::make_ASSIGN(yytext, loc); } //return ASSIGN;
"=="        { return Parser::make_EQ(yytext, loc); } //return EQ;
"&&"        { return Parser::make_AND(yytext, loc); } //return AND;
"||"        { return Parser::make_OR(yytext, loc); } //return OR;
"!="        { return Parser::make_NE(yytext, loc); } //return NE;
">"         { return Parser::make_GT(yytext, loc); } //return GT;
">="        { return Parser::make_GE(yytext, loc); } //return GE;
"<"         { return Parser::make_LT(yytext, loc); } //return LT;
"<="        { return Parser::make_LE(yytext, loc); } //return LE;
"("         { return Parser::make_LP(yytext, loc); } //return LP;
")"         { return Parser::make_RP(yytext, loc); } //return RP;
"{"         { return Parser::make_LC(yytext, loc); } //return LC;
"}"         { return Parser::make_RC(yytext, loc); } //return RC;
";"         { return Parser::make_SEMICOLON(yytext, loc); } //return SEMICOLON;
"break"     { return Parser::make_BREAK(yytext, loc); } //return BREAK;
"continue"  { return Parser::make_CONTINUE(yytext, loc); } //return CONTINUE;
"return"    { return Parser::make_RETURN(yytext, loc); } //return RETURN;
","         { return Parser::make_COMMA(yytext, loc); } //return COMMA;

[A-Za-z_][A-Za-z_0-9]* {
    return Parser::make_IDENTIFIER(yytext, loc);
}
([1-9][0-9]*)|"0" {
    return Parser::make_NUMBER(std::strtol(yytext,(&yytext+yyleng), 10),loc);
}
[0-9]+\.[0-9]+ {
    char* end = nullptr;
    return Parser::make_FLOAT(std::strtof(yytext, &end), loc);
}

\"[^\n"]+\" {
    return Parser::make_STRING(std::string(yytext + 1, yyleng - 2), loc);
}
<<EOF>>   {
    return yyterminate();
}

.   {
    loc.step();
}
%%
