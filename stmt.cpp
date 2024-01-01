#include "stmt.hpp"
#include <cstdio>

namespace
{

void indent(int level) {
    for (int i = 0; i < level; i++) fputc(' ', stdout);
}

}

void LabelStmtAST::print(int level) {
    indent(level - 2);
    switch (type) {
    case LABEL:
        label->print();
        printf(":\n");
        break;
    case CASE:
        printf("case ");
        case_exp->print();
        printf(":\n");
        break;
    case DEFAULT:
        printf("default:\n");
        break;
    }
    stmt->print(level);
}

void ExprStmtAST::print(int level) {
    indent(level);
    e->print();
    printf(";\n");
}

void BlockStmtAST::print(int level) {
    indent(level);
    printf("{\n");
    for (auto &decl: *decls) {
        decl->print(level + 2);
    }
    for (auto &stmt: *stmts) {
        stmt->print(level + 2);
    }
    indent(level);
    printf("}\n");
}

void IfStmtAST::print(int level) {
    indent(level);
    printf("if (");
    cond->print();
    printf(")\n");
    then_branch->print(level + 2);
    if (else_branch) {
        indent(level);
        printf("else\n");
        else_branch->print(level + 2);
    }
}

void SwitchStmtAST::print(int level) {
    indent(level);
    printf("switch (");
    cond->print();
    printf(")\n");
    body->print(level + 2);
}

void ForStmtAST::print(int level) {
    indent(level);
    printf("for (");
    if (init) init->print();
    printf("; ");
    if (cond) cond->print();
    printf("; ");
    if (incr) incr->print();
    printf(")\n");
    body->print(level + 2);
}

void WhileStmtAST::print(int level) {
    indent(level);
    printf("while (");
    cond->print();
    printf(")\n");
    body->print(level + 2);
}

void DoStmtAST::print(int level) {
    indent(level);
    printf("do\n");
    body->print(level + 2);
    printf("while (");
    cond->print();
    printf(")\n");
}

void JumpStmtAST::print(int level) {
    indent(level);
    switch (type) {
    case GOTO:
        printf("goto ");
        label->print();
        printf("\n");
        break;
    case CONTINUE:
        printf("continue;\n");
        break;
    case BREAK:
        printf("break;\n");
        break;
    }
}

void ReturnStmtAST::print(int level) {
    indent(level);
    printf("return");
    if (e) {
        printf(" ");
        e->print();
    }
    printf(";\n");
}
