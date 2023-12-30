#include "scan.hpp"
#include <cctype>

#define ARRAY_LEN(a) (sizeof a / sizeof a[0])
#define TOK_CASE1(c, t, l) case c: return {t, l};
#define TOK_CASE2(c, n, t1, l1, t2, l2) \
    case c:                             \
        if (match(n)) {                 \
            return {t1, l1};            \
        } else {                        \
            return {t2, l2};            \
        }
#define TOK_CASE3(c, n1, t1, l1, n2, t2, l2, t3, l3) \
    case c:                                          \
        if (match(n1)) {                             \
            return {t1, l1};                         \
        } else if (match(n2)) {                      \
            return {t2, l2};                         \
        } else {                                     \
            return {t3, l3};                         \
        }

namespace
{

inline bool isalpha_(char c) {
    return isalpha(c) || c == '_';
}

struct {
    std::string lexeme;
    TokenType type;
} keywords[] = {
    {"if", TOK_K_IF},
    {"else", TOK_K_ELSE},
    {"switch", TOK_K_SWITCH},
    {"for", TOK_K_FOR},
    {"while", TOK_K_WHILE},
    {"do", TOK_K_DO},
    {"return", TOK_K_RETURN},
    {"void", TOK_T_VOID},
    {"char", TOK_T_CHAR},
    {"short", TOK_T_SHORT},
    {"int", TOK_T_INT},
    {"long", TOK_T_LONG},
    {"float", TOK_T_FLOAT},
    {"double", TOK_T_DOUBLE},
    {"signed", TOK_T_SIGNED},
    {"unsigned", TOK_T_UNSIGNED},
};

}  // namespace

Token Scanner::scan() {
    skip_whitespace();
    char c = advance();
    if (isalpha_(c)) return tok_ident();
    else if (isdigit(c)) return tok_number();
    switch (c) {
    case '"': return tok_string();
    TOK_CASE1('(', TOK_LPAREN, "(")
    TOK_CASE1(')', TOK_RPAREN, ")")
    TOK_CASE1('[', TOK_LBRACKET, "[")
    TOK_CASE1(']', TOK_RBRACKET, "]")
    TOK_CASE2('+', '+', TOK_INCR, "++", TOK_PLUS, "+")
    TOK_CASE2('-', '-', TOK_DECR, "--", TOK_MINUS, "-")
    TOK_CASE1('~', TOK_TILDE, "~")
    TOK_CASE1('*', TOK_STAR, "*")
    TOK_CASE1('/', TOK_SLASH, "/")
    TOK_CASE1('%', TOK_MOD, "%")
    TOK_CASE3('<', '=', TOK_LE, "<=", '<', TOK_LSHIFT, "<<", TOK_LT, "<")
    TOK_CASE3('>', '=', TOK_GE, ">=", '>', TOK_RSHIFT, ">>", TOK_GT, ">")
    TOK_CASE2('=', '=', TOK_EQ, "==", TOK_ASSIGN, "=")
    TOK_CASE2('!', '=', TOK_NE, "!=", TOK_BANG, "!")
    TOK_CASE2('&', '&', TOK_AND_AND, "&&", TOK_AND, "&")
    TOK_CASE1('^', TOK_XOR, "^")
    TOK_CASE2('|', '|', TOK_OR_OR, "||", TOK_OR, "|")
    TOK_CASE1('?', TOK_QUERY, "?")
    TOK_CASE1(':', TOK_COLON, ":")
    TOK_CASE1(',', TOK_COMMA, ",")
    TOK_CASE1('{', TOK_LBRACE, "{")
    TOK_CASE1('}', TOK_RBRACE, "}")
    TOK_CASE1(';', TOK_SEMICOLON, ";")
    TOK_CASE1('\0', TOK_EOF, "")
    default:
        return {TOK_ERR, ""};
    }
}

Token Scanner::tok_ident() {
    while (isalpha_(peek()) || isdigit(peek()))
        advance();
    const auto lexeme = std::string(beg, end);
    for (unsigned i = 0; i < ARRAY_LEN(keywords); i++) {
        if (lexeme == keywords[i].lexeme)
            return {keywords[i].type, lexeme};
    }
    return {TOK_IDENT, lexeme};
}

Token Scanner::tok_number() {
    while (isdigit(peek()))
        advance();
    return {TOK_INT_CONST, std::string(beg, end)};
}

Token Scanner::tok_string() {
    while (peek() != '\0' && peek() != '"')
        advance();
    if (peek() == '\0') return {TOK_ERR, ""};
    advance();
    return {TOK_STRING, std::string(beg + 1, end - 1)};
}

void Scanner::skip_whitespace() {
    while (peek() != '\0' && isspace(peek()))
        advance();
    beg = end;
}

char Scanner::advance() {
    char c = *end;
    if (c) end++;
    return c;
}

// DO NOT match('\0')
bool Scanner::match(char c) {
    if (peek() == c) {
        advance();
        return true;
    }
    return false;
}
