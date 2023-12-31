1. Refactor `parse_stmt` using function array?
2. Add line number to token, improve error reporting

Unimplemented syntax:
- Declaration
    1. Storage class specifier: `auto`, `register`, `static`, `extern`
    2. Type specifier: `struct`, `union`, `enum`, `typedef`
    3. Type qualifier: `const`, `volatile`
    4. Multiple declaration in single "statement": `int a, b, c, ...`
    5. Initializer in declaration: `int a = 1`
    6. Variadic function declaration: `int printf(const char *s, ...)`
    7. Abstract declarator (`int f(int *, char)`)
- Statements
    1. Labeled statement: label, `case`, `default`
    2. Selection statement: `switch`
    3. Iteration statement: `do`
    4. Jump statement: `goto`, `continue`, `break`
- Expression
    1. Comma operator
    2. Compound assignment operators
    3. Cast expression
    4. `sizeof` operator
    5. Unary operators: `&`, `*`
    6. Postfix operators: `.`, `->`, `++`, `--`
    7. Constants: `char`, `float`, `enum`
