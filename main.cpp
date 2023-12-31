#include "decl.hpp"
#include "parse.hpp"
#include "scan.hpp"
#include "stmt.hpp"
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

namespace
{

std::unique_ptr<std::string> read_file(const char *name) {
    std::ifstream file(name);
    if (file.is_open()) {
        std::ostringstream buf;
        buf << file.rdbuf();
        return std::make_unique<std::string>(std::move(*buf.rdbuf()).str());
    } else {
        return NULL;
    }
}

}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: mycc <program>\n");
        exit(1);
    }
    auto src = read_file(argv[1]);
    if (!src) {
        fprintf(stderr, "mycc: %s: No such file or directory\n", argv[1]);
        exit(1);
    }
    Scanner scanner(src->c_str());
    Parser parser(scanner);
#if 0
    for (;;) {
        auto token = scanner.scan();
        if (token.type == TOK_EOF) break;
        else if (token.type == TOK_ERR) {
            fprintf(stderr, "mycc: error in token '%s'\n", token.lexeme.c_str());
            break;
        } else {
            printf("%2d '%s'\n", token.type, token.lexeme.c_str());
        }
    }
#endif
    auto decls = parser.parse_translation_unit();
    if (decls) {
        for (auto &decl: *decls) {
            decl->print(0);
        }
    }
    else {
        fprintf(stderr, "Parse error\n");
    }
}
