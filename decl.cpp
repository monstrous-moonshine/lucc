#include "decl.hpp"
#include "stmt.hpp"
#include <cstdio>

namespace
{

void indent(int level) {
    for (int i = 0; i < level; i++) fputc(' ', stdout);
}

}

void Declarator::print(int level, bool has_postfix) {
    if (has_postfix && ptr_level > 0) printf("(");
    for (int i = 0; i < ptr_level; i++) fputc('*', stdout);
    decl->print(level, false);
    if (has_postfix && ptr_level > 0) printf(")");
}

FuncDeclAST::FuncDeclAST(Token type, std::unique_ptr<Declarator> decl,
                         std::unique_ptr<StmtAST> body)
    : type(type), decl(std::move(decl)), body(std::move(body)) {}

void FuncDeclAST::print(int level) {
    indent(level);
    printf("%s ", &type.lexeme[0]);
    decl->print(level);
    printf("\n");
    body->print(level);
}

void InitDecl::print(int level) {
    decl->print(level);
    if (init) {
        printf(" = ");
        init->print();
    }
}

void DeclAST::print(int level) {
    indent(level);
    printf("%s ", &type.lexeme[0]);
    (*decl)[0]->print(level);
    for (size_t i = 1; i < decl->size(); i++) {
        printf(", ");
        (*decl)[i]->print(level);
    }
    printf(";\n");
}

void ParamDeclAST::print(int level) {
    indent(level);
    printf("%s", &type.lexeme[0]);
    if (decl) {
        printf(" ");
        decl->print(level);
    }
}

void VarDecl::print(int, bool) {
    printf("%s", &name[0]);
}

void ArrayDecl::print(int level, bool) {
    name->print(level, true);
    printf("[");
    if (dim)
        dim->print();
    printf("]");
}

void FuncDecl::print(int level, bool) {
    name->print(level, true);
    printf("(\n");
    if (params) {
        (*params)[0]->print(level + 2);
        for (size_t i = 1; i < params->size(); i++) {
            printf(",\n");
            (*params)[i]->print(level + 2);
        }
    }
    if (is_variadic)
        printf(", ...");
    printf(")");
}
