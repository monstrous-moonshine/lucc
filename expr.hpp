#ifndef EXPR_HPP
#define EXPR_HPP
#include "scan.hpp"
#include <cstdio>
#include <memory>
#include <string>
#include <utility>
#include <vector>

class ExprAST {
public:
    virtual ~ExprAST() = default;
    virtual void print() = 0;
};

class VarExprAST : public ExprAST {
    std::string name;
public:
    VarExprAST(std::string &&name) : name(name) {}
    void print() override {
        printf("%s", &name[0]);
    }
};

class NumberExprAST : public ExprAST {
    double v;
public:
    NumberExprAST(double v) : v(v) {}
    void print() override {
        printf("%.0lf", v);
    }
};

class IndexExprAst : public ExprAST {
    std::unique_ptr<ExprAST> base;
    std::unique_ptr<ExprAST> index;
public:
    IndexExprAst(std::unique_ptr<ExprAST> base,
                 std::unique_ptr<ExprAST> index)
        : base(std::move(base)), index(std::move(index)) {}
    void print() override {
        printf("([] ");
        base->print();
        printf(" ");
        index->print();
        printf(")");
    }
};

class CallExprAST : public ExprAST {
    std::unique_ptr<ExprAST> func;
    std::unique_ptr<std::vector<std::unique_ptr<ExprAST>>> args;
public:
    CallExprAST(std::unique_ptr<ExprAST> func,
                std::unique_ptr<std::vector<std::unique_ptr<ExprAST>>> args)
        : func(std::move(func)), args(std::move(args)) {}
    void print() override {
        printf("(");
        func->print();
        if (args) {
            for (auto &e: *args) {
                printf(" ");
                e->print();
            }
        }
        printf(")");
    }
};

class UnaryExprAST : public ExprAST {
    Token token;
    std::unique_ptr<ExprAST> exp;
public:
    UnaryExprAST(Token token, std::unique_ptr<ExprAST> exp)
        : token(token), exp(std::move(exp)) {}
    void print() override {
        printf("(%s ", &token.lexeme[0]);
        exp->print();
        printf(")");
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

class TernaryExprAST : public ExprAST {
    std::unique_ptr<ExprAST> cond, then_expr, else_expr;
public:
    TernaryExprAST(std::unique_ptr<ExprAST> cond,
                   std::unique_ptr<ExprAST> then_expr,
                   std::unique_ptr<ExprAST> else_expr)
        : cond(std::move(cond)), then_expr(std::move(then_expr))
        , else_expr(std::move(else_expr)) {}
    void print() override {
        printf("(if ");
        cond->print();
        printf(" ");
        then_expr->print();
        printf(" ");
        else_expr->print();
        printf(")");
    }
};
#endif