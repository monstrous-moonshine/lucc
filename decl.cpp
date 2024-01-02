#include "decl.hpp"
#include "stmt.hpp"
#include <cstdio>

DeclAST::DeclAST(bool is_param, Token type,
                 std::unique_ptr<Decl> decl,
                 std::unique_ptr<StmtAST> body)
    : is_param(is_param), type(type), decl(std::move(decl))
    , body(std::move(body)) {}

void DeclAST::print(int level) {
    for (int i = 0; i < level; i++) fputc(' ', stdout);
    printf("%s", &type.lexeme[0]);
    if (is_param && !decl) return;
    printf(" ");
    decl->print(level);
    if (is_param) return;
    if (body) {
        printf("\n");
        body->print(level);
    } else {
        printf(";\n");
    }
}

void Decl::print(int level, bool paren_if_ptr) {
    if (paren_if_ptr && ptr_level > 0) printf("(");
    for (int i = 0; i < ptr_level; i++) fputc('*', stdout);
    decl->print(level, false);
    if (paren_if_ptr && ptr_level > 0) printf(")");
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
