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

Token Parser::parse_type_spec() {
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
        return type;
    default:
        return {TOK_ERR, ""};
    }
}

std::unique_ptr<Parser::ExtDeclList> Parser::parse_translation_unit() {
    auto decls = std::make_unique<ExtDeclList>();
    while (prev.type != TOK_EOF) {
        auto decl = parse_external_decl();
        if (!decl) return NULL;
        decls->emplace_back(std::move(decl));
    }
    return decls;
}

std::unique_ptr<ExtDeclAST> Parser::parse_external_decl() {
    Token type = parse_type_spec();
    if (type.type == TOK_ERR) {
        fprintf(stderr, "Expect type specifier\n");
        return NULL;
    }
    auto decl = parse_declarator();
    if (!decl) return NULL;
    if (match(TOK_LBRACE)) {
        auto body = block_stmt();
        if (!body) return NULL;
        return std::make_unique<FuncDeclAST>(type, std::move(decl),
                                             std::move(body));
    } else {
        return parse_data_decl(type, std::move(decl));
    }
}

std::unique_ptr<DeclAST> Parser::parse_data_decl(
        Token type, std::unique_ptr<Declarator> decl) {
    std::unique_ptr<std::vector<InitDecl>> init_decls(
            new std::vector<InitDecl>);
    for (;;) {
        std::unique_ptr<ExprAST> init;
        if (match(TOK_ASSIGN)) {
            init = parse_expr(1);
            if (!init) return NULL;
        }
        init_decls->emplace_back(std::move(decl), std::move(init));
        if (match(TOK_COMMA)) {
            decl = parse_declarator();
            if (!decl) return NULL;
        } else if (match(TOK_SEMICOLON)) {
            break;
        } else {
            fprintf(stderr, "Expect ',' or ';'\n");
            return NULL;
        }
    }
    return std::make_unique<DeclAST>(type, std::move(init_decls));
}

std::unique_ptr<ParamDeclAST> Parser::parse_param_decl() {
    Token type = parse_type_spec();
    if (type.type == TOK_ERR) {
        fprintf(stderr, "Expect type specifier\n");
        return NULL;
    }
    if (prev.type == TOK_COMMA || prev.type == TOK_RPAREN)
        return std::make_unique<ParamDeclAST>(type, nullptr);
    auto decl = parse_declarator();
    if (!decl) return NULL;
    return std::make_unique<ParamDeclAST>(type, std::move(decl));
}

std::unique_ptr<Declarator> Parser::parse_declarator() {
    int ptr_level = 0;
    while (match(TOK_STAR)) {
        ptr_level++;
    }
    auto decl = parse_direct_declarator();
    if (!decl) return NULL;
    return std::make_unique<Declarator>(ptr_level, std::move(decl));
}

std::unique_ptr<DirectDecl> Parser::parse_direct_declarator() {
    std::unique_ptr<DirectDecl> decl;
    if (prev.type == TOK_IDENT) {
        decl = std::make_unique<VarDecl>(std::move(prev.lexeme));
        advance();
    } else if (match(TOK_LPAREN)) {
        decl = parse_declarator();
        if (!decl) return NULL;
        consume(TOK_RPAREN, "Expect ')'\n");
    } else {
        fprintf(stderr, "Expect identifier or '('\n");
        return NULL;
    }
    while (prev.type == TOK_LBRACKET || prev.type == TOK_LPAREN) {
        if (match(TOK_LBRACKET)) {
            decl = parse_array_decl(std::move(decl));
            if (!decl) return NULL;
        } else if (match(TOK_LPAREN)) {
            decl = parse_func_decl(std::move(decl));
            if (!decl) return NULL;
        }
    }
    return decl;
}

std::unique_ptr<DirectDecl> Parser::parse_array_decl(
        std::unique_ptr<DirectDecl> decl) {
    if (match(TOK_RBRACKET)) {
        return std::make_unique<ArrayDecl>(std::move(decl), nullptr);
    } else {
        auto e = parse_expr(2);
        if (!e) return NULL;
        consume(TOK_RBRACKET, "Expect ']'\n");
        return std::make_unique<ArrayDecl>(std::move(decl), std::move(e));
    }
}

std::unique_ptr<DirectDecl> Parser::parse_func_decl(
        std::unique_ptr<DirectDecl> decl) {
    if (match(TOK_RPAREN)) {
        return std::make_unique<FuncDecl>(false, std::move(decl), nullptr);
    } else {
        bool is_variadic = false;
        auto params = std::make_unique<FuncDecl::ParamList>();
        auto param_decl = parse_param_decl();
        if (!param_decl) return NULL;
        params->emplace_back(std::move(param_decl));
        while (match(TOK_COMMA)) {
            if (match(TOK_ELLIPSIS)) {
                is_variadic = true;
                break;
            }
            auto param_decl = parse_param_decl();
            if (!param_decl) return NULL;
            params->emplace_back(std::move(param_decl));
        }
        consume(TOK_RPAREN, "Expect ')'\n");
        return std::make_unique<FuncDecl>(is_variadic, std::move(decl),
                std::move(params));
    }
}

std::unique_ptr<StmtAST> Parser::parse_stmt() {
    StmtParser parser = get_stmt_parser(prev.type);
    if (!parser) {
        if (prev.type == TOK_IDENT) return label_stmt();
        auto e = parse_expr(0);
        if (!e) return NULL;
        consume(TOK_SEMICOLON, "Expect ';'\n");
        return std::make_unique<ExprStmtAST>(std::move(e));
    } else {
        advance();
        return std::invoke(parser, *this);
    }
}

std::unique_ptr<StmtAST> Parser::label_stmt() {
    auto label = std::make_unique<std::string>(prev.lexeme);
    advance();
    if (match(TOK_COLON)) {
        auto stmt = parse_stmt();
        if (!stmt) return NULL;
        return std::make_unique<LabelStmtAST>(LabelStmtAST::LABEL,
                std::move(label), nullptr, std::move(stmt));
    } else {
        auto e = parse_infix(0, std::make_unique<VarExprAST>(std::move(label)));
        if (!e) return NULL;
        consume(TOK_SEMICOLON, "Expect ';'\n");
        return std::make_unique<ExprStmtAST>(std::move(e));
    }
}

std::unique_ptr<StmtAST> Parser::case_stmt() {
    auto e = parse_expr(2);
    if (!e) return NULL;
    consume(TOK_COLON, "Expect ':'\n");
    auto stmt = parse_stmt();
    if (!stmt) return NULL;
    return std::make_unique<LabelStmtAST>(LabelStmtAST::CASE, nullptr,
                                          std::move(e), std::move(stmt));
}

std::unique_ptr<StmtAST> Parser::default_stmt() {
    consume(TOK_COLON, "Expect ':'\n");
    auto stmt = parse_stmt();
    if (!stmt) return NULL;
    return std::make_unique<LabelStmtAST>(LabelStmtAST::DEFAULT, nullptr,
                                          nullptr, std::move(stmt));
}

std::unique_ptr<StmtAST> Parser::block_stmt() {
    auto decls = std::make_unique<BlockStmtAST::DeclList>();
    auto stmts = std::make_unique<BlockStmtAST::StmtList>();
    while (prev.type != TOK_RBRACE) {
        Token type = parse_type_spec();
        if (type.type == TOK_ERR) break;
        auto decl = parse_declarator();
        if (!decl) return NULL;
        auto decl_ast = parse_data_decl(type, std::move(decl));
        if (!decl_ast) return NULL;
        decls->emplace_back(std::move(decl_ast));
    }
    while (prev.type != TOK_RBRACE) {
        auto stmt = parse_stmt();
        if (!stmt) return NULL;
        stmts->emplace_back(std::move(stmt));
    }
    advance();  // '}'
    return std::make_unique<BlockStmtAST>(std::move(decls), std::move(stmts));
}

std::unique_ptr<StmtAST> Parser::if_stmt() {
    consume(TOK_LPAREN, "Expect '('\n");
    auto cond = parse_expr(0);
    if (!cond) return NULL;
    consume(TOK_RPAREN, "Expect ')'\n");
    auto then_arm = parse_stmt();
    if (!then_arm) return NULL;
    std::unique_ptr<StmtAST> else_arm;
    if (match(TOK_K_ELSE)) {
        else_arm = parse_stmt();
        if (!else_arm) return NULL;
    }
    return std::make_unique<IfStmtAST>(std::move(cond), std::move(then_arm),
                                       std::move(else_arm));
}

std::unique_ptr<StmtAST> Parser::switch_stmt() {
    consume(TOK_LPAREN, "Expect '('\n");
    auto cond = parse_expr(0);
    if (!cond) return NULL;
    consume(TOK_RPAREN, "Expect ')'\n");
    auto body = parse_stmt();
    if (!body) return NULL;
    return std::make_unique<SwitchStmtAST>(std::move(cond), std::move(body));
}

std::unique_ptr<StmtAST> Parser::for_stmt() {
    consume(TOK_LPAREN, "Expect '('\n");
    std::unique_ptr<ExprAST> init;
    std::unique_ptr<ExprAST> cond;
    std::unique_ptr<ExprAST> incr;
    if (!match(TOK_SEMICOLON)) {
        init = parse_expr(0);
        if (!init) return NULL;
        consume(TOK_SEMICOLON, "Expect ';'\n");
    }
    if (!match(TOK_SEMICOLON)) {
        cond = parse_expr(0);
        if (!cond) return NULL;
        consume(TOK_SEMICOLON, "Expect ';'\n");
    }
    if (!match(TOK_RPAREN)) {
        incr = parse_expr(0);
        if (!incr) return NULL;
        consume(TOK_RPAREN, "Expect ')'\n");
    }
    auto body = parse_stmt();
    if (!body) return NULL;
    return std::make_unique<ForStmtAST>(std::move(init), std::move(cond),
                                        std::move(incr), std::move(body));
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

std::unique_ptr<StmtAST> Parser::do_stmt() {
    auto body = parse_stmt();
    if (!body) return NULL;
    consume(TOK_K_WHILE, "Expect 'while'\n");
    consume(TOK_LPAREN, "Expect '('\n");
    auto cond = parse_expr(0);
    if (!cond) return NULL;
    consume(TOK_RPAREN, "Expect ')'\n");
    consume(TOK_SEMICOLON, "Expect ';'\n");
    return std::make_unique<DoStmtAST>(std::move(cond), std::move(body));
}

std::unique_ptr<StmtAST> Parser::goto_stmt() {
    if (prev.type != TOK_IDENT) {
        fprintf(stderr, "Expect identifier\n");
        return NULL;
    }
    auto label = std::make_unique<std::string>(prev.lexeme);
    advance();
    consume(TOK_SEMICOLON, "Expect ';'\n");
    return std::make_unique<JumpStmtAST>(JumpStmtAST::GOTO, std::move(label));
}

std::unique_ptr<StmtAST> Parser::continue_stmt() {
    consume(TOK_SEMICOLON, "Expect ';'\n");
    return std::make_unique<JumpStmtAST>(JumpStmtAST::CONTINUE, nullptr);
}

std::unique_ptr<StmtAST> Parser::break_stmt() {
    consume(TOK_SEMICOLON, "Expect ';'\n");
    return std::make_unique<JumpStmtAST>(JumpStmtAST::BREAK, nullptr);
}

std::unique_ptr<StmtAST> Parser::return_stmt() {
    if (match(TOK_SEMICOLON)) {
        return std::make_unique<ReturnStmtAST>(nullptr);
    }
    auto e = parse_expr(0);
    if (!e) return NULL;
    consume(TOK_SEMICOLON, "Expect ';'\n");
    return std::make_unique<ReturnStmtAST>(std::move(e));
}

std::unique_ptr<StmtAST> Parser::empty_stmt() {
    return std::make_unique<EmptyStmtAST>();
}

Parser::StmtParser Parser::get_stmt_parser(TokenType type) {
    size_t idx = type - TOK_K_CASE;
    return idx < ARRAY_LEN(stmt_parsers) ? stmt_parsers[idx] : NULL;
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

    return parse_infix(prec, std::move(e));
}

std::unique_ptr<ExprAST> Parser::parse_infix(
        int prec, std::unique_ptr<ExprAST> e) {
    while (prec < get_expr_precedence()) {
        auto infix_fn = get_expr_rule(prev.type)->infix;
        e = std::invoke(infix_fn, *this, std::move(e));
        if (!e) return NULL;
    }
    return e;
}

std::unique_ptr<ExprAST> Parser::variable() {
    auto name = std::make_unique<std::string>(prev.lexeme);
    advance();
    return std::make_unique<VarExprAST>(std::move(name));
}

std::unique_ptr<ExprAST> Parser::number() {
    long v = std::stol(prev.lexeme);
    advance();
    return std::make_unique<NumberExprAST>(v);
}

std::unique_ptr<ExprAST> Parser::string() {
    auto str = prev.lexeme;
    advance();
    return std::make_unique<StringExprAST>(std::move(str));
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
    if (match(TOK_RPAREN)) {
        return std::make_unique<CallExprAST>(std::move(e), nullptr);
    }
    auto args = std::make_unique<CallExprAST::ArgList>();
    auto a = parse_expr(1);  // until ','
    if (!a) return NULL;
    args->emplace_back(std::move(a));
    while (match(TOK_COMMA)) {
        auto a = parse_expr(1);  // until ','
        if (!a) return NULL;
        args->emplace_back(std::move(a));
    }
    consume(TOK_RPAREN, "Expect ')'\n");
    return std::make_unique<CallExprAST>(std::move(e), std::move(args));
}

std::unique_ptr<ExprAST> Parser::unary() {
    Token token = prev;
    advance();
    auto e = parse_expr(13);  // until '*', '/' or '%'
    if (!e) return NULL;
    return std::make_unique<UnaryExprAST>(false, token, std::move(e));
}

std::unique_ptr<ExprAST> Parser::binary(std::unique_ptr<ExprAST> e) {
    // NOTE:
    // 1. Assignment is right associative.
    // 2. LHS of assignment must be unary expression.
    //
    // We handle point 1 by passing in a lower minimum precedence for the RHS
    // when parsing an assignment. This will ensure that an assignment operator
    // to the right of the current one will be parsed as part of the RHS. We
    // will check for point 2 in the semantic analysis phase.
    //
    Token token = prev;
    int prec = get_expr_precedence() - (prev.type == TOK_ASSIGN ? 1 : 0);
    advance();
    auto f = parse_expr(prec);
    if (!f) return NULL;
    return std::make_unique<BinaryExprAST>(token, std::move(e), std::move(f));
}

std::unique_ptr<ExprAST> Parser::ternary(std::unique_ptr<ExprAST> e) {
    // NOTE:
    // 1. @then_arm of conditional is parsed as if parenthesized. Precedence
    //    of '?' is not used.
    // 2. @else_arm of conditional must be another conditional. Here, prece-
    //    dence of '?' is used. Since the ternary operator is right-associa-
    //    tive, we pass in 1 less than the precedence of '?' here.
    //
    advance();  // '?'
    auto then_expr = parse_expr(0);
    if (!then_expr) return NULL;
    consume(TOK_COLON, "Expect ':'\n");
    auto else_expr = parse_expr(2);
    if (!else_expr) return NULL;
    return std::make_unique<TernaryExprAST>(std::move(e), std::move(then_expr),
                                            std::move(else_expr));
}

std::unique_ptr<ExprAST> Parser::postfix(std::unique_ptr<ExprAST> e) {
    Token token = prev;
    advance();
    return std::make_unique<UnaryExprAST>(true, token, std::move(e));
}

const Parser::ExprRule *Parser::get_expr_rule(TokenType type) {
    return type < ARRAY_LEN(expr_rules) ? &expr_rules[type] : NULL;
}
