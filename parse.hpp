#ifndef PARSE_HPP
#define PARSE_HPP
#include "scan.hpp"
#include <cstddef>
#include <memory>
#include <vector>
class ExprAST;
class StmtAST;
class DeclAST;
class Decl;
class DirectDecl;

class Parser {
public:
    Parser(Scanner &scanner) : scanner(scanner) { advance(); }
    std::unique_ptr<std::vector<std::unique_ptr<DeclAST>>> parse_trans_unit();
private:
    Scanner &scanner;
    Token prev;

    void advance() { prev = scanner.scan(); }
    bool match(TokenType type) {
        if (prev.type == type) {
            advance();
            return true;
        }
        return false;
    }

    Token parse_type_spec();
    std::unique_ptr<DeclAST> parse_external_decl();
    std::unique_ptr<DeclAST> parse_param_decl();
    std::unique_ptr<Decl> parse_declarator();
    std::unique_ptr<DirectDecl> parse_direct_declarator();
    std::unique_ptr<DirectDecl> parse_array_decl(std::unique_ptr<DirectDecl>);
    std::unique_ptr<DirectDecl> parse_func_decl(std::unique_ptr<DirectDecl>);
    std::unique_ptr<StmtAST> parse_stmt();
    std::unique_ptr<ExprAST> parse_expr(int prec);

    std::unique_ptr<ExprAST> variable();
    std::unique_ptr<ExprAST> number();
    std::unique_ptr<ExprAST> string();
    std::unique_ptr<ExprAST> grouping();
    std::unique_ptr<ExprAST> index(std::unique_ptr<ExprAST>);
    std::unique_ptr<ExprAST> call(std::unique_ptr<ExprAST>);
    std::unique_ptr<ExprAST> unary();
    std::unique_ptr<ExprAST> binary(std::unique_ptr<ExprAST>);
    std::unique_ptr<ExprAST> ternary(std::unique_ptr<ExprAST>);
    std::unique_ptr<ExprAST> postfix(std::unique_ptr<ExprAST>);

    std::unique_ptr<StmtAST> label_stmt();
    std::unique_ptr<StmtAST> case_stmt();
    std::unique_ptr<StmtAST> default_stmt();
    std::unique_ptr<StmtAST> block_stmt();
    std::unique_ptr<StmtAST> if_stmt();
    std::unique_ptr<StmtAST> switch_stmt();
    std::unique_ptr<StmtAST> for_stmt();
    std::unique_ptr<StmtAST> while_stmt();
    std::unique_ptr<StmtAST> do_stmt();
    std::unique_ptr<StmtAST> continue_stmt();
    std::unique_ptr<StmtAST> break_stmt();
    std::unique_ptr<StmtAST> return_stmt();
    std::unique_ptr<StmtAST> empty_stmt();

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
    /* STRING    */ {&Parser::string, NULL, 0},
    /* LPAREN    */ {&Parser::grouping, &Parser::call, 14},
    /* RPAREN    */ {NULL, NULL, 0},
    /* LBRACKET  */ {NULL, &Parser::index, 14},
    /* RBRACKET  */ {NULL, NULL, 0},
    /* INCR      */ {&Parser::unary, &Parser::postfix, 14},
    /* DECR      */ {&Parser::unary, &Parser::postfix, 14},
    /* TILDE     */ {&Parser::unary, NULL, 0},
    /* BANG      */ {&Parser::unary, NULL, 0},
    /* SIZEOF    */ {NULL, NULL, 0},
    /* STAR      */ {&Parser::unary, &Parser::binary, 13},
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
    /* AND       */ {&Parser::unary, &Parser::binary, 8},
    /* XOR       */ {NULL, &Parser::binary, 7},
    /* OR        */ {NULL, &Parser::binary, 6},
    /* AND_AND   */ {NULL, &Parser::binary, 5},
    /* OR_OR     */ {NULL, &Parser::binary, 4},
    /* QUERY     */ {NULL, &Parser::ternary, 3},
    /* COLON     */ {NULL, NULL, 0},
    /* ASSIGN    */ {NULL, &Parser::binary, 2},
    /* COMMA     */ {NULL, NULL, 1},
    };

    using StmtParser = std::unique_ptr<StmtAST> (Parser::*)();
    StmtParser get_stmt_parser(TokenType type);
    static constexpr StmtParser stmt_parsers[] = {
        &Parser::case_stmt,
        &Parser::default_stmt,
        &Parser::if_stmt,
        NULL,  // else
        &Parser::switch_stmt,
        &Parser::for_stmt,
        &Parser::while_stmt,
        &Parser::do_stmt,
        NULL,  // goto
        &Parser::continue_stmt,
        &Parser::break_stmt,
        &Parser::return_stmt,
        &Parser::block_stmt,
        NULL, // '}'
        &Parser::empty_stmt,
    };
};
#endif
