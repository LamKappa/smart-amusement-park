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

#ifndef YY_YY_YACC_PARSER_HPP_INCLUDED
#define YY_YY_YACC_PARSER_HPP_INCLUDED
// "%code requires" blocks.

#include <cmath>
#include <iostream>
#include <stdint.h>
#include <string>
#include <vector>
#include "location.hh"
#include "script_expression.h"
#include "script_function.h"
#include "script_param.h"
#include "script_statement.h"

using std::string;
namespace uscript {
  class Scanner;
  class ScriptInterpreter;
}

#include <cstdlib> // std::abort
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#if defined __cplusplus
# define YY_CPLUSPLUS __cplusplus
#else
# define YY_CPLUSPLUS 199711L
#endif

// Support move semantics when possible.
#if 201103L <= YY_CPLUSPLUS
# define YY_MOVE           std::move
# define YY_MOVE_OR_COPY   move
# define YY_MOVE_REF(Type) Type&&
# define YY_RVREF(Type)    Type&&
# define YY_COPY(Type)     Type
#else
# define YY_MOVE
# define YY_MOVE_OR_COPY   copy
# define YY_MOVE_REF(Type) Type&
# define YY_RVREF(Type)    const Type&
# define YY_COPY(Type)     const Type&
#endif

// Support noexcept when possible.
#if 201103L <= YY_CPLUSPLUS
# define YY_NOEXCEPT noexcept
# define YY_NOTHROW
#else
# define YY_NOEXCEPT
# define YY_NOTHROW throw ()
#endif

// Support constexpr when possible.
#if 201703 <= YY_CPLUSPLUS
# define YY_CONSTEXPR constexpr
#else
# define YY_CONSTEXPR
#endif
# include "location.hh"

#ifndef YY_ASSERT
# include <cassert>
# define YY_ASSERT assert
#endif

#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && ! defined __ICC && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                            \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

namespace uscript {
/// A Bison parser.
class  Parser {
public:
#ifndef YYSTYPE
/// A buffer to store and retrieve objects.
///
/// Sort of a variant, but does not keep track of the nature
/// of the stored data, since that knowledge is available
/// via the current parser state.
class semantic_type {
public:
    /// Type of *this.
    typedef semantic_type self_type;
    /// Empty construction.
    semantic_type () YY_NOEXCEPT
        : yybuffer_ ()
    {}
    /// Construct and fill.
    template <typename T>
    semantic_type (YY_RVREF (T)t)
    {
      YY_ASSERT (sizeof (T) <= size);
      new (yyas_<T> ()) T (YY_MOVE (t));
    }
#if 201103L <= YY_CPLUSPLUS
    /// Non copyable.
    semantic_type (const self_type&) = delete;
    /// Non copyable.
    self_type& operator= (const self_type&) = delete;
#endif
    /// Destruction, allowed only if empty.
    ~semantic_type () YY_NOEXCEPT {}
# if 201103L <= YY_CPLUSPLUS
    /// Instantiate a \a T in here from \a t.
    template <typename T, typename... U>
    T& emplace (U&&... u)
    {
        return *new (yyas_<T> ()) T (std::forward <U>(u)...);
    }
# else
    /// Instantiate an empty \a T in here.
    template <typename T>
    T& emplace ()
    {
        return *new (yyas_<T> ()) T ();
    }
    /// Instantiate a \a T in here from \a t.
    template <typename T>
    T& emplace (const T& t)
    {
        return *new (yyas_<T> ()) T (t);
    }
# endif
    /// Instantiate an empty \a T in here.
    /// Obsolete, use emplace.
    template <typename T>
    T& build ()
    {
        return emplace<T> ();
    }
    /// Instantiate a \a T in here from \a t.
    /// Obsolete, use emplace.
    template <typename T>
    T& build (const T& t)
    {
        return emplace<T> (t);
    }
    /// Accessor to a built \a T.
    template <typename T>
    T& as () YY_NOEXCEPT
    {
        return *yyas_<T> ();
    }
    /// Const accessor to a built \a T (for %printer).
    template <typename T>
    const T& as () const YY_NOEXCEPT
    {
        return *yyas_<T> ();
    }

    /// Swap the content with \a that, of same type.
    ///
    /// Both variants must be built beforehand, because swapping the actual
    /// data requires reading it (with as()), and this is not possible on
    /// unconstructed variants: it would require some dynamic testing, which
    /// should not be the variant's responsibility.
    /// Swapping between built and (possibly) non-built is done with
    /// self_type::move ().
    template <typename T>
    void swap (self_type& that) YY_NOEXCEPT
    {
        std::swap (as<T> (), that.as<T> ());
    }
    /// Move the content of \a that to this.
    ///
    /// Destroys \a that.
    template <typename T>
    void move (self_type& that)
    {
# if 201103L <= YY_CPLUSPLUS
        emplace<T> (std::move (that.as<T> ()));
# else
        emplace<T> ();
        swap<T> (that);
# endif
        that.destroy<T> ();
    }
# if 201103L <= YY_CPLUSPLUS
    /// Move the content of \a that to this.
    template <typename T>
    void move (self_type&& that)
    {
        emplace<T> (std::move (that.as<T> ()));
        that.destroy<T> ();
    }
#endif
    /// Copy the content of \a that to this.
    template <typename T>
    void copy (const self_type& that)
    {
        emplace<T> (that.as<T> ());
    }
    /// Destroy the stored \a T.
    template <typename T>
    void destroy ()
    {
        as<T> ().~T ();
    }

private:
#if YY_CPLUSPLUS < 201103L
    /// Non copyable.
    semantic_type (const self_type&);
    /// Non copyable.
    self_type& operator= (const self_type&);
#endif
    /// Accessor to raw memory as \a T.
    template <typename T>
    T*
    yyas_ () YY_NOEXCEPT
    {
        void *yyp = yybuffer_.yyraw;
        return static_cast<T*> (yyp);
    }
    /// Const accessor to raw memory as \a T.
    template <typename T>
    const T*
    yyas_ () const YY_NOEXCEPT
    {
        const void *yyp = yybuffer_.yyraw;
        return static_cast<const T*> (yyp);
    }

    /// An auxiliary type to compute the largest semantic type.
    union union_type {
        // function_definition
        char dummy1[sizeof (ScriptFunction*)];
        // arglist
        char dummy2[sizeof (ScriptParams*)];
        // definition_or_statement
        // expression
        // value_expression
        // compare_expression
        // add_sub_expression
        // mul_div_expression
        // primary_expression
        // arg
        // expression_option
        char dummy3[sizeof (UScriptExpression*)];
        // statement
        // expression_statement
        // for_statement
        // while_statement
        // if_statement
        // break_statement
        // continue_statement
        // return_statement
        char dummy4[sizeof (UScriptStatement*)];
        // statement_list
        // block
        char dummy5[sizeof (UScriptStatementList*)];
        // FLOAT
        char dummy6[sizeof (float)];
        // NUMBER
        char dummy7[sizeof (int)];
        // VAR
        // FUNCTION
        // GLOBAL
        // FOR
        // WHILE
        // IF
        // ELSE
        // ADD
        // SUB
        // MUL
        // DIV
        // ASSIGN
        // AND
        // OR
        // EQ
        // NE
        // GT
        // GE
        // LT
        // LE
        // LP
        // RP
        // LC
        // RC
        // SEMICOLON
        // IDENTIFIER
        // BREAK
        // CONTINUE
        // RETURN
        // COMMA
        // STRING
        char dummy8[sizeof(string)];
    };
    /// The size of the largest semantic type.
    enum {size = sizeof(union_type)};
    /// A buffer to store semantic values.
    union {
        /// Strongest alignment constraints.
        long double yyalign_me;
        /// A buffer large enough to store any of the semantic values.
        char yyraw[size];
    }yybuffer_;
};

#else
    typedef YYSTYPE semantic_type;
#endif
    /// Symbol locations.
    typedef location location_type;

    /// Syntax errors thrown from user actions.
    struct syntax_error : std::runtime_error
    {
        syntax_error(const location_type& l, const std::string& m)
            : std::runtime_error(m), location(l) {}
        syntax_error (const syntax_error& s)
            : std::runtime_error(s.what()), location(s.location) {}
        ~syntax_error ()YY_NOEXCEPT YY_NOTHROW;
        location_type location;
    };

    /// Token kinds.
    struct token {
        enum token_kind_type {
            TOKEN_YYEMPTY = -2,
            TOKEN_END = 0,                 // END
            TOKEN_YYerror = 256,           // error
            TOKEN_YYUNDEF = 257,           // "invalid token"
            TOKEN_NUMBER = 258,            // NUMBER
            TOKEN_FLOAT = 259,             // FLOAT
            TOKEN_EOL = 260,               // EOL
            TOKEN_VAR = 261,               // VAR
            TOKEN_FUNCTION = 262,          // FUNCTION
            TOKEN_GLOBAL = 263,            // GLOBAL
            TOKEN_FOR = 264,               // FOR
            TOKEN_WHILE = 265,             // WHILE
            TOKEN_IF = 266,                // IF
            TOKEN_ELSE = 267,              // ELSE
            TOKEN_ADD = 268,               // ADD
            TOKEN_SUB = 269,               // SUB
            TOKEN_MUL = 270,               // MUL
            TOKEN_DIV = 271,               // DIV
            TOKEN_ASSIGN = 272,            // ASSIGN
            TOKEN_AND = 273,               // AND
            TOKEN_OR = 274,                // OR
            TOKEN_EQ = 275,                // EQ
            TOKEN_NE = 276,                // NE
            TOKEN_GT = 277,                // GT
            TOKEN_GE = 278,                // GE
            TOKEN_LT = 279,                // LT
            TOKEN_LE = 280,                // LE
            TOKEN_LP = 281,                // LP
            TOKEN_RP = 282,                // RP
            TOKEN_LC = 283,                // LC
            TOKEN_RC = 284,                // RC
            TOKEN_SEMICOLON = 285,         // SEMICOLON
            TOKEN_IDENTIFIER = 286,        // IDENTIFIER
            TOKEN_BREAK = 287,             // BREAK
            TOKEN_CONTINUE = 288,          // CONTINUE
            TOKEN_RETURN = 289,            // RETURN
            TOKEN_COMMA = 290,             // COMMA
            TOKEN_STRING = 291             // STRING
        };
        /// Backward compatibility alias (Bison 3.6).
        typedef token_kind_type yytokentype;
    };

    /// Token kind, as returned by yylex.
    typedef token::yytokentype token_kind_type;
    /// Backward compatibility alias (Bison 3.6).
    typedef token_kind_type token_type;
    /// Symbol kinds.
    struct symbol_kind {
        enum symbol_kind_type {
            YYNTOKENS = 37, ///< Number of tokens.
            S_YYEMPTY = -2,
            S_YYEOF = 0,                             // END
            S_YYerror = 1,                           // error
            S_YYUNDEF = 2,                           // "invalid token"
            S_NUMBER = 3,                            // NUMBER
            S_FLOAT = 4,                             // FLOAT
            S_EOL = 5,                               // EOL
            S_VAR = 6,                               // VAR
            S_FUNCTION = 7,                          // FUNCTION
            S_GLOBAL = 8,                            // GLOBAL
            S_FOR = 9,                               // FOR
            S_WHILE = 10,                            // WHILE
            S_IF = 11,                               // IF
            S_ELSE = 12,                             // ELSE
            S_ADD = 13,                              // ADD
            S_SUB = 14,                              // SUB
            S_MUL = 15,                              // MUL
            S_DIV = 16,                              // DIV
            S_ASSIGN = 17,                           // ASSIGN
            S_AND = 18,                              // AND
            S_OR = 19,                               // OR
            S_EQ = 20,                               // EQ
            S_NE = 21,                               // NE
            S_GT = 22,                               // GT
            S_GE = 23,                               // GE
            S_LT = 24,                               // LT
            S_LE = 25,                               // LE
            S_LP = 26,                               // LP
            S_RP = 27,                               // RP
            S_LC = 28,                               // LC
            S_RC = 29,                               // RC
            S_SEMICOLON = 30,                        // SEMICOLON
            S_IDENTIFIER = 31,                       // IDENTIFIER
            S_BREAK = 32,                            // BREAK
            S_CONTINUE = 33,                         // CONTINUE
            S_RETURN = 34,                           // RETURN
            S_COMMA = 35,                            // COMMA
            S_STRING = 36,                           // STRING
            S_YYACCEPT = 37,                         // $accept
            S_translation_unit = 38,                 // translation_unit
            S_definition_or_statement = 39,          // definition_or_statement
            S_function_definition = 40,              // function_definition
            S_statement = 41,                        // statement
            S_expression_statement = 42,             // expression_statement
            S_expression = 43,                       // expression
            S_value_expression = 44,                 // value_expression
            S_compare_expression = 45,               // compare_expression
            S_add_sub_expression = 46,               // add_sub_expression
            S_mul_div_expression = 47,               // mul_div_expression
            S_primary_expression = 48,               // primary_expression
            S_statement_list = 49,                   // statement_list
            S_block = 50,                            // block
            S_arglist = 51,                          // arglist
            S_arg = 52,                              // arg
            S_expression_option = 53,                // expression_option
            S_for_statement = 54,                    // for_statement
            S_while_statement = 55,                  // while_statement
            S_if_statement = 56,                     // if_statement
            S_break_statement = 57,                  // break_statement
            S_continue_statement = 58,               // continue_statement
            S_return_statement = 59                  // return_statement
        };
    };

    /// (Internal) symbol kind.
    typedef symbol_kind::symbol_kind_type symbol_kind_type;
    /// The number of tokens.
    static const symbol_kind_type YYNTOKENS = symbol_kind::YYNTOKENS;
    /// A complete symbol.
    ///
    /// Expects its Base type to provide access to the symbol kind
    /// via kind ().
    ///
    /// Provide access to semantic value and location.
    template <typename Base>
    struct basic_symbol : Base {
        /// Alias to Base.
        typedef Base super_type;
        /// Default constructor.
        basic_symbol()
            : value(), location() {}
        #if 201103L <= YY_CPLUSPLUS
        /// Move constructor.
        basic_symbol(basic_symbol&& that)
            : Base(std::move (that)), value (), location(std::move (that.location))
        {
            switch (this->kind ()) {
                case symbol_kind::S_function_definition: // function_definition
                    value.move<ScriptFunction*>(std::move(that.value));
                    break;
                case symbol_kind::S_arglist: // arglist
                    value.move<ScriptParams*>(std::move(that.value));
                    break;
                case symbol_kind::S_definition_or_statement: // definition_or_statement
                case symbol_kind::S_expression: // expression
                case symbol_kind::S_value_expression: // value_expression
                case symbol_kind::S_compare_expression: // compare_expression
                case symbol_kind::S_add_sub_expression: // add_sub_expression
                case symbol_kind::S_mul_div_expression: // mul_div_expression
                case symbol_kind::S_primary_expression: // primary_expression
                case symbol_kind::S_arg: // arg
                case symbol_kind::S_expression_option: // expression_option
                    value.move<UScriptExpression*>(std::move(that.value));
                    break;
                case symbol_kind::S_statement: // statement
                case symbol_kind::S_expression_statement: // expression_statement
                case symbol_kind::S_for_statement: // for_statement
                case symbol_kind::S_while_statement: // while_statement
                case symbol_kind::S_if_statement: // if_statement
                case symbol_kind::S_break_statement: // break_statement
                case symbol_kind::S_continue_statement: // continue_statement
                case symbol_kind::S_return_statement: // return_statement
                    value.move<UScriptStatement*>(std::move(that.value));
                    break;
                case symbol_kind::S_statement_list: // statement_list
                case symbol_kind::S_block: // block
                    value.move<UScriptStatementList*>(std::move(that.value));
                    break;
                case symbol_kind::S_FLOAT: // FLOAT
                    value.move<float>(std::move(that.value));
                    break;
                case symbol_kind::S_NUMBER: // NUMBER
                    value.move<int> (std::move(that.value));
                    break;
                case symbol_kind::S_VAR: // VAR
                case symbol_kind::S_FUNCTION: // FUNCTION
                case symbol_kind::S_GLOBAL: // GLOBAL
                case symbol_kind::S_FOR: // FOR
                case symbol_kind::S_WHILE: // WHILE
                case symbol_kind::S_IF: // IF
                case symbol_kind::S_ELSE: // ELSE
                case symbol_kind::S_ADD: // ADD
                case symbol_kind::S_SUB: // SUB
                case symbol_kind::S_MUL: // MUL
                case symbol_kind::S_DIV: // DIV
                case symbol_kind::S_ASSIGN: // ASSIGN
                case symbol_kind::S_AND: // AND
                case symbol_kind::S_OR: // OR
                case symbol_kind::S_EQ: // EQ
                case symbol_kind::S_NE: // NE
                case symbol_kind::S_GT: // GT
                case symbol_kind::S_GE: // GE
                case symbol_kind::S_LT: // LT
                case symbol_kind::S_LE: // LE
                case symbol_kind::S_LP: // LP
                case symbol_kind::S_RP: // RP
                case symbol_kind::S_LC: // LC
                case symbol_kind::S_RC: // RC
                case symbol_kind::S_SEMICOLON: // SEMICOLON
                case symbol_kind::S_IDENTIFIER: // IDENTIFIER
                case symbol_kind::S_BREAK: // BREAK
                case symbol_kind::S_CONTINUE: // CONTINUE
                case symbol_kind::S_RETURN: // RETURN
                case symbol_kind::S_COMMA: // COMMA
                case symbol_kind::S_STRING: // STRING
                    value.move<string>(std::move(that.value));
                    break;
                default:
                    break;
            }
        }
#endif
        /// Copy constructor.
        basic_symbol(const basic_symbol& that);
      /// Constructor for valueless symbols, and symbols from each type.
#if 201103L <= YY_CPLUSPLUS
        basic_symbol(typename Base::kind_type t, location_type&& l)
            : Base (t), location (std::move (l)) {}
#else
        basic_symbol(typename Base::kind_type t, const location_type& l)
            : Base (t), location (l) {}
#endif
#if 201103L <= YY_CPLUSPLUS
        basic_symbol(typename Base::kind_type t, ScriptFunction*&& v, location_type&& l)
            : Base (t), value (std::move (v)), location (std::move (l)) {}
#else
        basic_symbol(typename Base::kind_type t, const ScriptFunction*& v, const location_type& l)
            : Base (t), value (v), location (l) {}
#endif
#if 201103L <= YY_CPLUSPLUS
        basic_symbol(typename Base::kind_type t, ScriptParams*&& v, location_type&& l)
            : Base (t), value (std::move (v)), location (std::move (l)) {}
#else
        basic_symbol(typename Base::kind_type t, const ScriptParams*& v, const location_type& l)
            : Base (t), value (v), location (l) {}
#endif
#if 201103L <= YY_CPLUSPLUS
        basic_symbol(typename Base::kind_type t, UScriptExpression*&& v, location_type&& l)
            : Base (t), value (std::move (v)), location (std::move (l)) {}
#else
        basic_symbol(typename Base::kind_type t, const UScriptExpression*& v, const location_type& l)
            : Base (t), value (v), location (l) {}
#endif
#if 201103L <= YY_CPLUSPLUS
        basic_symbol(typename Base::kind_type t, UScriptStatement*&& v, location_type&& l)
            : Base(t), value(std::move(v)), location(std::move(l)) {}
#else
        basic_symbol(typename Base::kind_type t, const UScriptStatement*& v, const location_type& l)
            : Base(t), value(v), location(l) {}
#endif
#if 201103L <= YY_CPLUSPLUS
        basic_symbol(typename Base::kind_type t, UScriptStatementList*&& v, location_type&& l)
            : Base(t), value(std::move(v)), location(std::move(l))
      {}
#else
        basic_symbol(typename Base::kind_type t, const UScriptStatementList*& v, const location_type& l)
            : Base(t), value(v), location(l) {}
#endif
#if 201103L <= YY_CPLUSPLUS
        basic_symbol(typename Base::kind_type t, float&& v, location_type&& l)
            : Base(t), value(std::move(v)), location(std::move(l)) {}
#else
        basic_symbol(typename Base::kind_type t, const float& v, const location_type& l)
            : Base(t), value(v), location(l) {}
#endif
#if 201103L <= YY_CPLUSPLUS
        basic_symbol(typename Base::kind_type t, int&& v, location_type&& l)
            : Base(t), value(std::move(v)), location(std::move(l)) {}
#else
        basic_symbol(typename Base::kind_type t, const int& v, const location_type& l)
            : Base(t), value(v), location(l) {}
#endif
#if 201103L <= YY_CPLUSPLUS
        basic_symbol(typename Base::kind_type t, string&& v, location_type&& l)
            : Base(t), value(std::move(v)), location(std::move(l)) {}
#else
        basic_symbol(typename Base::kind_type t, const string& v, const location_type& l)
            : Base(t), value(v), location(l) {}
#endif

        /// Destroy the symbol.
        ~basic_symbol()
        {
            clear();
        }
        /// Destroy contents, and record that is empty.
        void clear()
        {
            // User destructor.
            symbol_kind_type yykind = this->kind();
            basic_symbol<Base>& yysym = *this;
            (void) yysym;
            switch (yykind) {
                default:
                    break;
            }

            // Value type destructor.
            switch (yykind) {
                case symbol_kind::S_function_definition: // function_definition
                    value.template destroy<ScriptFunction*>();
                    break;
                case symbol_kind::S_arglist: // arglist
                    value.template destroy<ScriptParams*>();
                    break;
                case symbol_kind::S_definition_or_statement: // definition_or_statement
                case symbol_kind::S_expression: // expression
                case symbol_kind::S_value_expression: // value_expression
                case symbol_kind::S_compare_expression: // compare_expression
                case symbol_kind::S_add_sub_expression: // add_sub_expression
                case symbol_kind::S_mul_div_expression: // mul_div_expression
                case symbol_kind::S_primary_expression: // primary_expression
                case symbol_kind::S_arg: // arg
                case symbol_kind::S_expression_option: // expression_option
                    value.template destroy<UScriptExpression*>();
                    break;
                case symbol_kind::S_statement: // statement
                case symbol_kind::S_expression_statement: // expression_statement
                case symbol_kind::S_for_statement: // for_statement
                case symbol_kind::S_while_statement: // while_statement
                case symbol_kind::S_if_statement: // if_statement
                case symbol_kind::S_break_statement: // break_statement
                case symbol_kind::S_continue_statement: // continue_statement
                case symbol_kind::S_return_statement: // return_statement
                    value.template destroy< UScriptStatement* > ();
                  break;
                case symbol_kind::S_statement_list: // statement_list
                case symbol_kind::S_block: // block
                    value.template destroy< UScriptStatementList* > ();
                  break;
                case symbol_kind::S_FLOAT: // FLOAT
                    value.template destroy< float > ();
                  break;
                case symbol_kind::S_NUMBER: // NUMBER
                    value.template destroy< int > ();
                    break;
                case symbol_kind::S_VAR: // VAR
                case symbol_kind::S_FUNCTION: // FUNCTION
                case symbol_kind::S_GLOBAL: // GLOBAL
                case symbol_kind::S_FOR: // FOR
                case symbol_kind::S_WHILE: // WHILE
                case symbol_kind::S_IF: // IF
                case symbol_kind::S_ELSE: // ELSE
                case symbol_kind::S_ADD: // ADD
                case symbol_kind::S_SUB: // SUB
                case symbol_kind::S_MUL: // MUL
                case symbol_kind::S_DIV: // DIV
                case symbol_kind::S_ASSIGN: // ASSIGN
                case symbol_kind::S_AND: // AND
                case symbol_kind::S_OR: // OR
                case symbol_kind::S_EQ: // EQ
                case symbol_kind::S_NE: // NE
                case symbol_kind::S_GT: // GT
                case symbol_kind::S_GE: // GE
                case symbol_kind::S_LT: // LT
                case symbol_kind::S_LE: // LE
                case symbol_kind::S_LP: // LP
                case symbol_kind::S_RP: // RP
                case symbol_kind::S_LC: // LC
                case symbol_kind::S_RC: // RC
                case symbol_kind::S_SEMICOLON: // SEMICOLON
                case symbol_kind::S_IDENTIFIER: // IDENTIFIER
                case symbol_kind::S_BREAK: // BREAK
                case symbol_kind::S_CONTINUE: // CONTINUE
                case symbol_kind::S_RETURN: // RETURN
                case symbol_kind::S_COMMA: // COMMA
                case symbol_kind::S_STRING: // STRING
                    value.template destroy< string > ();
                    break;
                default:
                    break;
            }
            Base::clear ();
        }
        /// The user-facing name of this symbol.
        std::string name() const YY_NOEXCEPT
        {
            return  Parser::symbol_name (this->kind ());
        }
        /// Backward compatibility (Bison 3.6).
        symbol_kind_type type_get() const YY_NOEXCEPT;
        /// Whether empty.
        bool empty() const YY_NOEXCEPT;
        /// Destructive move, \a s is emptied into this.
        void move(basic_symbol& s);
        /// The semantic value.
        semantic_type value;
        /// The location.
        location_type location;
    private:
#if YY_CPLUSPLUS < 201103L
        /// Assignment operator.
        basic_symbol& operator= (const basic_symbol& that);
#endif
    };
    /// Type access provider for token (enum) based symbols.
    struct by_kind {
        /// Default constructor.
        by_kind ();
#if 201103L <= YY_CPLUSPLUS
        /// Move constructor.
        by_kind (by_kind&& that);
#endif
        /// Copy constructor.
        by_kind (const by_kind& that);
        /// The symbol kind as needed by the constructor.
        typedef token_kind_type kind_type;
        /// Constructor from (external) token numbers.
        by_kind (kind_type t);
        /// Record that this symbol is empty.
        void clear ();
        /// Steal the symbol kind from \a that.
        void move (by_kind& that);
        /// The (internal) type number (corresponding to \a type).
        /// \a empty when empty.
        symbol_kind_type kind () const YY_NOEXCEPT;
        /// Backward compatibility (Bison 3.6).
        symbol_kind_type type_get () const YY_NOEXCEPT;
        /// The symbol kind.
        /// \a S_YYEMPTY when empty.
        symbol_kind_type kind_;
    };

    /// Backward compatibility for a private implementation detail (Bison 3.6).
    typedef by_kind by_type;
    /// "External" symbols: returned by the scanner.
    struct symbol_type : basic_symbol<by_kind> {
        /// Superclass.
        typedef basic_symbol<by_kind> super_type;
        /// Empty symbol.
        symbol_type() {}
        /// Constructor for valueless symbols, and symbols from each type.
#if 201103L <= YY_CPLUSPLUS
        symbol_type(int tok, location_type l)
            : super_type(token_type (tok), std::move (l))
        {
            YY_ASSERT(tok == token::TOKEN_END || tok == token::TOKEN_YYerror ||
                tok == token::TOKEN_YYUNDEF || tok == token::TOKEN_EOL);
        }
#else
        symbol_type(int tok, const location_type& l)
            : super_type(token_type (tok), l)
        {
            YY_ASSERT(tok == token::TOKEN_END || tok == token::TOKEN_YYerror ||
                tok == token::TOKEN_YYUNDEF || tok == token::TOKEN_EOL);
        }
#endif
#if 201103L <= YY_CPLUSPLUS
        symbol_type(int tok, float v, location_type l)
            : super_type(token_type (tok), std::move (v), std::move (l))
        {
            YY_ASSERT(tok == token::TOKEN_FLOAT);
        }
#else
        symbol_type(int tok, const float& v, const location_type& l)
            : super_type(token_type (tok), v, l)
        {
            YY_ASSERT(tok == token::TOKEN_FLOAT);
        }
#endif
#if 201103L <= YY_CPLUSPLUS
        symbol_type(int tok, int v, location_type l)
            : super_type(token_type (tok), std::move (v), std::move (l))
        {
            YY_ASSERT(tok == token::TOKEN_NUMBER);
        }
#else
        symbol_type(int tok, const int& v, const location_type& l)
            : super_type(token_type (tok), v, l)
        {
            YY_ASSERT(tok == token::TOKEN_NUMBER);
        }
#endif
#if 201103L <= YY_CPLUSPLUS
        symbol_type(int tok, string v, location_type l)
        : super_type(token_type (tok), std::move (v), std::move (l))
        {
            YY_ASSERT(tok == token::TOKEN_VAR || tok == token::TOKEN_FUNCTION || tok == token::TOKEN_GLOBAL ||
                tok == token::TOKEN_FOR || tok == token::TOKEN_WHILE || tok == token::TOKEN_IF ||
                tok == token::TOKEN_ELSE || tok == token::TOKEN_ADD || tok == token::TOKEN_SUB ||
                tok == token::TOKEN_MUL || tok == token::TOKEN_DIV || tok == token::TOKEN_ASSIGN ||
                tok == token::TOKEN_AND || tok == token::TOKEN_OR || tok == token::TOKEN_EQ ||
                tok == token::TOKEN_NE || tok == token::TOKEN_GT || tok == token::TOKEN_GE ||
                tok == token::TOKEN_LT || tok == token::TOKEN_LE || tok == token::TOKEN_LP ||
                tok == token::TOKEN_RP || tok == token::TOKEN_LC || tok == token::TOKEN_RC ||
                tok == token::TOKEN_SEMICOLON || tok == token::TOKEN_IDENTIFIER || tok == token::TOKEN_BREAK ||
                tok == token::TOKEN_CONTINUE || tok == token::TOKEN_RETURN || tok == token::TOKEN_COMMA ||
                tok == token::TOKEN_STRING);
        }
#else
        symbol_type(int tok, const string& v, const location_type& l)
            : super_type(token_type (tok), v, l)
        {
            YY_ASSERT(tok == token::TOKEN_VAR || tok == token::TOKEN_FUNCTION ||
                tok == token::TOKEN_GLOBAL || tok == token::TOKEN_FOR || tok == token::TOKEN_WHILE ||
                tok == token::TOKEN_IF || tok == token::TOKEN_ELSE || tok == token::TOKEN_ADD ||
                tok == token::TOKEN_SUB || tok == token::TOKEN_MUL || tok == token::TOKEN_DIV ||
                tok == token::TOKEN_ASSIGN || tok == token::TOKEN_AND || tok == token::TOKEN_OR ||
                tok == token::TOKEN_EQ || tok == token::TOKEN_NE || tok == token::TOKEN_GT ||
                tok == token::TOKEN_GE || tok == token::TOKEN_LT || tok == token::TOKEN_LE ||
                tok == token::TOKEN_LP || tok == token::TOKEN_RP || tok == token::TOKEN_LC ||
                tok == token::TOKEN_RC || tok == token::TOKEN_SEMICOLON || tok == token::TOKEN_IDENTIFIER ||
                tok == token::TOKEN_BREAK || tok == token::TOKEN_CONTINUE || tok == token::TOKEN_RETURN ||
                tok == token::TOKEN_COMMA || tok == token::TOKEN_STRING);
        }
#endif
    };
    /// Build a parser object.
    Parser(uscript::Scanner* scanner_yyarg, uscript::ScriptInterpreter* interpreter_yyarg);
    virtual ~ Parser();
#if 201103L <= YY_CPLUSPLUS
    /// Non copyable.
    Parser(const  Parser &) = delete;
    /// Non copyable.
    Parser& operator=(const  Parser&) = delete;
#endif
    /// Parse.  An alias for parse ().
    /// \returns  0 iff parsing succeeded.
    int operator() ();
    /// Parse.
    /// \returns  0 iff parsing succeeded.
    virtual int parse();

#if YYDEBUG
    /// The current debugging stream.
    std::ostream& debug_stream () const YY_ATTRIBUTE_PURE;
    /// Set the current debugging stream.
    void set_debug_stream (std::ostream &);
    /// Type for debugging levels.
    typedef int debug_level_type;
    /// The current debugging level.
    debug_level_type debug_level() const YY_ATTRIBUTE_PURE;
    /// Set the current debugging level.
    void set_debug_level(debug_level_type l);
#endif

    /// Report a syntax error.
    /// \param loc    where the syntax error is found.
    /// \param msg    a description of the syntax error.
    virtual void error(const location_type& loc, const std::string& msg);
    /// Report a syntax error.
    void error(const syntax_error& err);
    /// The user-facing name of the symbol whose (internal) number is
    /// YYSYMBOL.  No bounds checking.
    static std::string symbol_name(symbol_kind_type yysymbol);

    // Implementation of make_symbol for each symbol type.
#if 201103L <= YY_CPLUSPLUS
    static symbol_type make_END(location_type l)
    {
        return symbol_type(token::TOKEN_END, std::move (l));
    }
#else
    static symbol_type make_END(const location_type& l)
    {
        return symbol_type(token::TOKEN_END, l);
    }
#endif
#if 201103L <= YY_CPLUSPLUS
    static symbol_type make_YYerror(location_type l)
    {
        return symbol_type(token::TOKEN_YYerror, std::move (l));
    }
#else
    static symbol_type make_YYerror(const location_type& l)
    {
        return symbol_type(token::TOKEN_YYerror, l);
    }
#endif
#if 201103L <= YY_CPLUSPLUS
    static symbol_type make_YYUNDEF(location_type l)
    {
        return symbol_type(token::TOKEN_YYUNDEF, std::move (l));
    }
#else
    static symbol_type make_YYUNDEF(const location_type& l)
    {
        return symbol_type(token::TOKEN_YYUNDEF, l);
    }
#endif
#if 201103L <= YY_CPLUSPLUS
    static symbol_type make_NUMBER(int v, location_type l)
    {
        return symbol_type(token::TOKEN_NUMBER, std::move (v), std::move (l));
    }
#else
    static symbol_type make_NUMBER (const int& v, const location_type& l)
    {
        return symbol_type(token::TOKEN_NUMBER, v, l);
    }
#endif
#if 201103L <= YY_CPLUSPLUS
    static symbol_type make_FLOAT (float v, location_type l)
    {
        return symbol_type(token::TOKEN_FLOAT, std::move (v), std::move (l));
    }
#else
    static symbol_type make_FLOAT (const float& v, const location_type& l)
    {
        return symbol_type(token::TOKEN_FLOAT, v, l);
    }
#endif
#if 201103L <= YY_CPLUSPLUS
    static symbol_type make_EOL (location_type l)
    {
        return symbol_type(token::TOKEN_EOL, std::move (l));
    }
#else
    static symbol_type make_EOL (const location_type& l)
    {
        return symbol_type(token::TOKEN_EOL, l);
    }
#endif
#if 201103L <= YY_CPLUSPLUS
    static symbol_type make_VAR (string v, location_type l)
    {
        return symbol_type(token::TOKEN_VAR, std::move (v), std::move (l));
    }
#else
    static symbol_type make_VAR (const string& v, const location_type& l)
    {
        return symbol_type(token::TOKEN_VAR, v, l);
    }
#endif
#if 201103L <= YY_CPLUSPLUS
    static symbol_type make_FUNCTION (string v, location_type l)
    {
        return symbol_type(token::TOKEN_FUNCTION, std::move (v), std::move (l));
    }
#else
    static symbol_type make_FUNCTION (const string& v, const location_type& l)
    {
        return symbol_type(token::TOKEN_FUNCTION, v, l);
    }
#endif
#if 201103L <= YY_CPLUSPLUS
    static symbol_type make_GLOBAL (string v, location_type l)
    {
        return symbol_type(token::TOKEN_GLOBAL, std::move (v), std::move (l));
    }
#else
    static symbol_type make_GLOBAL (const string& v, const location_type& l)
    {
        return symbol_type(token::TOKEN_GLOBAL, v, l);
    }
#endif
#if 201103L <= YY_CPLUSPLUS
    static symbol_type make_FOR (string v, location_type l)
    {
        return symbol_type(token::TOKEN_FOR, std::move (v), std::move (l));
    }
#else
    static symbol_type make_FOR (const string& v, const location_type& l)
    {
        return symbol_type(token::TOKEN_FOR, v, l);
    }
#endif
#if 201103L <= YY_CPLUSPLUS
    static symbol_type make_WHILE (string v, location_type l)
    {
        return symbol_type(token::TOKEN_WHILE, std::move (v), std::move (l));
    }
#else
    static symbol_type make_WHILE (const string& v, const location_type& l)
    {
        return symbol_type(token::TOKEN_WHILE, v, l);
    }
#endif
#if 201103L <= YY_CPLUSPLUS
    static symbol_type make_IF (string v, location_type l)
    {
        return symbol_type(token::TOKEN_IF, std::move (v), std::move (l));
    }
#else
    static symbol_type make_IF (const string& v, const location_type& l)
    {
        return symbol_type(token::TOKEN_IF, v, l);
    }
#endif
#if 201103L <= YY_CPLUSPLUS
    static symbol_type make_ELSE (string v, location_type l)
    {
        return symbol_type(token::TOKEN_ELSE, std::move (v), std::move (l));
    }
#else
    static symbol_type make_ELSE (const string& v, const location_type& l)
    {
        return symbol_type(token::TOKEN_ELSE, v, l);
    }
#endif
#if 201103L <= YY_CPLUSPLUS
    static symbol_type make_ADD (string v, location_type l)
    {
        return symbol_type(token::TOKEN_ADD, std::move (v), std::move (l));
    }
#else
    static symbol_type make_ADD (const string& v, const location_type& l)
    {
        return symbol_type(token::TOKEN_ADD, v, l);
    }
#endif
#if 201103L <= YY_CPLUSPLUS
    static symbol_type make_SUB (string v, location_type l)
    {
        return symbol_type(token::TOKEN_SUB, std::move (v), std::move (l));
    }
#else
    static symbol_type make_SUB (const string& v, const location_type& l)
    {
        return symbol_type(token::TOKEN_SUB, v, l);
    }
#endif
#if 201103L <= YY_CPLUSPLUS
    static symbol_type make_MUL (string v, location_type l)
    {
        return symbol_type(token::TOKEN_MUL, std::move (v), std::move (l));
    }
#else
    static symbol_type make_MUL (const string& v, const location_type& l)
    {
        return symbol_type(token::TOKEN_MUL, v, l);
    }
#endif
#if 201103L <= YY_CPLUSPLUS
    static symbol_type make_DIV (string v, location_type l)
    {
        return symbol_type(token::TOKEN_DIV, std::move (v), std::move (l));
    }
#else
    static symbol_type make_DIV (const string& v, const location_type& l)
    {
        return symbol_type(token::TOKEN_DIV, v, l);
    }
#endif
#if 201103L <= YY_CPLUSPLUS
    static symbol_type make_ASSIGN (string v, location_type l)
    {
        return symbol_type(token::TOKEN_ASSIGN, std::move (v), std::move (l));
    }
#else
    static symbol_type make_ASSIGN (const string& v, const location_type& l)
    {
        return symbol_type(token::TOKEN_ASSIGN, v, l);
    }
#endif
#if 201103L <= YY_CPLUSPLUS
    static symbol_type make_AND (string v, location_type l)
    {
        return symbol_type(token::TOKEN_AND, std::move (v), std::move (l));
    }
#else
    static symbol_type make_AND (const string& v, const location_type& l)
    {
        return symbol_type(token::TOKEN_AND, v, l);
    }
#endif
#if 201103L <= YY_CPLUSPLUS
    static symbol_type make_OR (string v, location_type l)
    {
        return symbol_type(token::TOKEN_OR, std::move (v), std::move (l));
    }
#else
    static symbol_type make_OR (const string& v, const location_type& l)
    {
        return symbol_type(token::TOKEN_OR, v, l);
    }
#endif
#if 201103L <= YY_CPLUSPLUS
    static symbol_type make_EQ (string v, location_type l)
    {
        return symbol_type(token::TOKEN_EQ, std::move (v), std::move (l));
    }
#else
    static symbol_type make_EQ (const string& v, const location_type& l)
    {
        return symbol_type(token::TOKEN_EQ, v, l);
    }
#endif
#if 201103L <= YY_CPLUSPLUS
    static symbol_type make_NE (string v, location_type l)
    {
        return symbol_type(token::TOKEN_NE, std::move (v), std::move (l));
    }
#else
    static symbol_type make_NE (const string& v, const location_type& l)
    {
        return symbol_type(token::TOKEN_NE, v, l);
    }
#endif
#if 201103L <= YY_CPLUSPLUS
    static symbol_type make_GT (string v, location_type l)
    {
        return symbol_type(token::TOKEN_GT, std::move (v), std::move (l));
    }
#else
    static symbol_type make_GT (const string& v, const location_type& l)
    {
        return symbol_type(token::TOKEN_GT, v, l);
    }
#endif
#if 201103L <= YY_CPLUSPLUS
    static symbol_type make_GE (string v, location_type l)
    {
        return symbol_type(token::TOKEN_GE, std::move (v), std::move (l));
    }
#else
    static symbol_type make_GE (const string& v, const location_type& l)
    {
        return symbol_type(token::TOKEN_GE, v, l);
    }
#endif
#if 201103L <= YY_CPLUSPLUS
    static symbol_type make_LT (string v, location_type l)
    {
        return symbol_type(token::TOKEN_LT, std::move (v), std::move (l));
    }
#else
    static symbol_type make_LT (const string& v, const location_type& l)
    {
        return symbol_type(token::TOKEN_LT, v, l);
    }
#endif
#if 201103L <= YY_CPLUSPLUS
    static symbol_type make_LE (string v, location_type l)
    {
        return symbol_type(token::TOKEN_LE, std::move (v), std::move (l));
    }
#else
    static symbol_type make_LE (const string& v, const location_type& l)
    {
        return symbol_type(token::TOKEN_LE, v, l);
    }
#endif
#if 201103L <= YY_CPLUSPLUS
    static symbol_type make_LP (string v, location_type l)
    {
        return symbol_type(token::TOKEN_LP, std::move (v), std::move (l));
    }
#else
    static symbol_type make_LP (const string& v, const location_type& l)
    {
        return symbol_type(token::TOKEN_LP, v, l);
    }
#endif
#if 201103L <= YY_CPLUSPLUS
    static symbol_type make_RP (string v, location_type l)
    {
        return symbol_type(token::TOKEN_RP, std::move (v), std::move (l));
    }
#else
    static symbol_type make_RP (const string& v, const location_type& l)
    {
        return symbol_type(token::TOKEN_RP, v, l);
    }
#endif
#if 201103L <= YY_CPLUSPLUS
    static symbol_type make_LC (string v, location_type l)
    {
        return symbol_type(token::TOKEN_LC, std::move (v), std::move (l));
    }
#else
    static symbol_type make_LC (const string& v, const location_type& l)
    {
        return symbol_type(token::TOKEN_LC, v, l);
    }
#endif
#if 201103L <= YY_CPLUSPLUS
    static symbol_type make_RC (string v, location_type l)
    {
        return symbol_type(token::TOKEN_RC, std::move (v), std::move (l));
    }
#else
    static symbol_type make_RC (const string& v, const location_type& l)
    {
        return symbol_type(token::TOKEN_RC, v, l);
    }
#endif
#if 201103L <= YY_CPLUSPLUS
    static symbol_type make_SEMICOLON (string v, location_type l)
    {
        return symbol_type(token::TOKEN_SEMICOLON, std::move (v), std::move (l));
    }
#else
    static symbol_type make_SEMICOLON (const string& v, const location_type& l)
    {
        return symbol_type(token::TOKEN_SEMICOLON, v, l);
    }
#endif
#if 201103L <= YY_CPLUSPLUS
    static symbol_type make_IDENTIFIER (string v, location_type l)
    {
        return symbol_type(token::TOKEN_IDENTIFIER, std::move (v), std::move (l));
    }
#else
    static symbol_type make_IDENTIFIER (const string& v, const location_type& l)
    {
        return symbol_type(token::TOKEN_IDENTIFIER, v, l);
    }
#endif
#if 201103L <= YY_CPLUSPLUS
    static symbol_type make_BREAK (string v, location_type l)
    {
        return symbol_type(token::TOKEN_BREAK, std::move (v), std::move (l));
    }
#else
    static symbol_type make_BREAK (const string& v, const location_type& l)
    {
        return symbol_type(token::TOKEN_BREAK, v, l);
    }
#endif
#if 201103L <= YY_CPLUSPLUS
    static symbol_type make_CONTINUE (string v, location_type l)
    {
        return symbol_type(token::TOKEN_CONTINUE, std::move (v), std::move (l));
    }
#else
    static symbol_type make_CONTINUE (const string& v, const location_type& l)
    {
        return symbol_type(token::TOKEN_CONTINUE, v, l);
    }
#endif
#if 201103L <= YY_CPLUSPLUS
    static symbol_type make_RETURN (string v, location_type l)
    {
        return symbol_type(token::TOKEN_RETURN, std::move (v), std::move (l));
    }
#else
    static symbol_type make_RETURN (const string& v, const location_type& l)
    {
        return symbol_type(token::TOKEN_RETURN, v, l);
    }
#endif
#if 201103L <= YY_CPLUSPLUS
    static symbol_type make_COMMA (string v, location_type l)
    {
        return symbol_type(token::TOKEN_COMMA, std::move (v), std::move (l));
    }
#else
    static symbol_type make_COMMA (const string& v, const location_type& l)
    {
        return symbol_type(token::TOKEN_COMMA, v, l);
    }
#endif
#if 201103L <= YY_CPLUSPLUS
    static symbol_type make_STRING (string v, location_type l)
    {
        return symbol_type(token::TOKEN_STRING, std::move (v), std::move (l));
    }
#else
    static symbol_type make_STRING (const string& v, const location_type& l)
    {
        return symbol_type(token::TOKEN_STRING, v, l);
    }
#endif
    class context {
    public:
        context (const  Parser & yyparser, const symbol_type& yyla);
        const symbol_type& lookahead () const { return yyla_; }
        symbol_kind_type token () const { return yyla_.kind (); }
        const location_type& location () const { return yyla_.location; }

        /// Put in YYARG at most YYARGN of the expected tokens, and return the
        /// number of tokens stored in YYARG.  If YYARG is null, return the
        /// number of expected tokens (guaranteed to be less than YYNTOKENS).
        int expected_tokens (symbol_kind_type yyarg[], int yyargn) const;
    private:
        const  Parser &yyparser_;
        const symbol_type &yyla_;
    };
private:
#if YY_CPLUSPLUS < 201103L
    /// Non copyable.
     Parser(const  Parser &);
    /// Non copyable.
     Parser& operator= (const  Parser &);
#endif
    /// Stored state numbers (used for stacks).
    typedef signed char state_type;
    /// The arguments of the error message.
    int yy_syntax_error_arguments_ (const context& yyctx,
                                    symbol_kind_type yyarg[], int yyargn) const;
    /// Generate an error message.
    /// \param yyctx     the context in which the error occurred.
    virtual std::string yysyntax_error_ (const context& yyctx) const;
    /// Compute post-reduction state.
    /// \param yystate   the current state
    /// \param yysym     the nonterminal to push on the stack
    static state_type yy_lr_goto_state_ (state_type yystate, int yysym);
    /// Whether the given \c yypact_ value indicates a defaulted state.
    /// \param yyvalue   the value to check
    static bool yy_pact_value_is_default_ (int yyvalue);
    /// Whether the given \c yytable_ value indicates a syntax error.
    /// \param yyvalue   the value to check
    static bool yy_table_value_is_error_ (int yyvalue);
    static const signed char yypact_ninf_;
    static const signed char yytable_ninf_;
    /// Convert a scanner token kind \a t to a symbol kind.
    /// In theory \a t should be a token_kind_type, but character literals
    /// are valid, yet not members of the token_type enum.
    static symbol_kind_type yytranslate_ (int t);
    /// Convert the symbol name \a n to a form suitable for a diagnostic.
    static std::string yytnamerr_ (const char *yystr);
    /// For a symbol, its name in clear.
    static const char* const yytname_[];
    // Tables.
    // YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
    // STATE-NUM.
    static const short yypact_[];
    // YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
    // Performed when YYTABLE does not specify something else to do.  Zero
    // means the default is an error.
    static const signed char yydefact_[];
    // YYPGOTO[NTERM-NUM].
    static const short yypgoto_[];
    // YYDEFGOTO[NTERM-NUM].
    static const signed char yydefgoto_[];
    // YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
    // positive, shift that token.  If negative, reduce the rule whose
    // number is the opposite.  If YYTABLE_NINF, syntax error.
    static const signed char yytable_[];
    static const signed char yycheck_[];
    // YYSTOS[STATE-NUM] -- The (internal number of the) accessing
    // symbol of state STATE-NUM.
    static const signed char yystos_[];
    // YYR1[YYN] -- Symbol number of symbol that rule YYN derives.
    static const signed char yyr1_[];
    // YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.
    static const signed char yyr2_[];

#if YYDEBUG
    // YYRLINE[YYN] -- Source line where rule number YYN was defined.
    static const short yyrline_[];
    /// Report on the debug stream that the rule \a r is going to be reduced.
    virtual void yy_reduce_print_ (int r) const;
    /// Print the state stack on the debug stream.
    virtual void yy_stack_print_ () const;

    /// Debugging level.
    int yydebug_;
    /// Debug stream.
    std::ostream* yycdebug_;

    /// \brief Display a symbol kind, value and location.
    /// \param yyo    The output stream.
    /// \param yysym  The symbol.
    template <typename Base>
    void yy_print_ (std::ostream& yyo, const basic_symbol<Base>& yysym) const;
#endif

    /// \brief Reclaim the memory associated to a symbol.
    /// \param yymsg     Why this token is reclaimed.
    ///                  If null, print nothing.
    /// \param yysym     The symbol.
    template <typename Base>
    void yy_destroy_ (const char* yymsg, basic_symbol<Base>& yysym) const;

private:
    /// Type access provider for state based symbols.
    struct by_state {
        /// Default constructor.
        by_state () YY_NOEXCEPT;
        /// The symbol kind as needed by the constructor.
        typedef state_type kind_type;
        /// Constructor.
        by_state (kind_type s) YY_NOEXCEPT;
        /// Copy constructor.
        by_state (const by_state& that) YY_NOEXCEPT;
        /// Record that this symbol is empty.
        void clear () YY_NOEXCEPT;
        /// Steal the symbol kind from \a that.
        void move (by_state& that);
        /// The symbol kind (corresponding to \a state).
        /// \a symbol_kind::S_YYEMPTY when empty.
        symbol_kind_type kind () const YY_NOEXCEPT;
        /// The state number used to denote an empty symbol.
        /// We use the initial state, as it does not have a value.
        enum { empty_state = 0 };
        /// The state.
        /// \a empty when empty.
        state_type state;
    };

    /// "Internal" symbol: element of the stack.
    struct stack_symbol_type : basic_symbol<by_state> {
        /// Superclass.
        typedef basic_symbol<by_state> super_type;
        /// Construct an empty symbol.
        stack_symbol_type();
        /// Move or copy construction.
        stack_symbol_type(YY_RVREF (stack_symbol_type) that);
        /// Steal the contents from \a sym to build this.
        stack_symbol_type(state_type s, YY_MOVE_REF (symbol_type) sym);
#if YY_CPLUSPLUS < 201103L
        /// Assignment, needed by push_back by some old implementations.
        /// Moves the contents of that.
        stack_symbol_type& operator= (stack_symbol_type& that);

        /// Assignment, needed by push_back by other implementations.
        /// Needed by some other old implementations.
        stack_symbol_type& operator= (const stack_symbol_type& that);
#endif
    };

    /// A stack with random access from its top.
    template <typename T, typename S = std::vector<T> >
    class stack {
    public:
        // Hide our reversed order.
        typedef typename S::iterator iterator;
        typedef typename S::const_iterator const_iterator;
        typedef typename S::size_type size_type;
        typedef typename std::ptrdiff_t index_type;
        stack (size_type n = 200) : seq_(n) {}
#if 201103L <= YY_CPLUSPLUS
        /// Non copyable.
        stack (const stack&) = delete;
        /// Non copyable.
        stack& operator= (const stack&) = delete;
#endif
        /// Random access.
        ///
        /// Index 0 returns the topmost element.
        const T& operator[] (index_type i) const
        {
            return seq_[size_type (size () - 1 - i)];
        }
        /// Random access.
        ///
        /// Index 0 returns the topmost element.
        T& operator[] (index_type i)
        {
            return seq_[size_type (size () - 1 - i)];
        }
        /// Steal the contents of \a t.
        ///
        /// Close to move-semantics.
        void push (YY_MOVE_REF (T) t)
        {
            seq_.push_back (T ());
            operator[] (0).move (t);
        }
        /// Pop elements from the stack.
        void pop (std::ptrdiff_t n = 1) YY_NOEXCEPT
        {
            for (; 0 < n; --n) {
                seq_.pop_back ();
		    }
        }
        /// Pop all elements from the stack.
        void clear() YY_NOEXCEPT
        {
            seq_.clear ();
        }
        /// Number of elements on the stack.
        index_type size() const YY_NOEXCEPT
        {
            return index_type (seq_.size ());
        }
      /// Iterator on top of the stack (going downwards).
        const_iterator begin() const YY_NOEXCEPT
        {
            return seq_.begin();
        }
        /// Bottom of the stack.
        const_iterator end() const YY_NOEXCEPT
        {
            return seq_.end();
        }

      /// Present a slice of the top of a stack.
        class slice
        {
        public:
            slice(const stack& stack, index_type range) : stack_(stack) , range_(range) {}
            const T& operator[] (index_type i) const
            {
                return stack_[range_ - i];
            }
        private:
            const stack& stack_;
            index_type range_;
        };
    private:
#if YY_CPLUSPLUS < 201103L
        /// Non copyable.
        stack (const stack&);
        /// Non copyable.
        stack& operator= (const stack&);
#endif
        /// The wrapped container.
        S seq_;
    };

    /// Stack type.
    typedef stack<stack_symbol_type> stack_type;
    /// The stack.
    stack_type yystack_;
    /// Push a new state on the stack.
    /// \param m    a debug message to display
    ///             if null, no trace is output.
    /// \param sym  the symbol
    /// \warning the contents of \a s.value is stolen.
    void yypush_ (const char* m, YY_MOVE_REF (stack_symbol_type) sym);
    /// Push a new look ahead token on the state on the stack.
    /// \param m    a debug message to display
    ///             if null, no trace is output.
    /// \param s    the state
    /// \param sym  the symbol (for its value and location).
    /// \warning the contents of \a sym.value is stolen.
    void yypush_ (const char* m, state_type s, YY_MOVE_REF (symbol_type) sym);
    /// Pop \a n symbols from the stack.
    void yypop_ (int n = 1);
    /// Constants.
    enum {
      yylast_ = 187,     ///< Last index in yytable_.
      yynnts_ = 23,  ///< Number of nonterminal symbols.
      yyfinal_ = 47 ///< Termination state number.
    };
    // User arguments.
    uscript::Scanner* scanner;
    uscript::ScriptInterpreter* interpreter;
};

    inline Parser::symbol_kind_type Parser::yytranslate_(int t)
    {
        // YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to
        // TOKEN-NUM as returned by yylex.
        static const signed char translate_table[] =
        {
             0, 2, 2, 2, 2, 2, 2, 2, 2, 2,
             2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
             2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
             2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
             2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
             2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
             2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
             2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
             2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
             2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
             2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
             2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
             2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
             2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
             2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
             2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
             2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
             2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
             2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
             2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
             2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
             2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
             2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
             2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
             2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
             2, 2, 2, 2, 2, 2, 1, 2, 3, 4,
             5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
            15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
            25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36
        };
        // Last valid token kind.
        const int code_max = 291;
        if (t <= 0) {
            return symbol_kind::S_YYEOF;
        } else if (t <= code_max) {
            return YY_CAST (symbol_kind_type, translate_table[t]);
        } else {
            return symbol_kind::S_YYUNDEF;
        }
    }

    // basic_symbol.
    template <typename Base>
    Parser::basic_symbol<Base>::basic_symbol(const basic_symbol& that)
        : Base (that), value (), location (that.location)
    {
        switch (this->kind ()) {
            case symbol_kind::S_function_definition: // function_definition
                value.copy< ScriptFunction* > (YY_MOVE (that.value));
                break;
            case symbol_kind::S_arglist: // arglist
                value.copy< ScriptParams* > (YY_MOVE (that.value));
                break;
            case symbol_kind::S_definition_or_statement: // definition_or_statement
            case symbol_kind::S_expression: // expression
            case symbol_kind::S_value_expression: // value_expression
            case symbol_kind::S_compare_expression: // compare_expression
            case symbol_kind::S_add_sub_expression: // add_sub_expression
            case symbol_kind::S_mul_div_expression: // mul_div_expression
            case symbol_kind::S_primary_expression: // primary_expression
            case symbol_kind::S_arg: // arg
            case symbol_kind::S_expression_option: // expression_option
                value.copy< UScriptExpression* > (YY_MOVE (that.value));
                break;
            case symbol_kind::S_statement: // statement
            case symbol_kind::S_expression_statement: // expression_statement
            case symbol_kind::S_for_statement: // for_statement
            case symbol_kind::S_while_statement: // while_statement
            case symbol_kind::S_if_statement: // if_statement
            case symbol_kind::S_break_statement: // break_statement
            case symbol_kind::S_continue_statement: // continue_statement
            case symbol_kind::S_return_statement: // return_statement
                value.copy< UScriptStatement* > (YY_MOVE (that.value));
                break;
            case symbol_kind::S_statement_list: // statement_list
            case symbol_kind::S_block: // block
                value.copy< UScriptStatementList* > (YY_MOVE (that.value));
                break;
            case symbol_kind::S_FLOAT: // FLOAT
                value.copy< float > (YY_MOVE (that.value));
                break;
            case symbol_kind::S_NUMBER: // NUMBER
                value.copy< int > (YY_MOVE (that.value));
                break;
            case symbol_kind::S_VAR: // VAR
            case symbol_kind::S_FUNCTION: // FUNCTION
            case symbol_kind::S_GLOBAL: // GLOBAL
            case symbol_kind::S_FOR: // FOR
            case symbol_kind::S_WHILE: // WHILE
            case symbol_kind::S_IF: // IF
            case symbol_kind::S_ELSE: // ELSE
            case symbol_kind::S_ADD: // ADD
            case symbol_kind::S_SUB: // SUB
            case symbol_kind::S_MUL: // MUL
            case symbol_kind::S_DIV: // DIV
            case symbol_kind::S_ASSIGN: // ASSIGN
            case symbol_kind::S_AND: // AND
            case symbol_kind::S_OR: // OR
            case symbol_kind::S_EQ: // EQ
            case symbol_kind::S_NE: // NE
            case symbol_kind::S_GT: // GT
            case symbol_kind::S_GE: // GE
            case symbol_kind::S_LT: // LT
            case symbol_kind::S_LE: // LE
            case symbol_kind::S_LP: // LP
            case symbol_kind::S_RP: // RP
            case symbol_kind::S_LC: // LC
            case symbol_kind::S_RC: // RC
            case symbol_kind::S_SEMICOLON: // SEMICOLON
            case symbol_kind::S_IDENTIFIER: // IDENTIFIER
            case symbol_kind::S_BREAK: // BREAK
            case symbol_kind::S_CONTINUE: // CONTINUE
            case symbol_kind::S_RETURN: // RETURN
            case symbol_kind::S_COMMA: // COMMA
            case symbol_kind::S_STRING: // STRING
                value.copy< string > (YY_MOVE (that.value));
                break;
            default:
                break;
        }
    }

    template <typename Base> Parser::symbol_kind_type
    Parser::basic_symbol<Base>::type_get () const YY_NOEXCEPT
    {
        return this->kind ();
    }

    template <typename Base>
    bool Parser::basic_symbol<Base>::empty () const YY_NOEXCEPT
    {
        return this->kind () == symbol_kind::S_YYEMPTY;
    }

    template <typename Base>
    void Parser::basic_symbol<Base>::move (basic_symbol& s)
    {
        super_type::move (s);
        switch (this->kind()) {
            case symbol_kind::S_function_definition: // function_definition
                value.move< ScriptFunction* > (YY_MOVE (s.value));
                break;
            case symbol_kind::S_arglist: // arglist
                value.move< ScriptParams* > (YY_MOVE (s.value));
                break;
            case symbol_kind::S_definition_or_statement: // definition_or_statement
            case symbol_kind::S_expression: // expression
            case symbol_kind::S_value_expression: // value_expression
            case symbol_kind::S_compare_expression: // compare_expression
            case symbol_kind::S_add_sub_expression: // add_sub_expression
            case symbol_kind::S_mul_div_expression: // mul_div_expression
            case symbol_kind::S_primary_expression: // primary_expression
            case symbol_kind::S_arg: // arg
            case symbol_kind::S_expression_option: // expression_option
                value.move< UScriptExpression* > (YY_MOVE (s.value));
                break;
            case symbol_kind::S_statement: // statement
            case symbol_kind::S_expression_statement: // expression_statement
            case symbol_kind::S_for_statement: // for_statement
            case symbol_kind::S_while_statement: // while_statement
            case symbol_kind::S_if_statement: // if_statement
            case symbol_kind::S_break_statement: // break_statement
            case symbol_kind::S_continue_statement: // continue_statement
            case symbol_kind::S_return_statement: // return_statement
                value.move< UScriptStatement* > (YY_MOVE (s.value));
                break;
            case symbol_kind::S_statement_list: // statement_list
            case symbol_kind::S_block: // block
                value.move< UScriptStatementList* > (YY_MOVE (s.value));
                break;
            case symbol_kind::S_FLOAT: // FLOAT
                value.move< float > (YY_MOVE (s.value));
                break;
            case symbol_kind::S_NUMBER: // NUMBER
                value.move< int > (YY_MOVE (s.value));
                break;
            case symbol_kind::S_VAR: // VAR
            case symbol_kind::S_FUNCTION: // FUNCTION
            case symbol_kind::S_GLOBAL: // GLOBAL
            case symbol_kind::S_FOR: // FOR
            case symbol_kind::S_WHILE: // WHILE
            case symbol_kind::S_IF: // IF
            case symbol_kind::S_ELSE: // ELSE
            case symbol_kind::S_ADD: // ADD
            case symbol_kind::S_SUB: // SUB
            case symbol_kind::S_MUL: // MUL
            case symbol_kind::S_DIV: // DIV
            case symbol_kind::S_ASSIGN: // ASSIGN
            case symbol_kind::S_AND: // AND
            case symbol_kind::S_OR: // OR
            case symbol_kind::S_EQ: // EQ
            case symbol_kind::S_NE: // NE
            case symbol_kind::S_GT: // GT
            case symbol_kind::S_GE: // GE
            case symbol_kind::S_LT: // LT
            case symbol_kind::S_LE: // LE
            case symbol_kind::S_LP: // LP
            case symbol_kind::S_RP: // RP
            case symbol_kind::S_LC: // LC
            case symbol_kind::S_RC: // RC
            case symbol_kind::S_SEMICOLON: // SEMICOLON
            case symbol_kind::S_IDENTIFIER: // IDENTIFIER
            case symbol_kind::S_BREAK: // BREAK
            case symbol_kind::S_CONTINUE: // CONTINUE
            case symbol_kind::S_RETURN: // RETURN
            case symbol_kind::S_COMMA: // COMMA
            case symbol_kind::S_STRING: // STRING
                value.move< string > (YY_MOVE (s.value));
                break;
            default:
                break;
        }
        location = YY_MOVE (s.location);
    }

    // by_kind.
    inline Parser::by_kind::by_kind ()
        : kind_ (symbol_kind::S_YYEMPTY) {}

#if 201103L <= YY_CPLUSPLUS
    inline Parser::by_kind::by_kind(by_kind&& that)
        : kind_ (that.kind_)
    {
        that.clear ();
    }
#endif

    inline Parser::by_kind::by_kind(const by_kind& that)
        : kind_ (that.kind_) {}

    inline Parser::by_kind::by_kind(token_kind_type t)
        : kind_ (yytranslate_ (t)) {}

    inline void Parser::by_kind::clear()
    {
        kind_ = symbol_kind::S_YYEMPTY;
    }

    inline void Parser::by_kind::move(by_kind& that)
    {
        kind_ = that.kind_;
        that.clear ();
    }

    inline Parser::symbol_kind_type Parser::by_kind::kind() const YY_NOEXCEPT
    {
        return kind_;
    }

    inline Parser::symbol_kind_type Parser::by_kind::type_get() const YY_NOEXCEPT
    {
        return this->kind ();
    }
} // uscript

#endif // !YY_YY_YACC_PARSER_HPP_INCLUDED
