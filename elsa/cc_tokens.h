// cc_tokens.h

#ifndef CC_TOKENS_H
#define CC_TOKENS_H

#include "TokenKinds.h"

// token flags
enum TokenFlag {
  TF_MULTISPELL   = 0x01,     // token has multiple spellings
  TF_NONSEPARATOR = 0x02,     // token is a nonseparator
  TF_CPLUSPLUS    = 0x04,     // token is a keyword in C++, but an identifier in C
  ALL_TOKEN_FLAGS = 0x07      // bitwise OR of above
};

#define TOK(X) ellcc::tok::X

typedef ellcc::tok::TokenKind TokenType;
#define TOK_EOF                         TOK(eof)

  // non-keyword name
#define TOK_NAME                        TOK(identifier)

  // literals
#define TOK_INT_LITERAL                 TOK(int_constant)
#define TOK_FLOAT_LITERAL               TOK(numeric_constant)
#define TOK_STRING_LITERAL              TOK(string_literal)
#define TOK_CHAR_LITERAL                TOK(char_constant)

  // keywords
#define TOK_ASM                         KEYWORD(asm                         , KEYCXX|KEYGNU)
#define TOK_AUTO                        KEYWORD(auto                        , KEYALL)
#define TOK_BREAK                       KEYWORD(break                       , KEYALL)
#define TOK_BOOL                        KEYWORD(bool                        , KEYCXX)
#define TOK_CASE                        KEYWORD(case                        , KEYALL)
#define TOK_CATCH                       KEYWORD(catch                       , KEYCXX)
#define TOK_CDECL                       KEYWORD(__cdecl                     , KEYALL)
#define TOK_CHAR                        KEYWORD(char                        , KEYALL)
#define TOK_CLASS                       KEYWORD(class                       , KEYCXX)
#define TOK_CONST                       KEYWORD(const                       , KEYALL)
#define TOK_CONST_CAST                  KEYWORD(const_cast                  , KEYCXX)
#define TOK_CONTINUE                    KEYWORD(continue                    , KEYALL)
#define TOK_DEFAULT                     KEYWORD(default                     , KEYALL)
#define TOK_DELETE                      KEYWORD(delete                      , KEYCXX)
#define TOK_DO                          KEYWORD(do                          , KEYALL)
#define TOK_DOUBLE                      KEYWORD(double                      , KEYALL)
#define TOK_DYNAMIC_CAST                KEYWORD(dynamic_cast                , KEYCXX)
#define TOK_ELSE                        KEYWORD(else                        , KEYALL)
#define TOK_ENUM                        KEYWORD(enum                        , KEYALL)
#define TOK_EXPLICIT                    KEYWORD(explicit                    , KEYCXX)
#define TOK_EXPORT                      KEYWORD(export                      , KEYCXX)
#define TOK_EXTERN                      KEYWORD(extern                      , KEYALL)
#define TOK_FALSE                       KEYWORD(false                       , KEYCXX)
#define TOK_FLOAT                       KEYWORD(float                       , KEYALL)
#define TOK_FOR                         KEYWORD(for                         , KEYALL)
#define TOK_FRIEND                      KEYWORD(friend                      , KEYCXX)
#define TOK_GOTO                        KEYWORD(goto                        , KEYALL)
#define TOK_IF                          KEYWORD(if                          , KEYALL)
#define TOK_INLINE                      KEYWORD(inline                      , KEYC99|KEYCXX|KEYGNU)
#define TOK_INT                         KEYWORD(int                         , KEYALL)
#define TOK_LONG                        KEYWORD(long                        , KEYALL)
#define TOK_MUTABLE                     KEYWORD(mutable                     , KEYCXX)
#define TOK_NAMESPACE                   KEYWORD(namespace                   , KEYCXX)
#define TOK_NEW                         KEYWORD(new                         , KEYCXX)
#define TOK_OPERATOR                    KEYWORD(operator                    , KEYCXX)
#define TOK_PASCAL                      TOK(unknown)
#define TOK_PRIVATE                     KEYWORD(private                     , KEYCXX)
#define TOK_PROTECTED                   KEYWORD(protected                   , KEYCXX)
#define TOK_PUBLIC                      KEYWORD(public                      , KEYCXX)
#define TOK_REGISTER                    KEYWORD(register                    , KEYALL)
#define TOK_REINTERPRET_CAST            KEYWORD(reinterpret_cast            , KEYCXX)
#define TOK_RETURN                      KEYWORD(return                      , KEYALL)
#define TOK_SHORT                       KEYWORD(short                       , KEYALL)
#define TOK_SIGNED                      KEYWORD(signed                      , KEYALL)
#define TOK_SIZEOF                      KEYWORD(sizeof                      , KEYALL)
#define TOK_STATIC                      KEYWORD(static                      , KEYALL)
#define TOK_STATIC_CAST                 KEYWORD(static_cast                 , KEYCXX)
#define TOK_STRUCT                      KEYWORD(struct                      , KEYALL)
#define TOK_SWITCH                      KEYWORD(switch                      , KEYALL)
#define TOK_TEMPLATE                    KEYWORD(template                    , KEYCXX)
#define TOK_THIS                        KEYWORD(this                        , KEYCXX)
#define TOK_THROW                       KEYWORD(throw                       , KEYCXX)
#define TOK_TRUE                        KEYWORD(true                        , KEYCXX)
#define TOK_TRY                         KEYWORD(try                         , KEYCXX)
#define TOK_TYPEDEF                     KEYWORD(typedef                     , KEYALL)
#define TOK_TYPEID                      KEYWORD(typeid                      , KEYCXX)
#define TOK_TYPENAME                    KEYWORD(typename                    , KEYCXX)
#define TOK_UNION                       KEYWORD(union                       , KEYALL)
#define TOK_UNSIGNED                    KEYWORD(unsigned                    , KEYALL)
#define TOK_USING                       KEYWORD(using                       , KEYCXX)
#define TOK_VIRTUAL                     KEYWORD(virtual                     , KEYCXX)
#define TOK_VOID                        KEYWORD(void                        , KEYALL)
#define TOK_VOLATILE                    KEYWORD(volatile                    , KEYALL)
#define TOK_WCHAR_T                     KEYWORD(wchar_t                     , KEYCXX)
#define TOK_WHILE                       KEYWORD(while                       , KEYALL)

  // operators (I don't identify C++ operators because in C they're not identifiers)
#define TOK_LPAREN                      TOK(l_paren)             // (
#define TOK_RPAREN                      TOK(r_paren)             // )
#define TOK_LBRACKET                    TOK(l_square)            // [
#define TOK_RBRACKET                    TOK(r_square)            // ]
#define TOK_ARROW                       TOK(arrow)               // ->
#define TOK_COLONCOLON                  TOK(coloncolon)          // ::
#define TOK_DOT                         TOK(period)              // .
#define TOK_BANG                        TOK(exclaim)             // !
#define TOK_TILDE                       TOK(tilde)               // ~
#define TOK_PLUS                        TOK(plus)                // +
#define TOK_MINUS                       TOK(minus)               // -
#define TOK_PLUSPLUS                    TOK(plusplus)            // ++
#define TOK_MINUSMINUS                  TOK(minusminus)          // --
#define TOK_AND                         TOK(amp)                 // &
#define TOK_STAR                        TOK(star)                // *
#define TOK_DOTSTAR                     TOK(periodstar)          // .*
#define TOK_ARROWSTAR                   TOK(arrowstar)           // ->*
#define TOK_SLASH                       TOK(slash)               // /
#define TOK_PERCENT                     TOK(percent)             // %
#define TOK_LEFTSHIFT                   TOK(lessless)            // <<
#define TOK_RIGHTSHIFT                  TOK(greatergreater)      // >>
#define TOK_LESSTHAN                    TOK(less)                // <
#define TOK_LESSEQ                      TOK(lessequal)           // <=
#define TOK_GREATERTHAN                 TOK(greater)             // >
#define TOK_GREATEREQ                   TOK(greaterequal)        // >=
#define TOK_EQUALEQUAL                  TOK(equalequal)          // ==
#define TOK_NOTEQUAL                    TOK(exclaimequal)        // !=
#define TOK_XOR                         TOK(caret)               // ^
#define TOK_OR                          TOK(pipe)                // |
#define TOK_ANDAND                      TOK(ampamp)              // &&
#define TOK_OROR                        TOK(pipepipe)            // ||
#define TOK_QUESTION                    TOK(question)            // ?
#define TOK_COLON                       TOK(colon)               // :
#define TOK_EQUAL                       TOK(equal)               // =
#define TOK_STAREQUAL                   TOK(starequal)           // *=
#define TOK_SLASHEQUAL                  TOK(slashequal)          // /=
#define TOK_PERCENTEQUAL                TOK(percentequal)        // %=
#define TOK_PLUSEQUAL                   TOK(plusequal)           // +=
#define TOK_MINUSEQUAL                  TOK(minusequal)          // -=
#define TOK_ANDEQUAL                    TOK(ampequal)            // &=
#define TOK_XOREQUAL                    TOK(caretequal)          // ^=
#define TOK_OREQUAL                     TOK(pipeequal)           // |=
#define TOK_LEFTSHIFTEQUAL              TOK(lesslessequal)       // <<=
#define TOK_RIGHTSHIFTEQUAL             TOK(greatergreaterequal) // >>=
#define TOK_COMMA                       TOK(comma)               // ,
#define TOK_ELLIPSIS                    TOK(ellipsis)            // ...
#define TOK_SEMICOLON                   TOK(semi)                // ;
#define TOK_LBRACE                      TOK(l_brace)             // {
#define TOK_RBRACE                      TOK(l_brace)             // {

  // dummy terminals used for precedence games
#define TOK_PREFER_REDUCE               TOK(prefer_reduce)
#define TOK_PREFER_SHIFT                TOK(prefer_shift)

  // GNU extensions
#define TOK_BUILTIN_CONSTANT_P          KEYWORD(__builtin_constant_p        , KEYALL)
#define TOK___ALIGNOF__                 KEYWORD(__alignof                   , KEYALL)
#define TOK___OFFSETOF__                KEYWORD(__offsetof                  , KEYALL)
#define TOK___BUILTIN_OFFSETOF          KEYWORD(__builtin_offsetof          , KEYALL)
#define TOK___ATTRIBUTE__               KEYWORD(__attribute                 , KEYALL)
#define TOK___FUNCTION__                KEYWORD(__FUNCTION__                , KEYALL)
#define TOK___LABEL__                   KEYWORD(__label__                   , KEYALL)
#define TOK___PRETTY_FUNCTION__         KEYWORD(__PRETTY_FUNCTION__         , KEYALL)
#define TOK___TYPEOF__                  KEYWORD(typeof                      , KEYGNU)
#define TOK___EXTENSION__               KEYWORD(__extension__               , KEYALL)
#define TOK___BUILTIN_EXPECT            KEYWORD(__builtin_expect            , KEYALL)

  // varargs; dsw: I think that we should make all of these their own
  // AST node, I just don't want to deal with the parsing ambiguity
  // with E_funCall right now
  // rdp: I'll give it a try.
#define TOK___BUILTIN_VA_START          KEYWORD(__builtin_va_start          , KEYALL)
#define TOK___BUILTIN_VA_COPY           KEYWORD(__builtin_va_copy           , KEYALL)
#define TOK___BUILTIN_VA_ARG            KEYWORD(__builtin_va_arg            , KEYALL)
#define TOK___BUILTIN_VA_END            KEYWORD(__builtin_va_end            , KEYALL)
#define TOK___BUILTIN_ALLOCA            KEYWORD(__builtin_alloca            , KEYALL)

#define TOK_MIN_OP                      TOK(min)                 // <?
#define TOK_MAX_OP                      TOK(max)                 // >?
#define TOK_REAL                        KEYWORD(__real                      , KEYALL)
#define TOK_IMAG                        KEYWORD(__imag                      , KEYALL)

  // sm: these are C99 but I'm putting them here anyway..
#define TOK_RESTRICT                    KEYWORD(restrict                    , KEYC99|KEYGNU)
#define TOK_COMPLEX                     KEYWORD(_Complex                    , KEYALL)
#define TOK_IMAGINARY                   KEYWORD(_Imaginary                  , KEYALL)
#define NUM_TOKEN_TYPES ellcc::tok::NUM_TOKENS

// map TokenType to its spelling or description
extern char const * const tokenNameTable[];
extern int const tokenNameTableSize;

// map TokenType to a bitwise OR of TokenFlags
extern unsigned char tokenFlagTable[];

#endif // CC_TOKENS_H
