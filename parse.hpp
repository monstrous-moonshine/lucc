#ifndef PARSE_HPP
#define PARSE_HPP
#include "scan.hpp"
#include <memory>
#include <vector>

class ExprAST {
public:
    virtual ~ExprAST() = default;
    virtual void print() = 0;
};

class NumberExprAST : public ExprAST {
    double v;
public:
    NumberExprAST(double v) : v(v) {}
    void print() override {
        printf("%.0lf", v);
    }
};

class BinaryExprAST : public ExprAST {
    Token token;
    std::unique_ptr<ExprAST> LHS, RHS;
public:
    BinaryExprAST(Token token, std::unique_ptr<ExprAST> LHS,
                  std::unique_ptr<ExprAST> RHS)
        : token(token), LHS(std::move(LHS)), RHS(std::move(RHS)) {}
    void print() override {
        printf("(%s ", &token.lexeme[0]);
        LHS->print();
        printf(" ");
        RHS->print();
        printf(")");
    }
};

class StmtAST {
public:
    virtual ~StmtAST() = default;
    virtual void print() = 0;
};

class ExprStmtAST : public StmtAST {
    std::unique_ptr<ExprAST> e;
public:
    ExprStmtAST(std::unique_ptr<ExprAST> e) : e(std::move(e)) {}
    void print() override {
        e->print();
        printf(";\n");
    }
};

class BlockStmtAST : public StmtAST {
    std::unique_ptr<std::vector<std::unique_ptr<StmtAST>>> stmts;
public:
    BlockStmtAST(std::unique_ptr<std::vector<std::unique_ptr<StmtAST>>> stmts)
        : stmts(std::move(stmts)) {}
    void print() override {
        printf("{\n");
        for (auto &stmt: *stmts) {
            stmt->print();
        }
        printf("}\n");
    }
};

class IfStmtAST : public StmtAST {
    std::unique_ptr<ExprAST> cond;
    std::unique_ptr<StmtAST> then_branch;
    std::unique_ptr<StmtAST> else_branch;
public:
    IfStmtAST(std::unique_ptr<ExprAST> cond,
              std::unique_ptr<StmtAST> then_branch,
              std::unique_ptr<StmtAST> else_branch)
        : cond(std::move(cond))
        , then_branch(std::move(then_branch))
        , else_branch(std::move(else_branch)) {}
    void print() override {
        printf("if (");
        cond->print();
        printf(")\n");
        then_branch->print();
        if (else_branch) {
            printf("else\n");
            else_branch->print();
        }
    }
};

class WhileStmtAST : public StmtAST {
    std::unique_ptr<ExprAST> cond;
    std::unique_ptr<StmtAST> body;
public:
    WhileStmtAST(std::unique_ptr<ExprAST> cond,
                 std::unique_ptr<StmtAST> body)
        : cond(std::move(cond)), body(std::move(body)) {}
    void print() override {
        printf("while (");
        cond->print();
        printf(")\n");
        body->print();
    }
};

class Parser {
public:
    Parser(Scanner &scanner) : scanner(scanner) { advance(); }
    std::unique_ptr<StmtAST> parse_stmt();
private:
    Scanner &scanner;
    Token prev;

    std::unique_ptr<ExprAST> parse_expr(int prec);
    void advance() { prev = scanner.scan(); }

    std::unique_ptr<ExprAST> number();
    std::unique_ptr<ExprAST> grouping();
    std::unique_ptr<ExprAST> binary(std::unique_ptr<ExprAST>);

    std::unique_ptr<StmtAST> block_stmt();
    std::unique_ptr<StmtAST> if_stmt();
    std::unique_ptr<StmtAST> while_stmt();

    using Prefix = std::unique_ptr<ExprAST> (Parser::*)();
    using Infix  = std::unique_ptr<ExprAST> (Parser::*)(
            std::unique_ptr<ExprAST>);
    struct ExprRule {
        Prefix prefix;
        Infix  infix;
        int    prec;
    };
    const ExprRule *get_expr_rule(TokenType type);
    int get_expr_precedence() {
        auto rule = get_expr_rule(prev.type);
        return rule && rule->infix ? rule->prec : 0;
    }
    static constexpr ExprRule expr_rules[] = {
    /* IDENT     */ {NULL, NULL, 0},
    /* INT_CONST */ {&Parser::number, NULL, 0},
    /* STRING    */ {NULL, NULL, 0},
    /* LPAREN    */ {&Parser::grouping, NULL, 0},
    /* RPAREN    */ {NULL, NULL, 0},
    /* LBRACKET  */ {NULL, NULL, 0},
    /* RBRACKET  */ {NULL, NULL, 0},
    /* INCR      */ {NULL, NULL, 0},
    /* DECR      */ {NULL, NULL, 0},
    /* TILDE     */ {NULL, NULL, 0},
    /* BANG      */ {NULL, NULL, 0},
    /* PLUS      */ {NULL, &Parser::binary, 1},
    /* MINUS     */ {NULL, &Parser::binary, 1},
    /* STAR      */ {NULL, &Parser::binary, 2},
    /* SLASH     */ {NULL, &Parser::binary, 2},
    /* MOD       */ {NULL, &Parser::binary, 2},
    /* LSHIFT    */ {NULL, NULL, 0},
    /* RSHIFT    */ {NULL, NULL, 0},
    /* EQ        */ {NULL, NULL, 0},
    /* NE        */ {NULL, NULL, 0},
    /* LT        */ {NULL, NULL, 0},
    /* GT        */ {NULL, NULL, 0},
    /* LE        */ {NULL, NULL, 0},
    /* GE        */ {NULL, NULL, 0},
    /* AND       */ {NULL, NULL, 0},
    /* XOR       */ {NULL, NULL, 0},
    /* OR        */ {NULL, NULL, 0},
    /* AND_AND   */ {NULL, NULL, 0},
    /* OR_OR     */ {NULL, NULL, 0},
    /* QUERY     */ {NULL, NULL, 0},
    /* COLON     */ {NULL, NULL, 0},
    /* ASSIGN    */ {NULL, NULL, 0},
    /* COMMA     */ {NULL, NULL, 0},
    };
};
#endif
