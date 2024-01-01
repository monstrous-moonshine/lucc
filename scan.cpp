#include "scan.hpp"
#include <cctype>
#include <unordered_map>
#include <utility>

#define ARRAY_LEN(a) (sizeof a / sizeof a[0])
#define CUR_LEX std::string(beg, end)
#define TOK_CASE1(c, t) case c: return {t, CUR_LEX};
#define TOK_CASE2(c, t, n1, t1)   \
    case c:                       \
        if (match(n1)) {          \
            return {t1, CUR_LEX}; \
        } else {                  \
            return {t, CUR_LEX};  \
        }
#define TOK_CASE3(c, t, n1, t1, n2, t2) \
    case c:                             \
        if (match(n1)) {                \
            return {t1, CUR_LEX};       \
        } else if (match(n2)) {         \
            return {t2, CUR_LEX};       \
        } else {                        \
            return {t, CUR_LEX};        \
        }

namespace
{

inline bool isalpha_(char c) {
    return isalpha(c) || c == '_';
}

const std::unordered_map<std::string, TokenType> keywords = {
    {"sizeof", TOK_SIZEOF},
    {"case", TOK_K_CASE},
    {"default", TOK_K_DEFAULT},
    {"if", TOK_K_IF},
    {"else", TOK_K_ELSE},
    {"switch", TOK_K_SWITCH},
    {"for", TOK_K_FOR},
    {"while", TOK_K_WHILE},
    {"do", TOK_K_DO},
    {"goto", TOK_K_GOTO},
    {"continue", TOK_K_CONTINUE},
    {"break", TOK_K_BREAK},
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
    {"struct", TOK_T_STRUCT},
    {"union", TOK_T_UNION},
    {"enum", TOK_T_ENUM},
    {"auto", TOK_T_AUTO},
    {"register", TOK_T_REGISTER},
    {"static", TOK_T_STATIC},
    {"extern", TOK_T_EXTERN},
    {"typedef", TOK_T_TYPEDEF},
    {"const", TOK_T_CONST},
    {"volatile", TOK_T_VOLATILE},
};

}  // namespace

Token Scanner::scan() {
    skip_whitespace();
    char c = advance();
    if (isalpha_(c)) return tok_ident();
    else if (isdigit(c)) return tok_number();
    switch (c) {
    case '"': return tok_string();
    TOK_CASE1('(', TOK_LPAREN)
    TOK_CASE1(')', TOK_RPAREN)
    TOK_CASE1('[', TOK_LBRACKET)
    TOK_CASE1(']', TOK_RBRACKET)
    TOK_CASE1('~', TOK_TILDE)
    TOK_CASE1('*', TOK_STAR)
    TOK_CASE1('/', TOK_SLASH)
    TOK_CASE1('%', TOK_MOD)
    TOK_CASE2('+', TOK_PLUS,  '+', TOK_INCR)
    TOK_CASE2('-', TOK_MINUS, '-', TOK_DECR)
    TOK_CASE3('<', TOK_LT, '=', TOK_LE, '<', TOK_LSHIFT)
    TOK_CASE3('>', TOK_GT, '=', TOK_GE, '>', TOK_RSHIFT)
    TOK_CASE1('^', TOK_XOR)
    TOK_CASE2('&', TOK_AND, '&', TOK_AND_AND)
    TOK_CASE2('|', TOK_OR,  '|', TOK_OR_OR)
    TOK_CASE1('?', TOK_QUERY)
    TOK_CASE1(':', TOK_COLON)
    TOK_CASE2('!', TOK_BANG,   '=', TOK_NE)
    TOK_CASE2('=', TOK_ASSIGN, '=', TOK_EQ)
    TOK_CASE1(',', TOK_COMMA)
    TOK_CASE1('{', TOK_LBRACE)
    TOK_CASE1('}', TOK_RBRACE)
    TOK_CASE1(';', TOK_SEMICOLON)
    case '.': return tok_ellipsis();
    case '\0': return {TOK_EOF, "?EOF"};
    default: return {TOK_ERR, CUR_LEX};
    }
}

Token Scanner::tok_ident() {
    while (isalpha_(peek()) || isdigit(peek()))
        advance();
    const auto lexeme = std::string(beg, end);
    auto it = keywords.find(lexeme);
    if (it == keywords.end())
        return {TOK_IDENT, lexeme};
    else
        return {it->second, lexeme};
}

Token Scanner::tok_number() {
    while (isdigit(peek()))
        advance();
    return {TOK_INT_CONST, std::string(beg, end)};
}

Token Scanner::tok_string() {
    while (peek() != '\0' && peek() != '"')
        advance();
    if (peek() == '\0') return {TOK_ERR, "?UNTERMINATED_STRING"};
    advance();
    return {TOK_STRING, std::string(beg + 1, end - 1)};
}

Token Scanner::tok_ellipsis() {
    const char *p = end;
    if (*p == '.') {
        p++;
        if (*p == '.') {
            p++;
            end = p;
            return {TOK_ELLIPSIS, CUR_LEX};
        } else {
            return {TOK_ERR, ".."};
        }
    } else {
        return {TOK_ERR, "."};
    }
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
