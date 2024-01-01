#ifndef STMT_HPP
#define STMT_HPP
#include "decl.hpp"
#include "expr.hpp"
#include <memory>
#include <utility>
#include <vector>

class StmtAST {
public:
    virtual ~StmtAST() = default;
    virtual void print(int level) = 0;
};

class LabelStmtAST : public StmtAST {
public:
    enum LabelType {
        LABEL,
        CASE,
        DEFAULT,
    };
private:
    LabelType type;
    std::unique_ptr<ExprAST> label;
    std::unique_ptr<ExprAST> case_exp;
    std::unique_ptr<StmtAST> stmt;
public:
    LabelStmtAST(LabelType type, std::unique_ptr<ExprAST> label,
                 std::unique_ptr<ExprAST> case_exp,
                 std::unique_ptr<StmtAST> stmt)
        : type(type), label(std::move(label))
        , case_exp(std::move(case_exp)), stmt(std::move(stmt)) {}
    void print(int level) override;
};

class ExprStmtAST : public StmtAST {
    std::unique_ptr<ExprAST> e;
public:
    ExprStmtAST(std::unique_ptr<ExprAST> e) : e(std::move(e)) {}
    void print(int level) override;
};

class BlockStmtAST : public StmtAST {
    std::unique_ptr<std::vector<std::unique_ptr<DeclAST>>> decls;
    std::unique_ptr<std::vector<std::unique_ptr<StmtAST>>> stmts;
public:
    BlockStmtAST(std::unique_ptr<std::vector<std::unique_ptr<DeclAST>>> decls,
                 std::unique_ptr<std::vector<std::unique_ptr<StmtAST>>> stmts)
        : decls(std::move(decls)), stmts(std::move(stmts)) {}
    void print(int level) override;
};

class IfStmtAST : public StmtAST {
    std::unique_ptr<ExprAST> cond;
    std::unique_ptr<StmtAST> then_branch;
    std::unique_ptr<StmtAST> else_branch;
public:
    IfStmtAST(std::unique_ptr<ExprAST> cond,
              std::unique_ptr<StmtAST> then_branch,
              std::unique_ptr<StmtAST> else_branch)
        : cond(std::move(cond))
        , then_branch(std::move(then_branch))
        , else_branch(std::move(else_branch)) {}
    void print(int level) override;
};

class SwitchStmtAST : public StmtAST {
    std::unique_ptr<ExprAST> cond;
    std::unique_ptr<StmtAST> body;
public:
    SwitchStmtAST(std::unique_ptr<ExprAST> cond,
                  std::unique_ptr<StmtAST> body)
        : cond(std::move(cond)), body(std::move(body)) {}
    void print(int level) override;
};

class ForStmtAST : public StmtAST {
    std::unique_ptr<ExprAST> init;
    std::unique_ptr<ExprAST> cond;
    std::unique_ptr<ExprAST> incr;
    std::unique_ptr<StmtAST> body;
public:
    ForStmtAST(std::unique_ptr<ExprAST> init,
               std::unique_ptr<ExprAST> cond,
               std::unique_ptr<ExprAST> incr,
               std::unique_ptr<StmtAST> body)
        : init(std::move(init)), cond(std::move(cond))
        , incr(std::move(incr)), body(std::move(body)) {}
    void print(int level) override;
};

class WhileStmtAST : public StmtAST {
    std::unique_ptr<ExprAST> cond;
    std::unique_ptr<StmtAST> body;
public:
    WhileStmtAST(std::unique_ptr<ExprAST> cond,
                 std::unique_ptr<StmtAST> body)
        : cond(std::move(cond)), body(std::move(body)) {}
    void print(int level) override;
};

class DoStmtAST : public StmtAST {
    std::unique_ptr<ExprAST> cond;
    std::unique_ptr<StmtAST> body;
public:
    DoStmtAST(std::unique_ptr<ExprAST> cond,
              std::unique_ptr<StmtAST> body)
        : cond(std::move(cond)), body(std::move(body)) {}
    void print(int level) override;
};

class JumpStmtAST : public StmtAST {
public:
    enum JumpType {
        GOTO,
        CONTINUE,
        BREAK,
    };
private:
    JumpType type;
    std::unique_ptr<ExprAST> label;
public:
    JumpStmtAST(JumpType type, std::unique_ptr<ExprAST> label)
        : type(type), label(std::move(label)) {}
    void print(int level) override;
};

class ReturnStmtAST : public StmtAST {
    std::unique_ptr<ExprAST> e;
public:
    ReturnStmtAST(std::unique_ptr<ExprAST> e) : e(std::move(e)) {}
    void print(int level) override;
};

class EmptyStmtAST : public StmtAST {
public:
    void print(int level) override;
};
#endif
