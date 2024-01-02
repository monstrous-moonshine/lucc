#ifndef DECL_HPP
#define DECL_HPP
#include "expr.hpp"
#include "scan.hpp"
#include <memory>
#include <string>
#include <utility>
#include <vector>
class StmtAST;

// A function declaration contains a block statement, and a block statement in
// turn contains declarations, so there's a circular dependency here. We deal
// with it by only keeping a forward declaration of StmtAST here, and putting
// the implementation of statement related functions into the cpp file. In the
// statements header file, we include this header normally.

class DirectDecl {
public:
    virtual ~DirectDecl() = default;
    // @has_postfix is passed true if we are the base of a function or array
    // declaration. In that case, the base declaration has to be parenthesized
    // if it's a pointer, since otherwise the function or array declaration
    // would bind more tightly than the pointer declaration. Note that only
    // Declarator cares about this since it potentially has some indirection.
    //
    // This function needs to be public in the base class (this one), but it
    // can be private in the subclasses since it's only ever called through
    // the base class.
    virtual void print(int level, bool has_postfix) = 0;
};

class Declarator : public DirectDecl {
    // Each level of pointer declaration can have cv-qualifiers, so normally a
    // simple int ptr_level wouldn't suffice. That, however, isn't a problem
    // for the moment, since we don't even support cv-qualifiers now. Each
    // pointer declaration is a simple int indicating the level of indirection.
    int ptr_level;
    std::unique_ptr<DirectDecl> decl;
    // This is the only place we need the @has_postfix argument, since a decla-
    // rator can appear recursively inside a direct_declarator in parenthesized
    // form, where it'd need the parentheses if it's the base of a function or
    // array declaration.
    void print(int level, bool has_postfix) override;
public:
    Declarator(int ptr_level, std::unique_ptr<DirectDecl> decl)
        : ptr_level(ptr_level), decl(std::move(decl)) {}
    // The top level declarator is not a direct_declarator, so pass false here.
    // We need this public interface here since the *DeclAST classes contain
    // one or more declarators and they want to print them.
    void print(int level) { print(level, false); }
};

class DeclASTBase {
    Token type;
public:
    DeclASTBase(Token type) : type(type) {}
    virtual ~DeclASTBase() = default;
    virtual void print(int level) = 0;
    Token get_type() { return type; }
};

class ExtDeclAST : public DeclASTBase {
public:
    ExtDeclAST(Token type) : DeclASTBase(type) {}
    virtual ~ExtDeclAST() = default;
};

class FuncDeclAST : public ExtDeclAST {
    std::unique_ptr<Declarator> decl;
    std::unique_ptr<StmtAST> body;
public:
    FuncDeclAST(Token type, std::unique_ptr<Declarator> decl,
                std::unique_ptr<StmtAST> body);
    void print(int level) override;
};

class InitDecl {
    std::unique_ptr<Declarator> decl;
    std::unique_ptr<ExprAST> init;
public:
    InitDecl(std::unique_ptr<Declarator> decl, std::unique_ptr<ExprAST> init)
        : decl(std::move(decl)), init(std::move(init)) {}
    void print(int level);
};

class DeclAST : public ExtDeclAST {
    std::unique_ptr<std::vector<InitDecl>> decl;
public:
    DeclAST(Token type, std::unique_ptr<std::vector<InitDecl>> decl)
        : ExtDeclAST(type), decl(std::move(decl)) {}
    void print(int level) override;
};

class ParamDeclAST : public DeclASTBase {
    std::unique_ptr<Declarator> decl;
public:
    ParamDeclAST(Token type, std::unique_ptr<Declarator> decl)
        : DeclASTBase(type), decl(std::move(decl)) {}
    void print(int level) override;
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
public:
    using ParamList = std::vector<std::unique_ptr<ParamDeclAST>>;
private:
    bool is_variadic;
    std::unique_ptr<DirectDecl> name;
    std::unique_ptr<ParamList> params;
    void print(int level, bool) override;
public:
    FuncDecl(bool is_variadic, std::unique_ptr<DirectDecl> name,
             std::unique_ptr<ParamList> params)
        : is_variadic(is_variadic), name(std::move(name))
        , params(std::move(params)) {}
};
#endif
