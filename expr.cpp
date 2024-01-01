#include "expr.hpp"
#include <cstdio>

void VarExprAST::print() {
    printf("%s", &name[0]);
}

void NumberExprAST::print() {
    printf("%.0lf", v);
}

void StringExprAST::print() {
    printf("\"%s\"", &str[0]);
}

void IndexExprAst::print() {
    printf("([] ");
    base->print();
    printf(" ");
    index->print();
    printf(")");
}

void CallExprAST::print() {
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

void UnaryExprAST::print() {
    printf("(%s ", &token.lexeme[0]);
    exp->print();
    printf(")");
}

void BinaryExprAST::print() {
    printf("(%s ", &token.lexeme[0]);
    LHS->print();
    printf(" ");
    RHS->print();
    printf(")");
}

void TernaryExprAST::print() {
    printf("(? ");
    cond->print();
    printf(" ");
    then_expr->print();
    printf(" ");
    else_expr->print();
    printf(")");
}

void PostfixExprAST::print() {
    printf("(>%s ", &token.lexeme[0]);
    exp->print();
    printf(")");
}
