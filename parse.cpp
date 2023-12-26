#include "parse.hpp"
#include <functional>
#include <memory>

std::unique_ptr<ExprAST> Parser::parse() {
    // XXX: make sure array bound is not exceeded
    auto prefix_fn = rules[prev.type].prefix;
    if (!prefix_fn) {
        fprintf(stderr, "Expect expression\n");
        return NULL;
    }
    auto e = std::invoke(prefix_fn, *this);
    if (!e) return NULL;
    // XXX: make sure array bound is not exceeded
    auto infix_fn = rules[prev.type].infix;
    if (!infix_fn) {
        return e;
    }
    return std::invoke(infix_fn, *this, std::move(e));
}

std::unique_ptr<ExprAST> Parser::number() {
    double v = std::stod(prev.lexeme);
    advance();
    return std::make_unique<NumberExprAST>(v);
}

std::unique_ptr<ExprAST> Parser::grouping() {
    advance();
    auto e = parse();
    if (!e) return NULL;
    if (prev.type != TOK_RPAREN) {
        fprintf(stderr, "Expect ')'\n");
        return NULL;
    }
    advance();
    return e;
}

std::unique_ptr<ExprAST> Parser::binary(std::unique_ptr<ExprAST> e) {
    Token token = prev;
    advance();
    auto f = parse();
    if (!f) return NULL;
    return std::make_unique<BinaryExprAST>(token, std::move(e), std::move(f));
}
