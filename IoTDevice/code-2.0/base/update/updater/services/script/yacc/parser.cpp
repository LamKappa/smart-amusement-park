/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

/* 尽可能放在parser.cpp靠近头部的地方，与requires相似 */
#include <iostream>
#include "scanner.h"
#include "parser.hpp"
#include "script_interpreter.h"
#include "log.h"

/* 注意：这里的参数由%parse-param决定 */
static uscript::Parser::symbol_type yylex(uscript::Scanner* scanner, uscript::ScriptInterpreter* interpreter) {
    return scanner->nextToken();
}

using namespace std;
using namespace uscript;
using namespace updater;

// First part of user prologue.

void Parser::error(const location_type& loc, const std::string& msg)
{
    LOG(updater::ERROR) << "error " << msg << "  loc " << loc << std::endl;
}

#include "parser.hpp"

#ifndef YY_
    #if defined YYENABLE_NLS && YYENABLE_NLS
        #if ENABLE_NLS
            #include <libintl.h> // FIXME: INFRINGES ON USER NAME SPACE.
            #define YY_(msgid)dgettext ("bison-runtime", msgid)
        #endif
    #endif
    #ifndef YY_
        #define YY_(msgid)msgid
    #endif
#endif

// Whether we are compiled with exception support.
#ifndef YY_EXCEPTIONS
    #if defined __GNUC__ && !defined __EXCEPTIONS
        #define YY_EXCEPTIONS 0
    #else
        #define YY_EXCEPTIONS 1
    #endif
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K].location)
/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
    If N is 0, then set CURRENT to the empty location which ends
    the previous symbol: RHS[0] (always defined).  */

#ifndef YYLLOC_DEFAULT
#define YYLLOC_DEFAULT(Current, Rhs, N)                          \
    do                                                            \
    if (N)                                                        \
    {                                                             \
        (Current).begin = YYRHSLOC (Rhs, 1).begin;                \
        (Current).end = YYRHSLOC (Rhs, N).end;                    \
    }  else {                                                     \
        (Current).begin = (Current).end = YYRHSLOC (Rhs, 0).end;  \
    }                                                             \
    while (false)
#endif

// Enable debugging if requested.
#if YYDEBUG

// A pseudo ostream that takes yydebug_ into account.
#define YYCDEBUG if (yydebug_)(*yycdebug_)

#define YY_SYMBOL_PRINT(Title, Symbol)       \
    do {                                     \
        if (yydebug_) {                      \
            *yycdebug_ << Title << ' ';      \
            yy_print_ (*yycdebug_, Symbol);  \
            *yycdebug_ << '\n';              \
        }                                    \
    } while (false)

#define YY_REDUCE_PRINT(Rule)             \
    do {                                  \
        if (yydebug_)                     \
            yy_reduce_print_ (Rule);      \
    } while (false)

#define YY_STACK_PRINT()             \
    do {                             \
        if (yydebug_)                \
            yy_stack_print_ ();      \
    } while (false)

#else // !YYDEBUG

#define YYCDEBUG if (false) std::cerr
#define YY_SYMBOL_PRINT(Title, Symbol) YYUSE(Symbol)
#define YY_REDUCE_PRINT(Rule)          static_cast<void> (0)
#define YY_STACK_PRINT()               static_cast<void> (0)

#endif // !YYDEBUG

#define yyerrok         (yyerrstatus_ = 0)
#define yyclearin       (yyla.clear ())

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYRECOVERING() (!!yyerrstatus_)

namespace uscript {

// Build a parser object.
Parser::Parser(uscript::Scanner* scanner_yyarg, uscript::ScriptInterpreter* interpreter_yyarg)
#if YYDEBUG
    : yydebug_(false),
    yycdebug_(&std::cerr),
#else
    :
#endif
    scanner(scanner_yyarg),
    interpreter(interpreter_yyarg) {}
    Parser::~Parser() {}

    Parser::syntax_error::~syntax_error() YY_NOEXCEPT YY_NOTHROW {}

    /*---------------.
    | symbol kinds.  |
    `---------------*/

    // by_state.
    Parser::by_state::by_state() YY_NOEXCEPT
        : state(empty_state) {}

    Parser::by_state::by_state(const by_state& that) YY_NOEXCEPT
        : state(that.state) {}

    void Parser::by_state::clear() YY_NOEXCEPT
    {
        state = empty_state;
    }

    void Parser::by_state::move(by_state& that)
    {
        state = that.state;
        that.clear();
    }

    Parser::by_state::by_state(state_type s) YY_NOEXCEPT
        : state(s) {}

    Parser::symbol_kind_type
    Parser::by_state::kind() const YY_NOEXCEPT
    {
        if (state == empty_state) {
            return symbol_kind::S_YYEMPTY;
        } else {
            return YY_CAST (symbol_kind_type, yystos_[+state]);
        }
    }

    Parser::stack_symbol_type::stack_symbol_type() {}

    Parser::stack_symbol_type::stack_symbol_type(YY_RVREF (stack_symbol_type)that)
        : super_type(YY_MOVE(that.state), YY_MOVE(that.location))
    {
        switch (that.kind()) {
            case symbol_kind::S_function_definition: // function_definition
                value.YY_MOVE_OR_COPY< ScriptFunction* > (YY_MOVE (that.value));
                break;

            case symbol_kind::S_arglist: // arglist
                value.YY_MOVE_OR_COPY< ScriptParams* >(YY_MOVE(that.value));
                break;

            case symbol_kind::S_definition_or_statement: // definition_or_statement
                /* fallthrough */
            case symbol_kind::S_expression: // expression
                /* fallthrough */
            case symbol_kind::S_value_expression: // value_expression
                /* fallthrough */
            case symbol_kind::S_compare_expression: // compare_expression
                /* fallthrough */
            case symbol_kind::S_add_sub_expression: // add_sub_expression
                /* fallthrough */
            case symbol_kind::S_mul_div_expression: // mul_div_expression
                /* fallthrough */
            case symbol_kind::S_primary_expression: // primary_expression
                /* fallthrough */
            case symbol_kind::S_arg: // arg
                /* fallthrough */
            case symbol_kind::S_expression_option: // expression_option
                value.YY_MOVE_OR_COPY< UScriptExpression* > (YY_MOVE (that.value));
                break;

            case symbol_kind::S_statement: // statement
                /* fallthrough */
            case symbol_kind::S_expression_statement: // expression_statement
                /* fallthrough */
            case symbol_kind::S_for_statement: // for_statement
                /* fallthrough */
            case symbol_kind::S_while_statement: // while_statement
                /* fallthrough */
            case symbol_kind::S_if_statement: // if_statement
                /* fallthrough */
            case symbol_kind::S_break_statement: // break_statement
                /* fallthrough */
            case symbol_kind::S_continue_statement: // continue_statement
                /* fallthrough */
            case symbol_kind::S_return_statement: // return_statement
                value.YY_MOVE_OR_COPY< UScriptStatement* > (YY_MOVE (that.value));
                break;

            case symbol_kind::S_statement_list: // statement_list
                /* fallthrough */
            case symbol_kind::S_block: // block
                value.YY_MOVE_OR_COPY< UScriptStatementList* > (YY_MOVE (that.value));
                break;

            case symbol_kind::S_FLOAT: // FLOAT
                value.YY_MOVE_OR_COPY< float > (YY_MOVE (that.value));
                break;

            case symbol_kind::S_NUMBER: // NUMBER
                value.YY_MOVE_OR_COPY< int > (YY_MOVE (that.value));
                break;

            case symbol_kind::S_VAR: // VAR
                /* fallthrough */
            case symbol_kind::S_FUNCTION: // FUNCTION
                /* fallthrough */
            case symbol_kind::S_GLOBAL: // GLOBAL
                /* fallthrough */
            case symbol_kind::S_FOR: // FOR
                /* fallthrough */
            case symbol_kind::S_WHILE: // WHILE
                /* fallthrough */
            case symbol_kind::S_IF: // IF
                /* fallthrough */
            case symbol_kind::S_ELSE: // ELSE
                /* fallthrough */
            case symbol_kind::S_ADD: // ADD
                /* fallthrough */
            case symbol_kind::S_SUB: // SUB
                /* fallthrough */
            case symbol_kind::S_MUL: // MUL
                /* fallthrough */
            case symbol_kind::S_DIV: // DIV
                /* fallthrough */
            case symbol_kind::S_ASSIGN: // ASSIGN
                /* fallthrough */
            case symbol_kind::S_AND: // AND
                /* fallthrough */
            case symbol_kind::S_OR: // OR
                /* fallthrough */
            case symbol_kind::S_EQ: // EQ
                /* fallthrough */
            case symbol_kind::S_NE: // NE
                /* fallthrough */
            case symbol_kind::S_GT: // GT
                /* fallthrough */
            case symbol_kind::S_GE: // GE
                /* fallthrough */
            case symbol_kind::S_LT: // LT
                /* fallthrough */
            case symbol_kind::S_LE: // LE
                /* fallthrough */
            case symbol_kind::S_LP: // LP
                /* fallthrough */
            case symbol_kind::S_RP: // RP
                /* fallthrough */
            case symbol_kind::S_LC: // LC
                /* fallthrough */
            case symbol_kind::S_RC: // RC
                /* fallthrough */
            case symbol_kind::S_SEMICOLON: // SEMICOLON
                /* fallthrough */
            case symbol_kind::S_IDENTIFIER: // IDENTIFIER
                /* fallthrough */
            case symbol_kind::S_BREAK: // BREAK
                /* fallthrough */
            case symbol_kind::S_CONTINUE: // CONTINUE
                /* fallthrough */
            case symbol_kind::S_RETURN: // RETURN
                /* fallthrough */
            case symbol_kind::S_COMMA: // COMMA
                /* fallthrough */
            case symbol_kind::S_STRING: // STRING
                value.YY_MOVE_OR_COPY< string > (YY_MOVE (that.value));
                break;

            default:
                break;
        }
#if 201103L <= YY_CPLUSPLUS
        // that is emptied.
        that.state = empty_state;
#endif
    }

    Parser::stack_symbol_type::stack_symbol_type(state_type s, YY_MOVE_REF (symbol_type)that)
        : super_type(s, YY_MOVE(that.location))
    {
        switch(that.kind()) {
            case symbol_kind::S_function_definition: // function_definition
                value.move< ScriptFunction* > (YY_MOVE (that.value));
                break;

            case symbol_kind::S_arglist: // arglist
                value.move< ScriptParams* > (YY_MOVE (that.value));
                break;

            case symbol_kind::S_definition_or_statement: // definition_or_statement
                /* fallthrough */
            case symbol_kind::S_expression: // expression
                /* fallthrough */
            case symbol_kind::S_value_expression: // value_expression
                /* fallthrough */
            case symbol_kind::S_compare_expression: // compare_expression
                /* fallthrough */
            case symbol_kind::S_add_sub_expression: // add_sub_expression
                /* fallthrough */
            case symbol_kind::S_mul_div_expression: // mul_div_expression
                /* fallthrough */
            case symbol_kind::S_primary_expression: // primary_expression
                /* fallthrough */
            case symbol_kind::S_arg: // arg
                /* fallthrough */
            case symbol_kind::S_expression_option: // expression_option
                value.move< UScriptExpression* > (YY_MOVE (that.value));
                break;

            case symbol_kind::S_statement: // statement
                /* fallthrough */
            case symbol_kind::S_expression_statement: // expression_statement
                /* fallthrough */
            case symbol_kind::S_for_statement: // for_statement
                /* fallthrough */
            case symbol_kind::S_while_statement: // while_statement
                /* fallthrough */
            case symbol_kind::S_if_statement: // if_statement
                /* fallthrough */
            case symbol_kind::S_break_statement: // break_statement
                /* fallthrough */
            case symbol_kind::S_continue_statement: // continue_statement
                /* fallthrough */
            case symbol_kind::S_return_statement: // return_statement
                value.move< UScriptStatement* > (YY_MOVE (that.value));
                break;

            case symbol_kind::S_statement_list: // statement_list
                /* fallthrough */
            case symbol_kind::S_block: // block
                value.move< UScriptStatementList* > (YY_MOVE (that.value));
                break;

            case symbol_kind::S_FLOAT: // FLOAT
                value.move< float > (YY_MOVE (that.value));
                break;

            case symbol_kind::S_NUMBER: // NUMBER
                value.move< int > (YY_MOVE (that.value));
                break;

            case symbol_kind::S_VAR: // VAR
                /* fallthrough */
            case symbol_kind::S_FUNCTION: // FUNCTION
                /* fallthrough */
            case symbol_kind::S_GLOBAL: // GLOBAL
                /* fallthrough */
            case symbol_kind::S_FOR: // FOR
                /* fallthrough */
            case symbol_kind::S_WHILE: // WHILE
                /* fallthrough */
            case symbol_kind::S_IF: // IF
                /* fallthrough */
            case symbol_kind::S_ELSE: // ELSE
                /* fallthrough */
            case symbol_kind::S_ADD: // ADD
                /* fallthrough */
            case symbol_kind::S_SUB: // SUB
                /* fallthrough */
            case symbol_kind::S_MUL: // MUL
                /* fallthrough */
            case symbol_kind::S_DIV: // DIV
                /* fallthrough */
            case symbol_kind::S_ASSIGN: // ASSIGN
                /* fallthrough */
            case symbol_kind::S_AND: // AND
                /* fallthrough */
            case symbol_kind::S_OR: // OR
                /* fallthrough */
            case symbol_kind::S_EQ: // EQ
                /* fallthrough */
            case symbol_kind::S_NE: // NE
                /* fallthrough */
            case symbol_kind::S_GT: // GT
                /* fallthrough */
            case symbol_kind::S_GE: // GE
                /* fallthrough */
            case symbol_kind::S_LT: // LT
                /* fallthrough */
            case symbol_kind::S_LE: // LE
                /* fallthrough */ 
            case symbol_kind::S_LP: // LP
                /* fallthrough */
            case symbol_kind::S_RP: // RP
                /* fallthrough */
            case symbol_kind::S_LC: // LC
                /* fallthrough */
            case symbol_kind::S_RC: // RC
                /* fallthrough */
            case symbol_kind::S_SEMICOLON: // SEMICOLON
                /* fallthrough */
            case symbol_kind::S_IDENTIFIER: // IDENTIFIER
                /* fallthrough */
            case symbol_kind::S_BREAK: // BREAK
                /* fallthrough */
            case symbol_kind::S_CONTINUE: // CONTINUE
                /* fallthrough */
            case symbol_kind::S_RETURN: // RETURN
                /* fallthrough */
            case symbol_kind::S_COMMA: // COMMA
                /* fallthrough */
            case symbol_kind::S_STRING: // STRING
                value.move< string > (YY_MOVE (that.value));
                break;

            default:
                break;
        }
        // that is emptied.
        that.kind_ = symbol_kind::S_YYEMPTY;
    }

#if YY_CPLUSPLUS < 201103L
    Parser::stack_symbol_type&
    Parser::stack_symbol_type::operator=(const stack_symbol_type &that)
    {
        state = that.state;
        switch (that.kind()) {
            case symbol_kind::S_function_definition: // function_definition
                value.copy< ScriptFunction* > (that.value);
                break;

            case symbol_kind::S_arglist: // arglist
                value.copy< ScriptParams* > (that.value);
                break;

            case symbol_kind::S_definition_or_statement: // definition_or_statement
                /* fallthrough */
            case symbol_kind::S_expression: // expression
                /* fallthrough */
            case symbol_kind::S_value_expression: // value_expression
                /* fallthrough */
            case symbol_kind::S_compare_expression: // compare_expression
                /* fallthrough */
            case symbol_kind::S_add_sub_expression: // add_sub_expression
                /* fallthrough */
            case symbol_kind::S_mul_div_expression: // mul_div_expression
                /* fallthrough */
            case symbol_kind::S_primary_expression: // primary_expression
                /* fallthrough */
            case symbol_kind::S_arg: // arg
                /* fallthrough */
            case symbol_kind::S_expression_option: // expression_option
                value.copy< UScriptExpression* > (that.value);
                break;

            case symbol_kind::S_statement: // statement
                /* fallthrough */
            case symbol_kind::S_expression_statement: // expression_statement
                /* fallthrough */
            case symbol_kind::S_for_statement: // for_statement
                /* fallthrough */
            case symbol_kind::S_while_statement: // while_statement
                /* fallthrough */
            case symbol_kind::S_if_statement: // if_statement
                /* fallthrough */
            case symbol_kind::S_break_statement: // break_statement
                /* fallthrough */
            case symbol_kind::S_continue_statement: // continue_statement
                /* fallthrough */
            case symbol_kind::S_return_statement: // return_statement
                value.copy< UScriptStatement* > (that.value);
                break;

            case symbol_kind::S_statement_list: // statement_list
                /* fallthrough */
            case symbol_kind::S_block: // block
                value.copy< UScriptStatementList* > (that.value);
                break;

            case symbol_kind::S_FLOAT: // FLOAT
                value.copy< float > (that.value);
                break;

            case symbol_kind::S_NUMBER: // NUMBER
                value.copy< int > (that.value);
                break;

            case symbol_kind::S_VAR: // VAR
                /* fallthrough */
            case symbol_kind::S_FUNCTION: // FUNCTION
                /* fallthrough */
            case symbol_kind::S_GLOBAL: // GLOBAL
                /* fallthrough */
            case symbol_kind::S_FOR: // FOR
                /* fallthrough */
            case symbol_kind::S_WHILE: // WHILE
                /* fallthrough */
            case symbol_kind::S_IF: // IF
                /* fallthrough */
            case symbol_kind::S_ELSE: // ELSE
                /* fallthrough */
            case symbol_kind::S_ADD: // ADD
                /* fallthrough */
            case symbol_kind::S_SUB: // SUB
                /* fallthrough */
            case symbol_kind::S_MUL: // MUL
                /* fallthrough */
            case symbol_kind::S_DIV: // DIV
                /* fallthrough */
            case symbol_kind::S_ASSIGN: // ASSIGN
                /* fallthrough */
            case symbol_kind::S_AND: // AND
                /* fallthrough */
            case symbol_kind::S_OR: // OR
                /* fallthrough */
            case symbol_kind::S_EQ: // EQ
                /* fallthrough */
            case symbol_kind::S_NE: // NE
                /* fallthrough */
            case symbol_kind::S_GT: // GT
                /* fallthrough */
            case symbol_kind::S_GE: // GE
                /* fallthrough */
            case symbol_kind::S_LT: // LT
                /* fallthrough */
            case symbol_kind::S_LE: // LE
                /* fallthrough */
            case symbol_kind::S_LP: // LP
                /* fallthrough */
            case symbol_kind::S_RP: // RP
                /* fallthrough */
            case symbol_kind::S_LC: // LC
                /* fallthrough */
            case symbol_kind::S_RC: // RC
                /* fallthrough */
            case symbol_kind::S_SEMICOLON: // SEMICOLON
                /* fallthrough */
            case symbol_kind::S_IDENTIFIER: // IDENTIFIER
                /* fallthrough */
            case symbol_kind::S_BREAK: // BREAK
                /* fallthrough */
            case symbol_kind::S_CONTINUE: // CONTINUE
                /* fallthrough */
            case symbol_kind::S_RETURN: // RETURN
                /* fallthrough */
            case symbol_kind::S_COMMA: // COMMA
                /* fallthrough */
            case symbol_kind::S_STRING: // STRING
                value.copy< string > (that.value);
                break;

            default:
                break;
        }
        location = that.location;
        return *this;
    }

    Parser::stack_symbol_type& Parser::stack_symbol_type::operator=(stack_symbol_type &that)
    {
        state = that.state;
        switch (that.kind()) {
            case symbol_kind::S_function_definition: // function_definition
                value.move<ScriptFunction*>(that.value);
                break;

            case symbol_kind::S_arglist: // arglist
                value.move<ScriptParams*>(that.value);
                break;

            case symbol_kind::S_definition_or_statement: // definition_or_statement
                /* fallthrough */
            case symbol_kind::S_expression: // expression
                /* fallthrough */
            case symbol_kind::S_value_expression: // value_expression
                /* fallthrough */
            case symbol_kind::S_compare_expression: // compare_expression
                /* fallthrough */
            case symbol_kind::S_add_sub_expression: // add_sub_expression
                /* fallthrough */
            case symbol_kind::S_mul_div_expression: // mul_div_expression
                /* fallthrough */
            case symbol_kind::S_primary_expression: // primary_expression
                /* fallthrough */
            case symbol_kind::S_arg: // arg
                /* fallthrough */
            case symbol_kind::S_expression_option: // expression_option
                value.move<UScriptExpression*>(that.value);
                break;

            case symbol_kind::S_statement: // statement
                /* fallthrough */
            case symbol_kind::S_expression_statement: // expression_statement
                /* fallthrough */
            case symbol_kind::S_for_statement: // for_statement
                /* fallthrough */
            case symbol_kind::S_while_statement: // while_statement
                /* fallthrough */
            case symbol_kind::S_if_statement: // if_statement
                /* fallthrough */
            case symbol_kind::S_break_statement: // break_statement
                /* fallthrough */
            case symbol_kind::S_continue_statement: // continue_statement
                /* fallthrough */
            case symbol_kind::S_return_statement: // return_statement
                value.move<UScriptStatement*>(that.value);
                break;

            case symbol_kind::S_statement_list: // statement_list
                /* fallthrough */
            case symbol_kind::S_block: // block
                value.move<UScriptStatementList*> (that.value);
                break;

            case symbol_kind::S_FLOAT: // FLOAT
                value.move<float> (that.value);
                break;

            case symbol_kind::S_NUMBER: // NUMBER
                value.move<int> (that.value);
                break;

            case symbol_kind::S_VAR: // VAR
                /* fallthrough */
            case symbol_kind::S_FUNCTION: // FUNCTION
                /* fallthrough */
            case symbol_kind::S_GLOBAL: // GLOBAL
                /* fallthrough */
            case symbol_kind::S_FOR: // FOR
                /* fallthrough */
            case symbol_kind::S_WHILE: // WHILE
                /* fallthrough */
            case symbol_kind::S_IF: // IF
                /* fallthrough */
            case symbol_kind::S_ELSE: // ELSE
                /* fallthrough */
            case symbol_kind::S_ADD: // ADD
                /* fallthrough */
            case symbol_kind::S_SUB: // SUB
                /* fallthrough */
            case symbol_kind::S_MUL: // MUL
                /* fallthrough */
            case symbol_kind::S_DIV: // DIV
                /* fallthrough */
            case symbol_kind::S_ASSIGN: // ASSIGN
                /* fallthrough */
            case symbol_kind::S_AND: // AND
                /* fallthrough */
            case symbol_kind::S_OR: // OR
                /* fallthrough */
            case symbol_kind::S_EQ: // EQ
                /* fallthrough */
            case symbol_kind::S_NE: // NE
                /* fallthrough */
            case symbol_kind::S_GT: // GT
                /* fallthrough */
            case symbol_kind::S_GE: // GE
                /* fallthrough */
            case symbol_kind::S_LT: // LT
                /* fallthrough */
            case symbol_kind::S_LE: // LE
                /* fallthrough */
            case symbol_kind::S_LP: // LP
                /* fallthrough */
            case symbol_kind::S_RP: // RP
                /* fallthrough */
            case symbol_kind::S_LC: // LC
                /* fallthrough */
            case symbol_kind::S_RC: // RC
                /* fallthrough */
            case symbol_kind::S_SEMICOLON: // SEMICOLON
                /* fallthrough */
            case symbol_kind::S_IDENTIFIER: // IDENTIFIER
                /* fallthrough */
            case symbol_kind::S_BREAK: // BREAK
                /* fallthrough */
            case symbol_kind::S_CONTINUE: // CONTINUE
                /* fallthrough */
            case symbol_kind::S_RETURN: // RETURN
                /* fallthrough */
            case symbol_kind::S_COMMA: // COMMA
                /* fallthrough */
            case symbol_kind::S_STRING: // STRING
                value.move<string>(that.value);
                break;

            default:
                break;
        }
        location = that.location;
        // that is emptied.
        that.state = empty_state;
        return *this;
    }
#endif

    template <typename Base>
    void Parser::yy_destroy_(const char *yymsg, basic_symbol<Base> &yysym) const
    {
        if (yymsg) {
            YY_SYMBOL_PRINT (yymsg, yysym);
        }
    }

#if YYDEBUG
    template <typename Base>
    void Parser::yy_print_(std::ostream& yyo, const basic_symbol<Base>& yysym) const
    {
        std::ostream &yyoutput = yyo;
        YYUSE(yyoutput);
        if (yysym.empty ()) {
            yyo << "empty symbol";
        } else {
            symbol_kind_type yykind = yysym.kind ();
            yyo << (yykind < YYNTOKENS ? "token" : "nterm") <<
                ' ' << yysym.name () << " (" << yysym.location << ": ";
            YYUSE (yykind);
            yyo << ')';
        }
    }
#endif

    void Parser::yypush_(const char *m, YY_MOVE_REF (stack_symbol_type)sym)
    {
        if (m) {
            YY_SYMBOL_PRINT (m, sym);
        }
        yystack_.push (YY_MOVE (sym));
    }

    void Parser::yypush_(const char *m, state_type s, YY_MOVE_REF (symbol_type)sym)
    {
#if 201103L <= YY_CPLUSPLUS
        yypush_(m, stack_symbol_type(s, std::move (sym)));
#else
        stack_symbol_type ss(s, sym);
        yypush_(m, ss);
#endif
    }

    void Parser::yypop_(int n)
    {
        yystack_.pop(n);
    }

#if YYDEBUG
    std::ostream& Parser::debug_stream() const
    {
        return *yycdebug_;
    }

    void Parser::set_debug_stream(std::ostream &o)
    {
        yycdebug_ = &o;
    }

    Parser::debug_level_type Parser::debug_level() const
    {
        return yydebug_;
    }

    void Parser::set_debug_level(debug_level_type l)
    {
        yydebug_ = l;
    }
#endif // YYDEBUG

    Parser::state_type Parser::yy_lr_goto_state_(state_type yystate, int yysym)
    {
        int yyr = yypgoto_[yysym - YYNTOKENS] + yystate;
        if (0 <= yyr && yyr <= yylast_ && yycheck_[yyr] == yystate) {
            return yytable_[yyr];
        } else {
            return yydefgoto_[yysym - YYNTOKENS];
        }
    }

    bool Parser::yy_pact_value_is_default_(int yyvalue)
    {
        return yyvalue == yypact_ninf_;
    }

    bool Parser::yy_table_value_is_error_(int yyvalue)
    {
        return yyvalue == yytable_ninf_;
    }

    int Parser::operator()()
    {
        return parse();
    }

    int Parser::parse()
    {
        int yyn;
        // Length of the RHS of the rule being reduced.
        int yylen = 0;

        // Error handling.
        int yynerrs_ = 0;
        int yyerrstatus_ = 0;

        // The lookahead symbol.
        symbol_type yyla;

        // The locations where the error started and ended.
        stack_symbol_type yyerror_range[3];

        // The return value of parse ().
        int yyresult;

#if YY_EXCEPTIONS
        try
#endif // YY_EXCEPTIONS
        {
            YYCDEBUG << "Starting parse\n";
            /* Initialize the stack.  The initial state will be set in
            * yynewstate, since the latter expects the semantical and the
            * location values to have been already stored, initialize these
            * stacks with a primary value.
            */
            yystack_.clear ();
            yypush_(YY_NULLPTR, 0, YY_MOVE (yyla));

            /*-----------------------------------------------.
            | yynewstate -- push a new symbol on the stack.  |
            `-----------------------------------------------*/
            yynewstate:
            YYCDEBUG << " Entering state" << int (yystack_[0].state) << '\n';
            YY_STACK_PRINT();

            // Accept?
            if (yystack_[0].state == yyfinal_) {
                YYACCEPT;
            }

            goto yybackup;


            /*-----------.
            | yybackup.  |
            `-----------*/
yybackup:
            // Try to take a decision without lookahead.
            yyn = yypact_[+yystack_[0].state];
            if (yy_pact_value_is_default_(yyn)) {
                goto yydefault;
            }

            // Read a lookahead token.
            if (yyla.empty()) {
                YYCDEBUG << "Reading a token\n";
#if YY_EXCEPTIONS
                try
#endif // YY_EXCEPTIONS
                {
                    symbol_type yylookahead(yylex (scanner, interpreter));
                    yyla.move(yylookahead);
                }
#if YY_EXCEPTIONS
                catch (const syntax_error& yyexc) {
                    YYCDEBUG << "Caught exception: " << yyexc.what() << '\n';
                    error(yyexc);
                    goto yyerrlab1;
                }
#endif // YY_EXCEPTIONS
            }
            YY_SYMBOL_PRINT("Next token is", yyla);
            if (yyla.kind() == symbol_kind::S_YYerror) {
                // The scanner already issued an error message, process directly
                // to error recovery.  But do not keep the error token as
                // lookahead, it is too special and may lead us to an endless
                // loop in error recovery. */
                yyla.kind_ = symbol_kind::S_YYUNDEF;
                goto yyerrlab1;
            }

            /* If the proper action on seeing token YYLA.TYPE is to reduce or
              to detect an error, take that action.  */
            yyn += yyla.kind();
            if (yyn < 0 || yylast_ < yyn || yycheck_[yyn] != yyla.kind())
            {
                goto yydefault;
            }

            // Reduce or error.
            yyn = yytable_[yyn];
            if (yyn <= 0) {
                if (yy_table_value_is_error_(yyn)) {
                    goto yyerrlab;
                }
                yyn = -yyn;
                goto yyreduce;
            }

            // Count tokens shifted since error; after three, turn off error status.
            if (yyerrstatus_) {
                --yyerrstatus_;
            }

            // Shift the lookahead token.
            yypush_ ("Shifting", state_type(yyn), YY_MOVE(yyla));
            goto yynewstate;

            /*-----------------------------------------------------------.
            | yydefault -- do the default action for the current state.  |
            `-----------------------------------------------------------*/
yydefault:
            yyn = yydefact_[+yystack_[0].state];
            if (yyn == 0) {
                goto yyerrlab;
            }
            goto yyreduce;


            /*-----------------------------.
            | yyreduce -- do a reduction.  |
            `-----------------------------*/
yyreduce:
            yylen = yyr2_[yyn];
            {
                stack_symbol_type yylhs;
                yylhs.state = yy_lr_goto_state_ (yystack_[yylen].state, yyr1_[yyn]);
                /* Variants are always initialized to an empty instance of the
                  correct type. The default '$$ = $1' action is NOT applied
                  when using variants.  */
                switch (yyr1_[yyn]) {
                    case symbol_kind::S_function_definition: // function_definition
                        yylhs.value.emplace< ScriptFunction* >();
                        break;

                    case symbol_kind::S_arglist: // arglist
                        yylhs.value.emplace< ScriptParams* >();
                        break;

                    case symbol_kind::S_definition_or_statement: // definition_or_statement
                        /* fallthrough */
                    case symbol_kind::S_expression: // expression
                        /* fallthrough */
                    case symbol_kind::S_value_expression: // value_expression
                        /* fallthrough */
                    case symbol_kind::S_compare_expression: // compare_expression
                        /* fallthrough */
                    case symbol_kind::S_add_sub_expression: // add_sub_expression
                        /* fallthrough */
                    case symbol_kind::S_mul_div_expression: // mul_div_expression
                        /* fallthrough */
                    case symbol_kind::S_primary_expression: // primary_expression
                        /* fallthrough */
                    case symbol_kind::S_arg: // arg
                        /* fallthrough */
                    case symbol_kind::S_expression_option: // expression_option
                        yylhs.value.emplace< UScriptExpression* >();
                        break;

                    case symbol_kind::S_statement: // statement
                        /* fallthrough */
                    case symbol_kind::S_expression_statement: // expression_statement
                        /* fallthrough */
                    case symbol_kind::S_for_statement: // for_statement
                        /* fallthrough */
                    case symbol_kind::S_while_statement: // while_statement
                        /* fallthrough */
                    case symbol_kind::S_if_statement: // if_statement
                        /* fallthrough */
                    case symbol_kind::S_break_statement: // break_statement
                        /* fallthrough */
                    case symbol_kind::S_continue_statement: // continue_statement
                        /* fallthrough */
                    case symbol_kind::S_return_statement: // return_statement
                        yylhs.value.emplace< UScriptStatement* >();
                        break;

                    case symbol_kind::S_statement_list: // statement_list
                        /* fallthrough */
                    case symbol_kind::S_block: // block
                        yylhs.value.emplace< UScriptStatementList* >();
                        break;

                    case symbol_kind::S_FLOAT: // FLOAT
                        yylhs.value.emplace< float >();
                        break;
                    case symbol_kind::S_NUMBER: // NUMBER
                        yylhs.value.emplace< int >();
                        break;
                    case symbol_kind::S_VAR: // VAR
                        /* fallthrough */
                    case symbol_kind::S_FUNCTION: // FUNCTION
                        /* fallthrough */
                    case symbol_kind::S_GLOBAL: // GLOBAL
                        /* fallthrough */
                    case symbol_kind::S_FOR: // FOR
                        /* fallthrough */
                    case symbol_kind::S_WHILE: // WHILE
                        /* fallthrough */
                    case symbol_kind::S_IF: // IF
                        /* fallthrough */
                    case symbol_kind::S_ELSE: // ELSE
                        /* fallthrough */
                    case symbol_kind::S_ADD: // ADD
                        /* fallthrough */
                    case symbol_kind::S_SUB: // SUB
                        /* fallthrough */
                    case symbol_kind::S_MUL: // MUL
                        /* fallthrough */
                    case symbol_kind::S_DIV: // DIV
                        /* fallthrough */
                    case symbol_kind::S_ASSIGN: // ASSIGN
                        /* fallthrough */
                    case symbol_kind::S_AND: // AND
                        /* fallthrough */
                    case symbol_kind::S_OR: // OR
                        /* fallthrough */
                    case symbol_kind::S_EQ: // EQ
                        /* fallthrough */
                    case symbol_kind::S_NE: // NE
                        /* fallthrough */
                    case symbol_kind::S_GT: // GT
                        /* fallthrough */
                    case symbol_kind::S_GE: // GE
                        /* fallthrough */
                    case symbol_kind::S_LT: // LT
                        /* fallthrough */
                    case symbol_kind::S_LE: // LE
                        /* fallthrough */
                    case symbol_kind::S_LP: // LP
                        /* fallthrough */
                    case symbol_kind::S_RP: // RP
                        /* fallthrough */
                    case symbol_kind::S_LC: // LC
                        /* fallthrough */
                    case symbol_kind::S_RC: // RC
                        /* fallthrough */
                    case symbol_kind::S_SEMICOLON: // SEMICOLON
                        /* fallthrough */
                    case symbol_kind::S_IDENTIFIER: // IDENTIFIER
                        /* fallthrough */
                    case symbol_kind::S_BREAK: // BREAK
                        /* fallthrough */
                    case symbol_kind::S_CONTINUE: // CONTINUE
                        /* fallthrough */
                    case symbol_kind::S_RETURN: // RETURN
                        /* fallthrough */
                    case symbol_kind::S_COMMA: // COMMA
                        /* fallthrough */
                    case symbol_kind::S_STRING: // STRING
                        yylhs.value.emplace< string >();
                        break;

                    default:
                        break;
                }

                // Default location.
                {
                    stack_type::slice range (yystack_, yylen);
                    YYLLOC_DEFAULT (yylhs.location, range, yylen);
                    yyerror_range[1].location = yylhs.location;
                }
                // Perform the reduction.
                YY_REDUCE_PRINT (yyn);
#if YY_EXCEPTIONS
                try
#endif // YY_EXCEPTIONS
                {
                    switch (yyn) {
                        case 4: { // definition_or_statement: function_definition
                            interpreter->AddFunction(yystack_[0].value.as <ScriptFunction*>());
                            break;
                        }
                        case 5: { // definition_or_statement: statement
                            interpreter->AddStatement(yystack_[0].value.as < UScriptStatement* >());
                            break;
                        }
                        case 6: { // function_definition: FUNCTION IDENTIFIER LP arglist RP block
                            yylhs.value.as<ScriptFunction*>() = ScriptFunction::CreateInstance(
                                yystack_[4].value.as<string>(),
                                yystack_[2].value.as<ScriptParams*>(),
                                yystack_[0].value.as < UScriptStatementList* >());
                            break;
                        }
                        case 7: { // function_definition: FUNCTION IDENTIFIER LP RP block
                            yylhs.value.as<ScriptFunction*>() = ScriptFunction::CreateInstance(
                                yystack_[3].value.as<string>(),
                                nullptr, yystack_[0].value.as<UScriptStatementList*>());
                            break;
                        }
                        case 8: { // statement: expression_statement
                            yylhs.value.as<UScriptStatement*>() = yystack_[0].value.as<UScriptStatement*>();
                            break;
                        }

                        case 9: { // statement: for_statement
                            yylhs.value.as<UScriptStatement*>() = yystack_[0].value.as<UScriptStatement*>();
                            break;
                        }
                        case 10: { // statement: while_statement
                            yylhs.value.as<UScriptStatement*>() = yystack_[0].value.as<UScriptStatement*>();
                            break;
                        }
                        case 11: { // statement: if_statement
                            yylhs.value.as<UScriptStatement*>() = yystack_[0].value.as<UScriptStatement*>();
                            break;
                        }
                        case 12: { // statement: break_statement
                            yylhs.value.as<UScriptStatement*>() = yystack_[0].value.as<UScriptStatement*>();
                            break;
                        }
                        case 13: { // statement: continue_statement
                            yylhs.value.as<UScriptStatement*>() = yystack_[0].value.as <UScriptStatement*>();
                            break;
                        }
                        case 14: { // statement: return_statement
                            yylhs.value.as<UScriptStatement*>() = yystack_[0].value.as <UScriptStatement*>();
                            break;
                        }
                        case 15: { // expression_statement: expression SEMICOLON
                            yylhs.value.as<UScriptStatement*>() =
                                UScriptStatement::CreateExpressionStatement(
                                yystack_[1].value.as<UScriptExpression*>());
                            break;
                        }
                        case 16: { // expression: value_expression
                            yylhs.value.as<UScriptExpression*>() = yystack_[0].value.as<UScriptExpression*>();
                            break;
                        }
                        case 17: { // expression: IDENTIFIER ASSIGN expression
                            yylhs.value.as<UScriptExpression*>() = AssignExpression::CreateExpression(
                                yystack_[2].value.as<string>(), yystack_[0].value.as<UScriptExpression*>());
                            break;
                        }
                        case 18: { // expression: IDENTIFIER COMMA IDENTIFIER ASSIGN expression
                            yylhs.value.as<UScriptExpression*>() = AssignExpression::CreateExpression(
                                yystack_[4].value.as<string>(), yystack_[0].value.as <UScriptExpression*>());
                            AssignExpression::AddIdentifier(yylhs.value.as<UScriptExpression*>(),
                                yystack_[2].value.as<string>());
                            break;
                        }
                        case 19: { // expression: IDENTIFIER COMMA IDENTIFIER COMMA IDENTIFIER ASSIGN expression
                            yylhs.value.as<UScriptExpression*>() = AssignExpression::CreateExpression(
                                yystack_[6].value.as<string>(), yystack_[0].value.as<UScriptExpression*>());
                            AssignExpression::AddIdentifier(yylhs.value.as<UScriptExpression*>(),
                                yystack_[4].value.as<string>());
                            AssignExpression::AddIdentifier(yylhs.value.as<UScriptExpression*>(),
                                yystack_[2].value.as<string>());
                            break;
                        }
                        case 20: {
                             // expression: IDENTIFIER COMMA IDENTIFIER COMMA IDENTIFIER
                             // COMMA IDENTIFIER ASSIGN expression
                            yylhs.value.as<UScriptExpression*>() = AssignExpression::CreateExpression(
                                yystack_[8].value.as<string>(), yystack_[0].value.as<UScriptExpression*>());
                            AssignExpression::AddIdentifier(yylhs.value.as<UScriptExpression*>(),
                                yystack_[6].value.as<string>());
                            AssignExpression::AddIdentifier(yylhs.value.as<UScriptExpression*>(),
                                yystack_[4].value.as<string>());
                            AssignExpression::AddIdentifier(yylhs.value.as <UScriptExpression*>(),
                                yystack_[2].value.as<string>());
                            break;
                        }
                        case 21: { // value_expression: compare_expression
                            yylhs.value.as<UScriptExpression*>() = yystack_[0].value.as <UScriptExpression*>();
                            break;
                        }
                        case 22: { // value_expression: value_expression EQ compare_expression
                            yylhs.value.as<UScriptExpression*>() = BinaryExpression::CreateExpression(
                                UScriptExpression::EQ_OPERATOR,
                                yystack_[2].value.as<UScriptExpression*>(),
                                yystack_[0].value.as<UScriptExpression*>());
                            break;
                        }
                        case 23: { // value_expression: value_expression NE compare_expression
                             yylhs.value.as<UScriptExpression*>() = BinaryExpression::CreateExpression(
                                UScriptExpression::NE_OPERATOR,
                                yystack_[2].value.as<UScriptExpression*>(),
                                yystack_[0].value.as<UScriptExpression*>());
                            break;
                        }
                        case 24: { // value_expression: value_expression AND compare_expression
                            yylhs.value.as<UScriptExpression*>() = BinaryExpression::CreateExpression(
                                UScriptExpression::AND_OPERATOR,
                                yystack_[2].value.as<UScriptExpression*>(),
                                yystack_[0].value.as<UScriptExpression*>());
                            break;
                        }
                        case 25: { // value_expression: value_expression OR compare_expression
                            yylhs.value.as<UScriptExpression*>() = BinaryExpression::CreateExpression(
                                UScriptExpression::OR_OPERATOR,
                                yystack_[2].value.as<UScriptExpression*>(),
                                yystack_[0].value.as<UScriptExpression*>());
                            break;
                        }
                        case 26: { // compare_expression: add_sub_expression
                            yylhs.value.as<UScriptExpression*>() = yystack_[0].value.as<UScriptExpression*>();
                            break;
                        }
                        case 27: { // compare_expression: compare_expression GT add_sub_expression
                            yylhs.value.as<UScriptExpression*>() = BinaryExpression::CreateExpression(
                                UScriptExpression::GT_OPERATOR,
                                yystack_[2].value.as<UScriptExpression*>(),
                                yystack_[0].value.as<UScriptExpression*>());
                            break;
                        }
                        case 28: { // compare_expression: compare_expression GE add_sub_expression
                            yylhs.value.as<UScriptExpression*>() = BinaryExpression::CreateExpression(
                                UScriptExpression::GE_OPERATOR,
                                yystack_[2].value.as<UScriptExpression*>(),
                                yystack_[0].value.as<UScriptExpression*>());
                            break;
                        }
                        case 29: { // compare_expression: compare_expression LT add_sub_expression
                            yylhs.value.as<UScriptExpression*>() = BinaryExpression::CreateExpression(
                                UScriptExpression::LT_OPERATOR,
                                yystack_[2].value.as<UScriptExpression*>(),
                                yystack_[0].value.as<UScriptExpression*>());
                            break;
                        }
                        case 30: { // compare_expression: compare_expression LE add_sub_expression
                            yylhs.value.as<UScriptExpression*>() = BinaryExpression::CreateExpression(
                                UScriptExpression::LE_OPERATOR,
                                yystack_[2].value.as<UScriptExpression*>(),
                                yystack_[0].value.as<UScriptExpression*>());
                            break;
                        }
                        case 31: { // add_sub_expression: mul_div_expression
                            yylhs.value.as<UScriptExpression*>() = yystack_[0].value.as<UScriptExpression*>();
                            break;
                        }
                        case 32: { // add_sub_expression: add_sub_expression ADD mul_div_expression
                            yylhs.value.as<UScriptExpression*>() = BinaryExpression::CreateExpression(
                                UScriptExpression::ADD_OPERATOR,
                                yystack_[2].value.as<UScriptExpression*>(),
                                yystack_[0].value.as<UScriptExpression*>());
                            break;
                        }
                        case 33: { // add_sub_expression: add_sub_expression SUB mul_div_expression
                            yylhs.value.as<UScriptExpression*>() = BinaryExpression::CreateExpression(
                                UScriptExpression::SUB_OPERATOR,
                                yystack_[2].value.as<UScriptExpression*>(),
                                yystack_[0].value.as<UScriptExpression*>());
                            break;
                        }
                        case 34: { // mul_div_expression: primary_expression
                            yylhs.value.as< UScriptExpression*>() = yystack_[0].value.as<UScriptExpression*>();
                            break;
                        }
                        case 35: { // mul_div_expression: mul_div_expression DIV primary_expression
                            yylhs.value.as<UScriptExpression*>() = BinaryExpression::CreateExpression(
                                UScriptExpression::DIV_OPERATOR,
                                yystack_[2].value.as<UScriptExpression*>(),
                                yystack_[0].value.as<UScriptExpression*>());
                            break;
                        }
                        case 36: { // mul_div_expression: mul_div_expression MUL primary_expression
                            yylhs.value.as<UScriptExpression*>() = BinaryExpression::CreateExpression(
                                UScriptExpression::MUL_OPERATOR,
                                yystack_[2].value.as<UScriptExpression*>(),
                                yystack_[0].value.as<UScriptExpression*>());
                            break;
                        }
                        case 37: { // primary_expression: SUB primary_expression
                            yylhs.value.as<UScriptExpression*>() =yystack_[0].value.as<UScriptExpression*>();
                            break;
                        }
                        case 38: { // primary_expression: LP expression RP
                            yylhs.value.as<UScriptExpression*>() =yystack_[1].value.as<UScriptExpression*>();
                            break;
                        }
                        case 39: { // primary_expression: IDENTIFIER
                            yylhs.value.as<UScriptExpression*>() = IdentifierExpression::CreateExpression(
                                yystack_[0].value.as < string >());
                            break;
                        }
                        case 40: { // primary_expression: STRING
                            yylhs.value.as< UScriptExpression* >() = StringExpression::CreateExpression(
                                yystack_[0].value.as<string>());
                            break;
                        }
                        case 41: { // primary_expression: NUMBER
                            yylhs.value.as<UScriptExpression*>() = IntegerExpression::CreateExpression(
                                yystack_[0].value.as<int>());
                            break;
                        }
                        case 42: { // primary_expression: FLOAT
                            yylhs.value.as<UScriptExpression*>() = FloatExpression::CreateExpression(
                                yystack_[0].value.as<float>());
                            break;
                        }
                        case 43: { // primary_expression: IDENTIFIER LP RP
                            yylhs.value.as<UScriptExpression*>() = FunctionCallExpression::CreateExpression(
                                yystack_[2].value.as<string>(), nullptr);
                            break;
                        }
                        case 44: { // primary_expression: IDENTIFIER LP arglist RP
                            yylhs.value.as<UScriptExpression*>() = FunctionCallExpression::CreateExpression(
                                yystack_[3].value.as<string>(), yystack_[1].value.as<ScriptParams*>());
                            break;
                        }
                        case 45: { // statement_list: statement_list statement
                            yystack_[1].value.as<UScriptStatementList*>()->AddScriptStatement(
                                yystack_[0].value.as < UScriptStatement* >());
                            yylhs.value.as <UScriptStatementList*>() = yystack_[1].value.as<UScriptStatementList*>();
                            break;
                        }
                        case 46: { // statement_list: statement
                            yylhs.value.as<UScriptStatementList*>() = UScriptStatementList::CreateInstance(
                                yystack_[0].value.as<UScriptStatement*>());
                            break;
                        }
                        case 47: { // block: LC RC
                            yylhs.value.as<UScriptStatementList*>()= nullptr;
                            break;
                        }
                        case 48: { // block: LC statement_list RC
                            yylhs.value.as<UScriptStatementList*>() = yystack_[1].value.as<UScriptStatementList*>();
                            break;
                        }
                        case 49: { // arglist: arglist COMMA arg
                            yylhs.value.as<ScriptParams*>() = ScriptParams::AddParams(
                                yystack_[2].value.as<ScriptParams*>(), yystack_[0].value.as<UScriptExpression*>());
                            break;
                        }
                        case 50: { // arglist: arg
                            yylhs.value.as<ScriptParams*>() = ScriptParams::CreateParams(
                                yystack_[0].value.as<UScriptExpression*>());
                            break;
                        }
                        case 51: { // arg: value_expression
                            yylhs.value.as<UScriptExpression*>() = yystack_[0].value.as<UScriptExpression*>();
                            break;
                        }
                        case 52: { // expression_option: %empty
                            yylhs.value.as<UScriptExpression*>() = nullptr;
                            break;
                        }
                        case 53: { // expression_option: expression
                            yylhs.value.as<UScriptExpression*>() = yystack_[0].value.as<UScriptExpression*>();
                            break;
                        }
                        case 54: {
                             // for_statement: FOR LP expression_option SEMICOLON
                             // expression_option SEMICOLON expression_option RP block
                            yylhs.value.as<UScriptStatement*>() = UScriptStatement::CreateForStatement(
                                yystack_[6].value.as<UScriptExpression*>(),
                                yystack_[4].value.as<UScriptExpression*>(),
                                yystack_[2].value.as<UScriptExpression*>(),
                                yystack_[0].value.as<UScriptStatementList*>());
                            break;
                        }
                        case 55: { // while_statement: WHILE LP expression_option RP block
                            yylhs.value.as<UScriptStatement*>() = UScriptStatement::CreateWhileStatement(
                                yystack_[2].value.as<UScriptExpression*>(),
                                (UScriptStatementList*)yystack_[0].value.as<UScriptStatementList*>());
                            break;
                        }
                        case 56: { // if_statement: IF LP expression RP block
                            yylhs.value.as<UScriptStatement*>() = UScriptStatement::CreateIfStatement(
                                yystack_[2].value.as<UScriptExpression*>(),
                                yystack_[0].value.as<UScriptStatementList*>());
                            break;
                        }
                        case 57: { // if_statement: IF LP expression RP block ELSE if_statement
                            yylhs.value.as<UScriptStatement*>() = UScriptStatement::CreateIfStatement(
                                yystack_[4].value.as<UScriptExpression*>(),
                                yystack_[2].value.as<UScriptStatementList*>(),
                                nullptr, yystack_[0].value.as<UScriptStatement*>());
                            break;
                        }
                        case 58: { // if_statement: IF LP expression RP block ELSE block
                            yylhs.value.as<UScriptStatement*>() = UScriptStatement::CreateIfStatement(
                                yystack_[4].value.as<UScriptExpression*>(),
                                yystack_[2].value.as<UScriptStatementList*>(),
                                yystack_[0].value.as<UScriptStatementList*>());
                            break;
                        }
                        case 59: { // break_statement: BREAK SEMICOLON
                            yylhs.value.as<UScriptStatement*>() = UScriptStatement::CreateStatement(
                                UScriptStatement::STATEMENT_TYPE_BREAK);
                            break;
                        }
                        case 60: { // continue_statement: CONTINUE SEMICOLON
                            // $$=create_Statement(STATEMENT_TYPE_CONTINUE);
                            yylhs.value.as<UScriptStatement*>() = UScriptStatement::CreateStatement(
                                UScriptStatement::STATEMENT_TYPE_CONTINUE);
                            break;
                        }
                        case 61: { // return_statement: RETURN arglist SEMICOLON
                            yylhs.value.as<UScriptStatement*>() = UScriptReturnStatement::CreateStatement(
                                yystack_[1].value.as<ScriptParams*>());
                            break;
                        }
                        case 62: { // return_statement: RETURN SEMICOLON
                            yylhs.value.as< UScriptStatement*>() = UScriptReturnStatement::CreateStatement(nullptr);
                            break;
                        }
                        default:
                            break;
                    }
                }
#if YY_EXCEPTIONS
                catch (const syntax_error& yyexc) {
                    YYCDEBUG << "Caught exception: " << yyexc.what()<< '\n';
                    error (yyexc);
                    YYERROR;
                }
#endif // YY_EXCEPTIONS
                YY_SYMBOL_PRINT("-> $$ =", yylhs);
                yypop_(yylen);
                yylen = 0;
                // Shift the result of the reduction.
                yypush_(YY_NULLPTR, YY_MOVE (yylhs));
            }
            goto yynewstate;

            /*--------------------------------------.
            | yyerrlab -- here on detecting error.  |
            `--------------------------------------*/
yyerrlab:
            // If not already recovering from an error, report this error.
            if (!yyerrstatus_) {
                ++yynerrs_;
                context yyctx (*this, yyla);
                std::string msg = yysyntax_error_ (yyctx);
                error (yyla.location, YY_MOVE (msg));
            }

            yyerror_range[1].location = yyla.location;
            if (yyerrstatus_ == 3) {
                /* If just tried and failed to reuse lookahead token after an error, discard it.  */
                // Return failure if at end of input.
                if (yyla.kind ()== symbol_kind::S_YYEOF) {
                    YYABORT;
                } else if (!yyla.empty ()) {
                    yy_destroy_ ("Error: discarding", yyla);
                    yyla.clear ();
               }
            }
            // Else will try to reuse lookahead token after shifting the error token.
            goto yyerrlab1;
            /*---------------------------------------------------.
            | yyerrorlab -- error raised explicitly by YYERROR.  |
            `---------------------------------------------------*/
yyerrorlab:
            /* Pacify compilers when the user code never invokes YYERROR and
              the label yyerrorlab therefore never appears in user code.  */
            if (false) {
                YYERROR;
            }

            /* Do not reclaim the symbols of the rule whose action triggered this YYERROR.  */
            yypop_(yylen);
            yylen = 0;
            YY_STACK_PRINT();
            goto yyerrlab1;
            /*-------------------------------------------------------------.
            | yyerrlab1 -- common code for both syntax error and YYERROR.  |
            `-------------------------------------------------------------*/
yyerrlab1:
            yyerrstatus_ = 3;   // Each real token shifted decrements this.
            // Pop stack until we find a state that shifts the error token.
            for (;;) {
                yyn = yypact_[+yystack_[0].state];
                if (!yy_pact_value_is_default_(yyn)) {
                    yyn += symbol_kind::S_YYerror;
                    if (0 <= yyn && yyn <= yylast_ && yycheck_[yyn] == symbol_kind::S_YYerror) {
                        yyn = yytable_[yyn];
                        if (0 < yyn) {
                            break;
                        }
                    }
                }

                // Pop the current state because it cannot handle the error token.
                if (yystack_.size()== 1) {
                    YYABORT;
                }
                yyerror_range[1].location = yystack_[0].location;
                yy_destroy_("Error: popping", yystack_[0]);
                yypop_();
                YY_STACK_PRINT();
            }

            {
                stack_symbol_type error_token;
                yyerror_range[2].location = yyla.location;
                YYLLOC_DEFAULT (error_token.location, yyerror_range, 2);

                // Shift the error token.
                error_token.state = state_type (yyn);
                yypush_ ("Shifting", YY_MOVE (error_token));
            }
            goto yynewstate;
            /*-------------------------------------.
            | yyacceptlab -- YYACCEPT comes here.  |
            `-------------------------------------*/
yyacceptlab:
            yyresult = 0;
            goto yyreturn;
            /*-----------------------------------.
            | yyabortlab -- YYABORT comes here.  |
            `-----------------------------------*/
yyabortlab:
            yyresult = 1;
            goto yyreturn;

            /*-----------------------------------------------------.
            | yyreturn -- parsing is finished, return the result.  |
            `-----------------------------------------------------*/
yyreturn:
            if (!yyla.empty()) {
                yy_destroy_("Cleanup: discarding lookahead", yyla);
            }
            /* Do not reclaim the symbols of the rule whose action triggered this YYABORT or YYACCEPT.  */
            yypop_ (yylen);
            YY_STACK_PRINT ();
            while (1 < yystack_.size()) {
                yy_destroy_("Cleanup: popping", yystack_[0]);
                yypop_();
            }
            return yyresult;
        }
#if YY_EXCEPTIONS
        catch (...) {
            YYCDEBUG << "Exception caught: cleaning lookahead and stack\n";
            // Do not try to display the values of the reclaimed symbols,
            // as their printers might throw an exception.
            if (!yyla.empty ()) {
                    yy_destroy_ (YY_NULLPTR, yyla);
            }
            while (1 < yystack_.size ()) {
                yy_destroy_ (YY_NULLPTR, yystack_[0]);
                yypop_ ();
            }
            throw;
        }
#endif // YY_EXCEPTIONS
    }

    void Parser::error(const syntax_error& yyexc)
    {
        error(yyexc.location, yyexc.what());
    }

    /* Return YYSTR after stripping away unnecessary quotes and
       backslashes, so that it's suitable for yyerror.  The heuristic is
       that double-quoting is unnecessary unless the string contains an
       apostrophe, a comma, or backslash (other than backslash-backslash).
       YYSTR is taken from yytname.  */
    std::string Parser::yytnamerr_(const char *yystr)
    {
        if (*yystr == '"') {
            std::string yyr;
            char const *yyp = yystr;
            for (;;)
                switch (*++yyp) {
                    case '\'':
                    /* fallthrough */
                    case ',':
                        goto do_not_strip_quotes;

                    case '\\':
                        if (*++yyp != '\\') {
                            goto do_not_strip_quotes;
                        } else {
                            goto append;
                        }
append:
                    default:
                        yyr += *yyp;
                        break;

                    case '"':
                        return yyr;
                }
            do_not_strip_quotes: ;
        }
        return yystr;
    }

    std::string Parser::symbol_name(symbol_kind_type yysymbol)
    {
        return yytnamerr_(yytname_[yysymbol]);
    }

    //  Parser::context.
    Parser::context::context(const  Parser &yyparser, const symbol_type &yyla)
        : yyparser_(yyparser), yyla_(yyla) {}

    int Parser::context::expected_tokens(symbol_kind_type yyarg[], int yyargn) const
    {
        // Actual number of expected tokens
        int yycount = 0;

        int yyn = yypact_[+yyparser_.yystack_[0].state];
        if (!yy_pact_value_is_default_(yyn)) {
        /* Start YYX at -YYN if negative to avoid negative indexes in
           YYCHECK.  In other words, skip the first -YYN actions for
           this state because they are default actions.  */
            int yyxbegin = yyn < 0 ? -yyn : 0;
            // Stay within bounds of both yycheck and yytname.
            int yychecklim = yylast_ - yyn + 1;
            int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
            for (int yyx = yyxbegin; yyx < yyxend; ++yyx) {
                if (yycheck_[yyx + yyn] == yyx && yyx != symbol_kind::S_YYerror &&
                    !yy_table_value_is_error_ (yytable_[yyx + yyn])) {
                    if (!yyarg) {
                        ++yycount;
                    } else if (yycount == yyargn) {
                        return 0;
                    } else {
                        yyarg[yycount++] = YY_CAST (symbol_kind_type, yyx);
                    }
                }
            }
        }

        if (yyarg && yycount == 0 && 0 < yyargn) {
            yyarg[0] = symbol_kind::S_YYEMPTY;
        }
        return yycount;
    }

    int Parser::yy_syntax_error_arguments_(const context& yyctx, symbol_kind_type yyarg[], int yyargn) const
    {
        /* There are many possibilities here to consider:
              - If this state is a consistent state with a default action, then
                  the only way this function was invoked is if the default action
                  is an error action.  In that case, don't check for expected
                  tokens because there are none.
              - The only way there can be no lookahead present (in yyla)is
                  if this state is a consistent state with a default action.
                  Thus, detecting the absence of a lookahead is sufficient to
                  determine that there is no unexpected or expected token to
                  report.  In that case, just report a simple "syntax error".
              - Don't assume there isn't a lookahead just because this state is
                  a consistent state with a default action.  There might have
                  been a previous inconsistent state, consistent state with a
                  non-default action, or user semantic action that manipulated
                  yyla.  (However, yyla is currently not documented for users.)
              - Of course, the expected token list depends on states to have
                  correct lookahead information, and it depends on the parser not
                  to perform extra reductions after fetching a lookahead from the
                  scanner and before detecting a syntax error.  Thus, state merging
                  (from LALR or IELR)and default reductions corrupt the expected
                  token list.  However, the list is correct for canonical LR with
                  one exception: it will still contain any token that will not be
                  accepted due to an error action in a later state.
        */

        if (!yyctx.lookahead ().empty()) {
            if (yyarg) {
                yyarg[0] = yyctx.token();
            }
            int yyn = yyctx.expected_tokens(yyarg ? yyarg + 1 : yyarg, yyargn - 1);
            return yyn + 1;
        }
        return 0;
    }

    // Generate an error message.
    std::string Parser::yysyntax_error_(const context& yyctx) const
    {
        // Its maximum.
        enum { YYARGS_MAX = 5 };
        // Arguments of yyformat.
        symbol_kind_type yyarg[YYARGS_MAX];
        int yycount = yy_syntax_error_arguments_(yyctx, yyarg, YYARGS_MAX);

        char const* yyformat = YY_NULLPTR;
        switch (yycount) {
#define YYCASE_(N, S)                        \
            case N:                               \
                yyformat = S;                       \
                break
            default: // Avoid compiler warnings.
                YYCASE_ (0, YY_("syntax error"));
                YYCASE_ (1, YY_("syntax error, unexpected %s"));
                YYCASE_ (2, YY_("syntax error, unexpected %s, expecting %s"));
                YYCASE_ (3, YY_("syntax error, unexpected %s, expecting %s or %s"));
                YYCASE_ (4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
                YYCASE_ (5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
#undef YYCASE_
        }

        std::string yyres;
        // Argument number.
        std::ptrdiff_t yyi = 0;
        for (char const* yyp = yyformat; *yyp; ++yyp)
            if (yyp[0] == '%' && yyp[1] == 's' && yyi < yycount) {
                yyres += symbol_name (yyarg[yyi++]);
                ++yyp;
            } else {
                yyres += *yyp;
            }
        return yyres;
    }

    const signed char  Parser::yypact_ninf_ = -89;
    const signed char  Parser::yytable_ninf_ = -1;
    const short Parser::yypact_[] =
    {
        40, -89, -89, -11, -4, 16, 27, 99, 151, 29,
        15, 32, 74, -89, 5, -89, -89, -89, -89, 35,
        120, 148, 19, 54, -89, -89, -89, -89, -89, -89,
        -89, 33, 151, 151, 151, 41, -89, 56,  151, 130,
        59, -89, -89, -89, 120, -17, -89, -89, -89, -89,
        99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
        99, 99, 133, -89, 55, 64, 68, -89, -89, -89,
        0, -14, -89, 99, 148, 148, 148, 148, 19, 19,
        19, 19, 54, 54, -89, -89, 70, 13, 151, 70,
        70, -89, 151, 65, -89, 83, -89, 70, 76, -89,
        95, -89, -7, -89, -89, 117, -89, 151, -5, 151,
        77, -89, -89, 91, -89, -89, -89, 105, 70, 151,
        -89, -89
    };

    const signed char Parser::yydefact_[] =
    {
        0, 41, 42, 0, 0, 0, 0, 0, 0, 39,
        0, 0, 0, 40, 0, 2, 4, 5, 8, 0,
        16, 21, 26, 31, 34, 9, 10, 11, 12, 13,
        14, 0, 52, 52, 0, 39, 37, 0, 0, 0,
        0, 59, 60, 62, 51, 0, 50, 1, 3, 15,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 53, 0, 0, 0, 38, 17, 43,
        0, 0, 61, 0, 24, 25, 22, 23, 27, 28,
        29, 30, 32, 33, 36, 35, 0, 0, 52, 0,
        0, 44, 0, 0, 49, 0, 7, 0,  0, 55,
        56, 18, 0, 47, 46, 0, 6, 52, 0, 0,
        0, 48, 45, 0, 58, 57, 19, 0, 0, 0,
        54, 20
    };

    const short Parser::yypgoto_[] =
    {
        -89, -89, 109, -89, -88, -89, -8, -10, 128, 129,
        23, -3, -89, -29, -28, 51, -32, -89, -89, 21,
        -89, -89, -89
    };

    const signed char Parser::yydefgoto_[] =
    {
        -1, 14, 15, 16, 17, 18, 19, 20, 21, 22,
        23, 24, 105, 96, 45, 46, 64, 25, 26, 27,
        28, 29, 30
    };

    const signed char Parser::yytable_[] =
    {
        37, 65, 44, 92, 36, 47, 6, 104, 1, 2,
        109, 70, 3, 72, 4, 5, 6, 112, 73, 7,
        31, 93, 32, 95, 63, 63, 66, 91, 110, 44,
        68, 8, 58, 59, 87, 73, 9, 10, 11, 12,
        97, 13, 33, 1, 2, 41, 38, 3, 73, 4,
        5, 6, 44, 34, 7, 39, 98, 84, 85, 62,
        99, 100, 42, 44, 40, 49, 8, 39, 106, 60,
        61, 9, 10, 11, 12, 113, 13, 1, 2, 114,
        63, 82, 83, 67, 101, 88, 1, 2, 7, 120,
        71, 89, 4, 5, 6, 90, 102, 7, 95, 63,
        8, 116, 1, 2, 43, 35, 107, 108, 117, 8,
        13, 121, 103, 7, 9, 10, 11, 12, 118, 13,
        1, 2, 119, 48, 94, 8, 4, 5, 6, 115,
        35, 7, 0, 1, 2, 13, 1, 2, 50, 51,
        52, 53, 0, 8, 7, 0, 111, 7, 9, 10,
        11, 12, 0, 13, 1, 2, 8, 69, 0, 8,
        86, 35, 0, 0, 35, 7, 13, 0, 0, 13,
        54, 55, 56, 57, 0, 0, 0, 8, 74, 75,
        76, 77, 9, 78, 79, 80, 81, 13
    };

    const signed char Parser::yycheck_[] =
    {
        8, 33, 12, 17, 7, 0, 11, 95, 3, 4,
        17, 39, 7, 30, 9, 10, 11, 105, 35, 14,
        31, 35, 26, 28, 32, 33, 34, 27, 35, 39,
        38, 26, 13, 14, 62, 35, 31, 32, 33, 34,
        27, 36, 26, 3, 4, 30, 17, 7, 35, 9,
        10, 11, 62, 26, 14, 26, 88, 60, 61, 26,
        89, 90, 30, 73, 35, 30, 26, 26, 97, 15,
        16, 31, 32, 33, 34, 107, 36, 3, 4, 108,
        88, 58, 59, 27, 92, 30, 3, 4, 14, 118,
        31, 27, 9, 10, 11, 27, 31, 14, 28, 107,
        26, 109, 3, 4, 30, 31, 30, 12, 31, 26,
        36, 119, 29, 14, 31, 32, 33, 34, 27, 36,
        3,  4, 17, 14, 73, 26, 9, 10, 11, 108,
        31, 14, -1, 3, 4, 36, 3, 4, 18, 19,
        20, 21, -1, 26, 14, -1, 29, 14, 31, 32,
        33, 34, -1, 36, 3, 4, 26, 27, -1, 26,
        27, 31, -1, -1, 31, 14, 36, -1, -1, 36,
        22, 23, 24, 25, -1, -1, -1, 26, 50, 51,
        52, 53, 31, 54, 55, 56, 57, 36
    };

    const signed char Parser::yystos_[] =
    {
        0, 3, 4, 7, 9, 10, 11, 14, 26, 31,
        32, 33, 34, 36, 38, 39, 40, 41, 42, 43,
        44, 45, 46, 47, 48, 54, 55, 56, 57, 58,
        59, 31, 26, 26, 26, 31, 48, 43, 17, 26,
        35, 30, 30, 30, 44, 51, 52,  0, 39, 30,
        18, 19, 20, 21, 22, 23, 24, 25, 13, 14,
        15, 16, 26, 43, 53, 53, 43, 27, 43, 27,
        51, 31, 30, 35, 45, 45, 45, 45, 46, 46,
        46, 46, 47, 47, 48, 48, 27, 51, 30, 27,
        27, 27, 17, 35, 52, 28, 50, 27, 53, 50,
        50, 43, 31, 29, 41, 49, 50, 30, 12, 17,
        35, 29, 41, 53, 50, 56, 43, 31, 27, 17,
        50, 43
    };

    const signed char Parser::yyr1_[] =
    {
        0,  37, 38, 38, 39, 39, 40, 40, 41, 41,
        41, 41, 41, 41, 41, 42, 43, 43, 43, 43,
        43, 44, 44, 44, 44, 44, 45, 45, 45, 45,
        45, 46, 46, 46, 47, 47, 47, 48, 48, 48,
        48, 48, 48, 48, 48, 49, 49, 50, 50, 51,
        51, 52, 53, 53, 54, 55, 56, 56, 56, 57,
        58, 59, 59
    };

    const signed char Parser::yyr2_[] =
    {
        0, 2, 1, 2, 1, 1, 6, 5, 1, 1,
        1, 1, 1, 1, 1, 2, 1, 3, 5, 7,
        9, 1, 3, 3, 3, 3, 1, 3, 3, 3,
        3, 1, 3, 3, 1, 3, 3, 2, 3, 1,
        1, 1, 1, 3, 4, 2, 1, 2, 3, 3,
        1, 1, 0, 1, 9, 5, 5, 7, 7, 2,
        2, 3, 2
    };

#if YYDEBUG || 1
    // YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
    // First, the terminals, then, starting at \a YYNTOKENS, nonterminals.
    const char* const  Parser::yytname_[] =
    {
        "END", "error", "\"invalid token\"", "NUMBER", "FLOAT", "EOL", "VAR",
        "FUNCTION", "GLOBAL", "FOR", "WHILE", "IF", "ELSE", "ADD", "SUB", "MUL",
        "DIV", "ASSIGN", "AND", "OR", "EQ", "NE", "GT", "GE", "LT", "LE", "LP",
        "RP", "LC", "RC", "SEMICOLON", "IDENTIFIER", "BREAK", "CONTINUE",
        "RETURN", "COMMA", "STRING", "$accept", "translation_unit",
        "definition_or_statement", "function_definition", "statement",
        "expression_statement", "expression", "value_expression",
        "compare_expression", "add_sub_expression", "mul_div_expression",
        "primary_expression", "statement_list", "block", "arglist", "arg",
        "expression_option", "for_statement", "while_statement", "if_statement",
        "break_statement", "continue_statement", "return_statement", YY_NULLPTR
    };
#endif

#if YYDEBUG
    const short Parser::yyrline_[] =
    {
        0, 117, 117, 118, 120, 124, 129, 134, 139, 140,
        141, 142, 143, 144, 145, 147, 152, 153, 157, 162,
        168, 176, 177, 181, 185, 189, 194, 195, 199, 203,
        207, 212, 213, 217, 222, 223, 227, 232, 236, 240,
        244, 248, 252, 256, 260, 265, 270, 275, 279, 284,
        288, 293, 296, 299, 301, 306, 311, 315, 319, 324,
        329, 335, 339
    };

    void Parser::yy_stack_print_() const
    {
        *yycdebug_ << "Stack now";
        for (stack_type::const_iterator i = yystack_.begin (), i_end = yystack_.end ();
            i != i_end; ++i) {
            *yycdebug_ << ' ' << int (i->state);
        }
        *yycdebug_ << '\n';
    }

    void Parser::yy_reduce_print_(int yyrule) const
    {
        int yylno = yyrline_[yyrule];
        int yynrhs = yyr2_[yyrule];
        // Print the symbols being reduced, and their result.
        *yycdebug_ << "Reducing stack by rule " << yyrule - 1
                              << " (line " << yylno << "):\n";
        // The symbols being reduced.
        for (int yyi = 0; yyi < yynrhs; yyi++) {
            YY_SYMBOL_PRINT ("   $" << yyi + 1 << " =",
                yystack_[(yynrhs)- (yyi + 1)]);
        }
    }
#endif // YYDEBUG

} // uscript

