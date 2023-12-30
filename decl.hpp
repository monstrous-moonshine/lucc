#ifndef DECL_HPP
#define DECL_HPP
#include "expr.hpp"
#include "scan.hpp"
#include <memory>
#include <string>
#include <utility>
#include <vector>
class Decl;
class StmtAST;

class DeclAST {
    bool is_param;
    Token type;
    std::unique_ptr<Decl> decl;
    std::unique_ptr<StmtAST> body;
public:
    DeclAST(bool is_param, Token type,
            std::unique_ptr<Decl> decl,
            std::unique_ptr<StmtAST> body);
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
    void print(int level) override;
};

class VarDecl : public DirectDecl {
    std::string name;
public:
    VarDecl(std::string &&name) : name(name) {}
    void print(int) override;
};

class ArrayDecl : public DirectDecl {
    std::unique_ptr<DirectDecl> name;
    std::unique_ptr<ExprAST> dim;
public:
    ArrayDecl(std::unique_ptr<DirectDecl> name, std::unique_ptr<ExprAST> dim)
        : name(std::move(name)), dim(std::move(dim)) {}
    void print(int level) override;
};

class FuncDecl : public DirectDecl {
    std::unique_ptr<DirectDecl> name;
    std::unique_ptr<std::vector<std::unique_ptr<DeclAST>>> params;
public:
    FuncDecl(std::unique_ptr<DirectDecl> name,
             std::unique_ptr<std::vector<std::unique_ptr<DeclAST>>> params)
        : name(std::move(name)), params(std::move(params)) {}
    void print(int level) override;
};
#endif
