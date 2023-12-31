#include "stmt.hpp"
#include <cstdio>

namespace
{

void blank(int level) {
    for (int i = 0; i < level; i++) fputc(' ', stdout);
}

}

void ExprStmtAST::print(int level) {
    blank(level);
    e->print();
    printf(";\n");
}

void BlockStmtAST::print(int level) {
    blank(level);
    printf("{\n");
    for (auto &decl: *decls) {
        decl->print(level + 2);
    }
    for (auto &stmt: *stmts) {
        stmt->print(level + 2);
    }
    blank(level);
    printf("}\n");
}

void IfStmtAST::print(int level) {
    blank(level);
    printf("if (");
    cond->print();
    printf(")\n");
    then_branch->print(level + 2);
    if (else_branch) {
        blank(level);
        printf("else\n");
        else_branch->print(level + 2);
    }
}

void WhileStmtAST::print(int level) {
    blank(level);
    printf("while (");
    cond->print();
    printf(")\n");
    body->print(level + 2);
}

void ReturnStmtAST::print(int level) {
    blank(level);
    printf("return");
    if (e) {
        printf(" ");
        e->print();
    }
    printf(";\n");
}
