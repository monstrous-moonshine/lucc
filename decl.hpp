#ifndef DECL_HPP
#define DECL_HPP
#include "expr.hpp"
#include "scan.hpp"
#include "stmt.hpp"
#include <memory>
#include <string>
#include <utility>
#include <vector>
class Decl;

class DeclAST {
    bool is_param;
    Token type;
    std::unique_ptr<Decl> decl;
    std::unique_ptr<StmtAST> body;
public:
    DeclAST(bool is_param, Token type,
            std::unique_ptr<Decl> decl,
            std::unique_ptr<StmtAST> body)
        : is_param(is_param), type(type), decl(std::move(decl))
        , body(std::move(body)) {}
    void print(int level);
};

class DirectDecl {
public:
    virtual ~DirectDecl() = default;
    virtual void print(int level) = 0;
};

class Decl : public DirectDecl {
    int ptr_level;
    std::unique_ptr<DirectDecl> decl;
public:
    Decl(int ptr_level, std::unique_ptr<DirectDecl> decl)
        : ptr_level(ptr_level), decl(std::move(decl)) {}
    void print(int level) override {
        for (int i = 0; i < ptr_level; i++) fputc('*', stdout);
        decl->print(level);
    }
};

inline void DeclAST::print(int level) {
    for (int i = 0; i < level; i++) fputc(' ', stdout);
    printf("%s ", &type.lexeme[0]);
    decl->print(level);
    if (is_param) return;
    if (body) {
        printf("\n");
        body->print(level);
    } else {
        printf(";\n");
    }
}

class VarDecl : public DirectDecl {
    std::string name;
public:
    VarDecl(std::string &&name) : name(name) {}
    void print(int) override {
        printf("%s", &name[0]);
    }
};

class ArrayDecl : public DirectDecl {
    std::unique_ptr<DirectDecl> name;
    std::unique_ptr<ExprAST> dim;
public:
    ArrayDecl(std::unique_ptr<DirectDecl> name, std::unique_ptr<ExprAST> dim)
        : name(std::move(name)), dim(std::move(dim)) {}
    void print(int level) override {
        printf("(");
        name->print(level);
        printf(")");
        printf("[");
        dim->print();
        printf("]");
    }
};

class FuncDecl : public DirectDecl {
    std::unique_ptr<DirectDecl> name;
    std::unique_ptr<std::vector<std::unique_ptr<DeclAST>>> params;
public:
    FuncDecl(std::unique_ptr<DirectDecl> name,
             std::unique_ptr<std::vector<std::unique_ptr<DeclAST>>> params)
        : name(std::move(name)), params(std::move(params)) {}
    void print(int level) override {
        printf("(");
        name->print(level);
        printf(")");
        printf("(\n");
        if (params) {
            (*params)[0]->print(level + 2);
            for (size_t i = 1; i < params->size(); i++) {
                printf(",\n");
                (*params)[i]->print(level + 2);
            }
        }
        printf(")");
    }
};
#endif
