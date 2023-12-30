#ifndef STMT_HPP
#define STMT_HPP
#include "expr.hpp"
#include <memory>
#include <utility>
#include <vector>

class StmtAST {
public:
    virtual ~StmtAST() = default;
    virtual void print(int level) = 0;
};

class ExprStmtAST : public StmtAST {
    std::unique_ptr<ExprAST> e;
public:
    ExprStmtAST(std::unique_ptr<ExprAST> e) : e(std::move(e)) {}
    void print(int level) override;
};

class BlockStmtAST : public StmtAST {
    std::unique_ptr<std::vector<std::unique_ptr<StmtAST>>> stmts;
public:
    BlockStmtAST(std::unique_ptr<std::vector<std::unique_ptr<StmtAST>>> stmts)
        : stmts(std::move(stmts)) {}
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

class WhileStmtAST : public StmtAST {
    std::unique_ptr<ExprAST> cond;
    std::unique_ptr<StmtAST> body;
public:
    WhileStmtAST(std::unique_ptr<ExprAST> cond,
                 std::unique_ptr<StmtAST> body)
        : cond(std::move(cond)), body(std::move(body)) {}
    void print(int level) override;
};

class ReturnStmtAST : public StmtAST {
    std::unique_ptr<ExprAST> e;
public:
    ReturnStmtAST(std::unique_ptr<ExprAST> e) : e(std::move(e)) {}
    void print(int level) override;
};
#endif
