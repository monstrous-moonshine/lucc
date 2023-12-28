#ifndef SCAN_HPP
#define SCAN_HPP
#include <string>

enum TokenType {
    TOK_IDENT,
    TOK_INT_CONST,
    TOK_STRING,
    TOK_LPAREN,
    TOK_RPAREN,
    TOK_LBRACKET,
    TOK_RBRACKET,
    TOK_INCR,
    TOK_DECR,
    TOK_TILDE,
    TOK_BANG,
    TOK_PLUS,
    TOK_MINUS,
    TOK_STAR,
    TOK_SLASH,
    TOK_MOD,
    TOK_LSHIFT,
    TOK_RSHIFT,
    TOK_EQ,
    TOK_NE,
    TOK_LT,
    TOK_GT,
    TOK_LE,
    TOK_GE,
    TOK_AND,
    TOK_XOR,
    TOK_OR,
    TOK_AND_AND,
    TOK_OR_OR,
    TOK_QUERY,
    TOK_COLON,
    TOK_ASSIGN,
    TOK_COMMA,
    TOK_K_IF,
    TOK_K_SWITCH,
    TOK_K_FOR,
    TOK_K_WHILE,
    TOK_K_DO,
    TOK_K_RETURN,
    TOK_LBRACE,
    TOK_RBRACE,
    TOK_SEMICOLON,
    TOK_EOF,
    TOK_ERR,
};

struct Token {
    TokenType type;
    std::string lexeme;
};

class Scanner {
public:
    Scanner(const char *src) : beg(src), end(src) {}
    Token scan();
private:
    void skip_whitespace();
    char advance();
    char peek();
    Token tok_ident();
    Token tok_number();
    Token tok_string();
    const char *beg, *end;
};
#endif
