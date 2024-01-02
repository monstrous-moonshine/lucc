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
    // The only thing that'll definitely be present is type.
    // If it's a data or prototype declaration, body will be
    // absent. In addition, if it's a parameter declaration,
    // decl might be absent too, like in this example:
    //
    // int foo(int, float);
    //
    // To rephrase, non-parameter declarations can be:
    // - data or prototype declaration: ends in ';'
    // - function definition: ends in block stmt
    //
    // While parameter declarations have:
    // - no body
    // - possibly no decl
    //
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
    virtual void print(int level, bool) = 0;
};

class Decl : public DirectDecl {
    int ptr_level;
    std::unique_ptr<DirectDecl> decl;
    void print(int level, bool paren_if_ptr) override;
public:
    Decl(int ptr_level, std::unique_ptr<DirectDecl> decl)
        : ptr_level(ptr_level), decl(std::move(decl)) {}
    void print(int level) { print(level, false); }
};

class VarDecl : public DirectDecl {
    std::string name;
    void print(int, bool) override;
public:
    VarDecl(std::string &&name) : name(name) {}
};

class ArrayDecl : public DirectDecl {
    std::unique_ptr<DirectDecl> name;
    std::unique_ptr<ExprAST> dim;
    void print(int level, bool) override;
public:
    ArrayDecl(std::unique_ptr<DirectDecl> name, std::unique_ptr<ExprAST> dim)
        : name(std::move(name)), dim(std::move(dim)) {}
};

class FuncDecl : public DirectDecl {
    bool is_variadic;
    std::unique_ptr<DirectDecl> name;
    std::unique_ptr<std::vector<std::unique_ptr<DeclAST>>> params;
    void print(int level, bool) override;
public:
    FuncDecl(bool is_variadic, std::unique_ptr<DirectDecl> name,
             std::unique_ptr<std::vector<std::unique_ptr<DeclAST>>> params)
        : is_variadic(is_variadic), name(std::move(name))
        , params(std::move(params)) {}
};
#endif
