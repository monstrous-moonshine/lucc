#include "decl.hpp"
#include "expr.hpp"
#include "stmt.hpp"
#include "parse.hpp"
#include <cstdio>
#include <functional>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#define ARRAY_LEN(a) (sizeof a / sizeof a[0])
#define consume(expected_type, msg) ({ \
    if (prev.type != expected_type) {  \
        fprintf(stderr, msg);          \
        return NULL;                   \
    }                                  \
    advance();                         \
})

std::unique_ptr<DeclAST> Parser::parse_decl() {
    return parse_decl(false);
}

std::unique_ptr<DeclAST> Parser::parse_decl(bool is_param) {
    Token type;
    switch (prev.type) {
    case TOK_T_VOID:
    case TOK_T_CHAR:
    case TOK_T_SHORT:
    case TOK_T_INT:
    case TOK_T_LONG:
    case TOK_T_FLOAT:
    case TOK_T_DOUBLE:
    case TOK_T_SIGNED:
    case TOK_T_UNSIGNED:
        type = prev;
        advance();
        break;
    default:
        fprintf(stderr, "Expect type specifier\n");
        return NULL;
    }
    auto decl = parse_declarator();
    if (!decl) return NULL;
    if (is_param) return std::make_unique<DeclAST>(
            true, type, std::move(decl), nullptr);
    if (prev.type == TOK_SEMICOLON) {
        advance();
        return std::make_unique<DeclAST>(false, type, std::move(decl), nullptr);
    } else if (prev.type == TOK_LBRACE) {
        advance();
        auto body = block_stmt();
        if (!body) return NULL;
        return std::make_unique<DeclAST>(false, type,
                                         std::move(decl),
                                         std::move(body));
    } else {
        fprintf(stderr, "Expect ';' or '{'\n");
        return NULL;
    }
}

std::unique_ptr<Decl> Parser::parse_declarator() {
    int ptr_level = 0;
    while (prev.type == TOK_STAR) {
        advance();
        ptr_level++;
    }
    auto decl = parse_direct_declarator();
    if (!decl) return NULL;
    return std::make_unique<Decl>(ptr_level, std::move(decl));
}

std::unique_ptr<DirectDecl> Parser::parse_direct_declarator() {
    std::unique_ptr<DirectDecl> decl;
    if (prev.type == TOK_IDENT) {
        decl = std::make_unique<VarDecl>(std::move(prev.lexeme));
        advance();
    } else if (prev.type == TOK_LPAREN) {
        advance();
        decl = parse_declarator();
        consume(TOK_RPAREN, "Expect ')'\n");
    } else {
        fprintf(stderr, "Expect identifier or '('\n");
        return NULL;
    }
    if (prev.type == TOK_LBRACKET) {
        advance();
        if (prev.type == TOK_RBRACKET) {
            advance();
            return std::make_unique<ArrayDecl>(std::move(decl), nullptr);
        } else {
            auto e = parse_expr(2);
            consume(TOK_RBRACKET, "Expect ']'\n");
            return std::make_unique<ArrayDecl>(std::move(decl), std::move(e));
        }
    } else if (prev.type == TOK_LPAREN) {
        advance();
        if (prev.type == TOK_RPAREN) {
            advance();
            return std::make_unique<FuncDecl>(std::move(decl), nullptr);
        } else {
            std::unique_ptr<std::vector<std::unique_ptr<DeclAST>>> params(
                    new std::vector<std::unique_ptr<DeclAST>>);
            auto param_decl = parse_decl(true);
            if (!param_decl) return NULL;
            params->emplace_back(std::move(param_decl));
            while (prev.type == TOK_COMMA) {
                advance();
                auto param_decl = parse_decl(true);
                if (!param_decl) return NULL;
                params->emplace_back(std::move(param_decl));
            }
            consume(TOK_RPAREN, "Expect ')'\n");
            return std::make_unique<FuncDecl>(std::move(decl), std::move(params));
        }
    } else {
        return decl;
    }
}

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
        return NULL;
    case TOK_K_RETURN:
        advance();
        return return_stmt();
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
    std::unique_ptr<std::vector<std::unique_ptr<StmtAST>>> stmts(
            new std::vector<std::unique_ptr<StmtAST>>);
    while (prev.type != TOK_RBRACE) {
        auto stmt = parse_stmt();
        if (!stmt) return NULL;
        stmts->emplace_back(std::move(stmt));
    }
    advance();  // '}'
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
        advance();
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

std::unique_ptr<StmtAST> Parser::return_stmt() {
    if (prev.type == TOK_SEMICOLON) {
        advance();
        return std::make_unique<ReturnStmtAST>(nullptr);
    }
    auto e = parse_expr(0);
    consume(TOK_SEMICOLON, "Expect ';'\n");
    return std::make_unique<ReturnStmtAST>(std::move(e));
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

std::unique_ptr<ExprAST> Parser::variable() {
    auto name = prev.lexeme;
    advance();
    return std::make_unique<VarExprAST>(std::move(name));
}

std::unique_ptr<ExprAST> Parser::number() {
    double v = std::stod(prev.lexeme);
    advance();
    return std::make_unique<NumberExprAST>(v);
}

std::unique_ptr<ExprAST> Parser::grouping() {
    advance();  // '('
    auto e = parse_expr(0);
    if (!e) return NULL;
    consume(TOK_RPAREN, "Expect ')'\n");
    return e;
}

std::unique_ptr<ExprAST> Parser::index(std::unique_ptr<ExprAST> e) {
    advance();  // '['
    auto i = parse_expr(0);
    if (!i) return NULL;
    consume(TOK_RBRACKET, "Expect ']'\n");
    return std::make_unique<IndexExprAst>(std::move(e), std::move(i));
}

std::unique_ptr<ExprAST> Parser::call(std::unique_ptr<ExprAST> e) {
    advance();  // '('
    if (prev.type == TOK_RPAREN) {
        advance();
        return std::make_unique<CallExprAST>(std::move(e), nullptr);
    }
    std::unique_ptr<std::vector<std::unique_ptr<ExprAST>>> args(
            new std::vector<std::unique_ptr<ExprAST>>);
    auto a = parse_expr(1);  // PREC_COMMA
    if (!a) return NULL;
    args->emplace_back(std::move(a));
    while (prev.type == TOK_COMMA) {
        advance();
        auto a = parse_expr(1);  // PREC_COMMA
        if (!a) return NULL;
        args->emplace_back(std::move(a));
    }
    consume(TOK_RPAREN, "Expect ')'\n");
    return std::make_unique<CallExprAST>(std::move(e), std::move(args));
}

std::unique_ptr<ExprAST> Parser::unary() {
    Token token = prev;
    advance();
    auto e = parse_expr(13);
    if (!e) return NULL;
    return std::make_unique<UnaryExprAST>(token, std::move(e));
}

std::unique_ptr<ExprAST> Parser::binary(std::unique_ptr<ExprAST> e) {
    /* NOTE:
     * 1. Assignment is right associative.
     * 2. LHS of assignment must be unary expression.
     *
     * We handle point 1 by passing in a lower minimum precedence for the RHS
     * when parsing an assignment. This will ensure that an assignment operator
     * to the right of the current one will be parsed as part of the RHS. We
     * will check for point 2 in the semantic analysis phase.
     */
    Token token = prev;
    int   prec = get_expr_precedence() - (prev.type == TOK_ASSIGN ? 1 : 0);
    advance();
    auto f = parse_expr(prec);
    if (!f) return NULL;
    return std::make_unique<BinaryExprAST>(token, std::move(e), std::move(f));
}

std::unique_ptr<ExprAST> Parser::ternary(std::unique_ptr<ExprAST> e) {
    /* NOTE:
     * 1. @then_arm of conditional is parsed as if parenthesized. Precedence
     *    of '?' is not used.
     * 2. @else_arm of conditional must be another conditional. Here, prece-
     *    dence of '?' is used. Since the ternary operator is right-associa-
     *    tive, we pass in 1 less than the precedence of '?' here.
     */
    advance();  // '?'
    auto then_expr = parse_expr(0);
    if (!then_expr) return NULL;
    consume(TOK_COLON, "Expect ':'\n");
    auto else_expr = parse_expr(2);
    if (!else_expr) return NULL;
    return std::make_unique<TernaryExprAST>(std::move(e),
                                            std::move(then_expr),
                                            std::move(else_expr));
}

const Parser::ExprRule *Parser::get_expr_rule(TokenType type) {
    return type < ARRAY_LEN(expr_rules) ? &expr_rules[type] : NULL;
}
