1. Refactor `parse_stmt` using function array?
2. Add line number to token, improve error reporting
3. Implement missing keywords or flag them, don't silently treat them
   as identifiers

Unimplemented syntax:
- Declaration
    1. Storage class specifier: `auto`, `register`, `static`, `extern`, `typedef`
    2. Type specifier: `struct`, `union`, `enum`, `typedef_name`
    3. Type qualifier: `const`, `volatile`
    4. Multiple declaration in single "statement": `int a, b, c, ...`
    5. Initializer in declaration: `int a = 1`
    6. Abstract declarator: `int f(int *, char)`
- Statements
    1. Labeled statement: label
    2. Jump statement: `goto`
- Expression
    1. Comma operator
    2. Compound assignment operators
    3. Cast expression
    4. `sizeof` operator
    5. Postfix operators: `.`, `->`
    6. Constants: `char_const`, `float_const`, `enum_const`
