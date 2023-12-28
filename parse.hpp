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

#if 0
class StmtAST {
public:
    virtual ~StmtAST() = default;
};

class ExprStmtAST : public StmtAST {
    std::unique_ptr<ExprAST> e;
public:
    ExprStmtAST(std::unique_ptr<ExprAST> e) : e(std::move(e)) {}
};

class BlockStmtAST : public StmtAST {
    std::unique_ptr<std::vector<std::unique_ptr<StmtAST>>> stmts;
public:
    BlockStmtAST(std::unique_ptr<std::vector<std::unique_ptr<StmtAST>>> stmts)
        : stmts(std::move(stmts)) {}
};

class IfStmtAST : public StmtAST {
    std::unique_ptr<StmtAST> then_branch;
    std::unique_ptr<StmtAST> else_branch;
public:
    IfStmtAST(std::unique_ptr<StmtAST> then_branch,
              std::unique_ptr<StmtAST> else_branch)
        : then_branch(std::move(then_branch))
        , else_branch(std::move(else_branch)) {}
};
#endif

class Parser {
public:
    Parser(Scanner &scanner) : scanner(scanner) { advance(); }
    std::unique_ptr<ExprAST> parse(int prec);
private:
    Scanner &scanner;
    Token prev;

    void advance() { prev = scanner.scan(); }
    int get_precedence();

    std::unique_ptr<ExprAST> number();
    std::unique_ptr<ExprAST> grouping();
    std::unique_ptr<ExprAST> binary(std::unique_ptr<ExprAST>);

    using Unary  = std::unique_ptr<ExprAST> (Parser::*)();
    using Binary = std::unique_ptr<ExprAST> (Parser::*)(
            std::unique_ptr<ExprAST>);
    static constexpr struct {
        Unary  prefix;
        Binary infix;
        int    prec;
    } rules[] = {
    /* IDENT     */ {NULL, NULL, 0},
    /* NUMBER    */ {&Parser::number, NULL, 0},
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
    /* AMPERSAND */ {NULL, NULL, 0},
    /* CARET     */ {NULL, NULL, 0},
    /* PIPE      */ {NULL, NULL, 0},
    /* AND       */ {NULL, NULL, 0},
    /* OR        */ {NULL, NULL, 0},
    /* QUERY     */ {NULL, NULL, 0},
    /* COLON     */ {NULL, NULL, 0},
    /* ASSIGN    */ {NULL, NULL, 0},
    /* COMMA     */ {NULL, NULL, 0},
    /* K_IF      */ {NULL, NULL, 0},
    /* K_SWITCH  */ {NULL, NULL, 0},
    /* K_FOR     */ {NULL, NULL, 0},
    /* K_WHILE   */ {NULL, NULL, 0},
    /* K_DO      */ {NULL, NULL, 0},
    /* K_RETURN  */ {NULL, NULL, 0},
    /* LBRACE    */ {NULL, NULL, 0},
    /* RBRACE    */ {NULL, NULL, 0},
    /* SEMICOLON */ {NULL, NULL, 0},
    /* EOF       */ {NULL, NULL, 0},
    };
};
#endif
