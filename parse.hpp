#ifndef PARSE_HPP
#define PARSE_HPP
#include "scan.hpp"
#include <cstddef>
#include <memory>
class ExprAST;
class StmtAST;
class DeclAST;
class Decl;
class DirectDecl;

class Parser {
public:
    Parser(Scanner &scanner) : scanner(scanner) { advance(); }
    std::unique_ptr<DeclAST> parse_decl() { return parse_decl(false); }
private:
    Scanner &scanner;
    Token prev;

    void advance() { prev = scanner.scan(); }

    std::unique_ptr<DeclAST> parse_decl(bool is_param);
    std::unique_ptr<Decl> parse_declarator();
    std::unique_ptr<DirectDecl> parse_direct_declarator();
    std::unique_ptr<StmtAST> parse_stmt();
    std::unique_ptr<ExprAST> parse_expr(int prec);

    std::unique_ptr<ExprAST> variable();
    std::unique_ptr<ExprAST> number();
    std::unique_ptr<ExprAST> grouping();
    std::unique_ptr<ExprAST> index(std::unique_ptr<ExprAST>);
    std::unique_ptr<ExprAST> call(std::unique_ptr<ExprAST>);
    std::unique_ptr<ExprAST> unary();
    std::unique_ptr<ExprAST> binary(std::unique_ptr<ExprAST>);
    std::unique_ptr<ExprAST> ternary(std::unique_ptr<ExprAST>);

    std::unique_ptr<StmtAST> block_stmt();
    std::unique_ptr<StmtAST> if_stmt();
    std::unique_ptr<StmtAST> while_stmt();

    using Prefix = std::unique_ptr<ExprAST> (Parser::*)();
    using Infix  = std::unique_ptr<ExprAST> (Parser::*)(
            std::unique_ptr<ExprAST>);
    struct ExprRule {
        Prefix prefix;
        Infix  infix;
        int    prec;
    };
    const ExprRule *get_expr_rule(TokenType type);
    int get_expr_precedence() {
        auto rule = get_expr_rule(prev.type);
        return rule && rule->infix ? rule->prec : 0;
    }
    static constexpr ExprRule expr_rules[] = {
    /* IDENT     */ {&Parser::variable, NULL, 0},
    /* INT_CONST */ {&Parser::number, NULL, 0},
    /* STRING    */ {NULL, NULL, 0},
    /* LPAREN    */ {&Parser::grouping, &Parser::call, 14},
    /* RPAREN    */ {NULL, NULL, 0},
    /* LBRACKET  */ {NULL, &Parser::index, 14},
    /* RBRACKET  */ {NULL, NULL, 0},
    /* INCR      */ {&Parser::unary, NULL, 0},
    /* DECR      */ {&Parser::unary, NULL, 0},
    /* TILDE     */ {&Parser::unary, NULL, 0},
    /* BANG      */ {&Parser::unary, NULL, 0},
    /* STAR      */ {NULL, &Parser::binary, 13},
    /* SLASH     */ {NULL, &Parser::binary, 13},
    /* MOD       */ {NULL, &Parser::binary, 13},
    /* PLUS      */ {&Parser::unary, &Parser::binary, 12},
    /* MINUS     */ {&Parser::unary, &Parser::binary, 12},
    /* LSHIFT    */ {NULL, &Parser::binary, 11},
    /* RSHIFT    */ {NULL, &Parser::binary, 11},
    /* LT        */ {NULL, &Parser::binary, 10},
    /* GT        */ {NULL, &Parser::binary, 10},
    /* LE        */ {NULL, &Parser::binary, 10},
    /* GE        */ {NULL, &Parser::binary, 10},
    /* EQ        */ {NULL, &Parser::binary, 9},
    /* NE        */ {NULL, &Parser::binary, 9},
    /* AND       */ {NULL, &Parser::binary, 8},
    /* XOR       */ {NULL, &Parser::binary, 7},
    /* OR        */ {NULL, &Parser::binary, 6},
    /* AND_AND   */ {NULL, &Parser::binary, 5},
    /* OR_OR     */ {NULL, &Parser::binary, 4},
    /* QUERY     */ {NULL, &Parser::ternary, 3},
    /* COLON     */ {NULL, NULL, 0},
    /* ASSIGN    */ {NULL, &Parser::binary, 2},
    /* COMMA     */ {NULL, NULL, 1},
    };
};
#endif
