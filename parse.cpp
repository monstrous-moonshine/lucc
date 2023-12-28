#include "parse.hpp"
#include <functional>
#include <memory>

#define ARRAY_LEN(a) (sizeof a / sizeof a[0])
#define consume(expected_type, msg) ({ \
    if (prev.type != expected_type) {  \
        fprintf(stderr, msg);          \
        return NULL;                   \
    }                                  \
    advance();                         \
})


std::unique_ptr<StmtAST> Parser::parse_stmt() {
retry:
    if (prev.type == TOK_ERR) {
        fprintf(stderr, "Unrecognized token\n");
        return NULL;
    }
    if (prev.type == TOK_EOF) {
        fprintf(stderr, "End of file reached\n");
        return NULL;
    }
    switch (prev.type) {
    case TOK_K_IF:
        advance();
        return if_stmt();
    case TOK_K_ELSE:
        fprintf(stderr, "Invalid token 'else'\n");
        return NULL;
    case TOK_K_SWITCH:
    case TOK_K_FOR:
        return NULL;
    case TOK_K_WHILE:
        advance();
        return while_stmt();
    case TOK_K_DO:
    case TOK_K_RETURN:
        return NULL;
    case TOK_LBRACE:
        advance();
        return block_stmt();
    case TOK_RBRACE:
        fprintf(stderr, "Invalid token '}'\n");
        return NULL;
    case TOK_SEMICOLON:
        advance();
        goto retry;
    default:
        {
            auto e = parse_expr(0);
            consume(TOK_SEMICOLON, "Expect ';'\n");
            return std::make_unique<ExprStmtAST>(std::move(e));
        }
    }
}

std::unique_ptr<StmtAST> Parser::block_stmt() {
    std::unique_ptr<std::vector<std::unique_ptr<StmtAST>>> stmts;
    while (prev.type != TOK_RBRACE) {
        auto stmt = parse_stmt();
        if (!stmt) return NULL;
        stmts->emplace_back(std::move(stmt));
    }
    advance();  // TOK_RBRACE
    return std::make_unique<BlockStmtAST>(std::move(stmts));
}

std::unique_ptr<StmtAST> Parser::if_stmt() {
    consume(TOK_LPAREN, "Expect '('\n");
    auto cond = parse_expr(0);
    if (!cond) return NULL;
    consume(TOK_RPAREN, "Expect ')'\n");
    auto then_arm = parse_stmt();
    if (!then_arm) return NULL;
    std::unique_ptr<StmtAST> else_arm(nullptr);
    if (prev.type == TOK_K_ELSE) {
        else_arm = parse_stmt();
        if (!else_arm) return NULL;
    }
    return std::make_unique<IfStmtAST>(std::move(cond),
                                       std::move(then_arm),
                                       std::move(else_arm));
}

std::unique_ptr<StmtAST> Parser::while_stmt() {
    consume(TOK_LPAREN, "Expect '('\n");
    auto cond = parse_expr(0);
    if (!cond) return NULL;
    consume(TOK_RPAREN, "Expect ')'\n");
    auto body = parse_stmt();
    if (!body) return NULL;
    return std::make_unique<WhileStmtAST>(std::move(cond), std::move(body));
}

std::unique_ptr<ExprAST> Parser::parse_expr(int prec) {
    auto rule = get_expr_rule(prev.type);
    auto prefix_fn = rule ? rule->prefix : NULL;
    if (!prefix_fn) {
        fprintf(stderr, "Expect expression\n");
        return NULL;
    }
    auto e = std::invoke(prefix_fn, *this);
    if (!e) return NULL;

    while (prec < get_expr_precedence()) {
        auto infix_fn = get_expr_rule(prev.type)->infix;
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
    auto e = parse_expr(0);
    if (!e) return NULL;
    consume(TOK_RPAREN, "Expect ')'\n");
    return e;
}

std::unique_ptr<ExprAST> Parser::binary(std::unique_ptr<ExprAST> e) {
    Token token = prev;
    int   prec = get_expr_precedence();
    advance();
    auto f = parse_expr(prec);
    if (!f) return NULL;
    return std::make_unique<BinaryExprAST>(token, std::move(e), std::move(f));
}

const Parser::ExprRule *Parser::get_expr_rule(TokenType type) {
    return type < ARRAY_LEN(expr_rules) ? &expr_rules[type] : NULL;
}
