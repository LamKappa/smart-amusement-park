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

/* 使用指令％skeleton "lalr1.cc"选择C++解析器的骨架 */
%skeleton "lalr1.cc"

/* 指定bison的版本 */
%require "3.0.4"

%define api.namespace {uscript} //声明命名空间与下面声明的类名结合使用 uscript::Parser::  在scanner.l中有体现
%define api.parser.class { Parser }
%define api.token.constructor
%define api.value.type variant //使得类型与token定义可以使用各种复杂的结构与类型
%defines  //生成各种头文件  location.hh position.hh  parser.hpp

%code requires
{
  /*requires中的内容会放在YYLTYPE与YYSTPYPE定义前*/
  #include <iostream>
  #include <string>
  #include <vector>
  #include <stdint.h>
  #include <cmath>
  #include "location.hh"
  #include "script_statement.h"
  #include "script_param.h"
  #include "script_function.h"
  #include "script_expression.h"

  using std::string;

  namespace uscript { /*避免包含头文件时冲突 */
    class Scanner;
    class ScriptInterpreter;
  }

}

%code top
{
  /* 尽可能放在parser.cpp靠近头部的地方，与requires相似 */
  #include <iostream>
  #include "scanner.h"
  #include "parser.hpp"
  #include "script_interpreter.h"
  #include "log.h"

  /* 注意：这里的参数由%parse-param决定 */
  static uscript::Parser::symbol_type yylex(uscript::Scanner* scanner, uscript::ScriptInterpreter* interpreter){
    return scanner->nextToken();
  }

  using namespace std;
  using namespace uscript;
  using namespace updater;
}

%{
void Parser::error (const location_type& loc, const std::string& msg)
{
    LOG(updater::ERROR) << "error " << msg << "  loc "  << loc << std::endl;
}
%}

/*定义parser传给scanner的参数*/
%lex-param { uscript::Scanner* scanner }
%lex-param { uscript::ScriptInterpreter* interpreter }

/*定义interpreter传给parser的参数*/
%parse-param { uscript::Scanner* scanner }
%parse-param { uscript::ScriptInterpreter* interpreter }

%locations

/*详细显示错误信息*/
%define parse.error verbose

/*通过Marker::Parser::make_XXX(loc)给token添加前缀*/
%define api.token.prefix {TOKEN_}

%token <int> NUMBER
%token <float> FLOAT

%token EOL
%token END 0

%token  <string>VAR FUNCTION GLOBAL FOR WHILE IF ELSE ADD SUB MUL DIV ASSIGN AND OR
        EQ NE GT GE LT LE LP RP LC RC SEMICOLON IDENTIFIER
        BREAK CONTINUE RETURN COMMA STRING

%type <ScriptParams*>arglist

%type <ScriptFunction*>function_definition

%type <UScriptExpression*> definition_or_statement
        expression value_expression compare_expression add_sub_expression mul_div_expression
        primary_expression expression_option arg

%type <UScriptStatementList*> block statement_list

%type <UScriptStatement*> expression_statement return_statement continue_statement break_statement
                for_statement while_statement statement if_statement

%%
translation_unit: definition_or_statement
        | translation_unit definition_or_statement
        ;
definition_or_statement:function_definition
        {
                interpreter->AddFunction($1);
        }
        |statement
        {
                interpreter->AddStatement($1);
        }
        ;
function_definition: FUNCTION IDENTIFIER LP arglist RP block
        {
                $$ = ScriptFunction::CreateInstance($2, $4, $6);
        }
        |
        FUNCTION IDENTIFIER LP RP block
        {
                $$ = ScriptFunction::CreateInstance($2, nullptr, $5);
        }
        ;
statement:expression_statement
        |for_statement
        |while_statement
        |if_statement
        |break_statement
        |continue_statement
        |return_statement
        ;
expression_statement:expression SEMICOLON
        {
                $$ = UScriptStatement::CreateExpressionStatement($1);
        }
        ;
expression: value_expression
        |IDENTIFIER ASSIGN expression
        {
                $$ = AssignExpression::CreateExpression($1, $3);
        }
        |IDENTIFIER COMMA IDENTIFIER ASSIGN expression
        {
                $$ = AssignExpression::CreateExpression($1, $5);
                AssignExpression::AddIdentifier($$, $3);
        }
        |IDENTIFIER COMMA IDENTIFIER COMMA IDENTIFIER ASSIGN expression
        {
                $$ = AssignExpression::CreateExpression($1, $7);
                AssignExpression::AddIdentifier($$, $3);
                AssignExpression::AddIdentifier($$, $5);
        }
        |IDENTIFIER COMMA IDENTIFIER COMMA IDENTIFIER COMMA IDENTIFIER ASSIGN expression
        {
                $$ = AssignExpression::CreateExpression($1, $9);
                AssignExpression::AddIdentifier($$, $3);
                AssignExpression::AddIdentifier($$, $5);
                AssignExpression::AddIdentifier($$, $7);
        }
        ;
value_expression: compare_expression
        |value_expression EQ compare_expression
        {
                $$ = BinaryExpression::CreateExpression(UScriptExpression::EQ_OPERATOR, $1, $3);
        }
        |value_expression NE compare_expression
        {
                $$ = BinaryExpression::CreateExpression(UScriptExpression::NE_OPERATOR, $1, $3);
        }
        |value_expression AND compare_expression
        {
                $$ = BinaryExpression::CreateExpression(UScriptExpression::AND_OPERATOR, $1, $3);
        }
        |value_expression OR compare_expression
        {
                $$ = BinaryExpression::CreateExpression(UScriptExpression::OR_OPERATOR, $1, $3);
        }
        ;
compare_expression:add_sub_expression
        |compare_expression GT add_sub_expression
        {
                $$ = BinaryExpression::CreateExpression(UScriptExpression::GT_OPERATOR, $1, $3);
        }
        |compare_expression GE add_sub_expression
        {
                $$ = BinaryExpression::CreateExpression(UScriptExpression::GE_OPERATOR, $1, $3);
        }
        |compare_expression LT add_sub_expression
        {
                $$ = BinaryExpression::CreateExpression(UScriptExpression::LT_OPERATOR, $1, $3);
        }
        |compare_expression LE add_sub_expression
        {
                $$ = BinaryExpression::CreateExpression(UScriptExpression::LE_OPERATOR, $1, $3);
        }
        ;
add_sub_expression:mul_div_expression
        |add_sub_expression ADD mul_div_expression
        {
                $$ = BinaryExpression::CreateExpression(UScriptExpression::ADD_OPERATOR, $1, $3);
        }
        |add_sub_expression SUB mul_div_expression
        {
                $$ = BinaryExpression::CreateExpression(UScriptExpression::SUB_OPERATOR, $1, $3);
        }
        ;
mul_div_expression:primary_expression
        |mul_div_expression DIV primary_expression
        {
                $$ = BinaryExpression::CreateExpression(UScriptExpression::DIV_OPERATOR, $1, $3);
        }
        |mul_div_expression MUL primary_expression
        {
                $$ = BinaryExpression::CreateExpression(UScriptExpression::MUL_OPERATOR, $1, $3);
        }
        ;
primary_expression:SUB primary_expression
        {
                $$=$2;
        }
        |LP expression RP
        {
                $$=$2;
        }
        |IDENTIFIER
        {
                $$ = IdentifierExpression::CreateExpression($1);
        }
        |STRING
        {
                $$ = StringExpression::CreateExpression($1);
        }
        |NUMBER
        {
                $$ = IntegerExpression::CreateExpression($1);
        }
        |FLOAT
        {
                $$ = FloatExpression::CreateExpression($1);
        }
        |IDENTIFIER LP RP
        {
                $$ = FunctionCallExpression::CreateExpression($1, nullptr);
        }
        |IDENTIFIER LP arglist RP
        {
                $$ = FunctionCallExpression::CreateExpression($1, $3);
        }
        ;
statement_list:statement_list statement
        {
                $1->AddScriptStatement($2);
                $$ = $1;
        }
        |statement
        {
                $$ = UScriptStatementList::CreateInstance($1);
        }
        ;
block:LC RC
        {
                $$=nullptr;
        }
        |LC statement_list RC
        {
                $$=$2;
        }
        ;
arglist:arglist COMMA arg
        {
                $$ = ScriptParams::AddParams($1, $3);
        }
        |arg
        {
                $$ = ScriptParams::CreateParams($1);
        }
        ;
arg:    value_expression
        ;
expression_option:
        {
                $$=nullptr;
        }
        |expression
        ;
for_statement: FOR LP expression_option SEMICOLON expression_option SEMICOLON expression_option  RP block
        {
                $$ = UScriptStatement::CreateForStatement($3,$5,$7,$9);
        }
        ;
while_statement:  WHILE LP expression_option RP block
	{
		$$ = UScriptStatement::CreateWhileStatement($3, (UScriptStatementList*)$5);
	}
	;
if_statement: IF LP expression RP block
        {
               $$ = UScriptStatement::CreateIfStatement($3,$5);
        }
        | IF LP expression RP block ELSE if_statement
        {
                $$ = UScriptStatement::CreateIfStatement($3,$5, nullptr, $7);
        }
        | IF LP expression RP block ELSE block
        {
                $$ = UScriptStatement::CreateIfStatement($3,$5, $7);
        }
        ;
break_statement:BREAK SEMICOLON
        {
                $$ = UScriptStatement::CreateStatement(UScriptStatement::STATEMENT_TYPE_BREAK);
        }
        ;
continue_statement:CONTINUE SEMICOLON
        {
                //$$=create_Statement(STATEMENT_TYPE_CONTINUE);
                $$ = UScriptStatement::CreateStatement(UScriptStatement::STATEMENT_TYPE_CONTINUE);
        }
        ;
return_statement:RETURN arglist SEMICOLON
        {
                $$ = UScriptReturnStatement::CreateStatement($2);
        }
        | RETURN SEMICOLON
        {
                $$ = UScriptReturnStatement::CreateStatement(nullptr);
        }
        ;