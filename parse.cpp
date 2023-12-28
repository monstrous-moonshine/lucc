#include "parse.hpp"
#include <functional>
#include <memory>

std::unique_ptr<ExprAST> Parser::parse(int prec) {
    // XXX: make sure array bound is not exceeded
    auto prefix_fn = rules[prev.type].prefix;
    if (!prefix_fn) {
        fprintf(stderr, "Expect expression\n");
        return NULL;
    }
    auto e = std::invoke(prefix_fn, *this);
    if (!e) return NULL;

    while (prec < get_precedence()) {
        // XXX: make sure array bound is not exceeded
        auto infix_fn = rules[prev.type].infix;
        e = std::invoke(infix_fn, *this, std::move(e));
    }
    return e;
}

std::unique_ptr<ExprAST> Parser::number() {
    double v = std::stod(prev.lexeme);
    advance();
    return std::make_unique<NumberExprAST>(v);
}

std::unique_ptr<ExprAST> Parser::grouping() {
    advance();
    auto e = parse(0);
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
    int   prec = get_precedence();
    advance();
    auto f = parse(prec);
    if (!f) return NULL;
    return std::make_unique<BinaryExprAST>(token, std::move(e), std::move(f));
}

int Parser::get_precedence() {
    // XXX: make sure array bound is not exceeded
    auto infix_rule = &rules[prev.type];
    return infix_rule->infix ? infix_rule->prec : 0;
}
