1. ~~Refactor `parse_stmt` using function array?~~
2. Add line number to token, improve error reporting
3. ~~Implement missing keywords or flag them, don't silently treat them
   as identifiers~~
4. Add tests, lot more

Unimplemented syntax:
- Declaration
    1. Storage class specifier: `auto`, `register`, `static`, `extern`, `typedef`
    2. Type specifier: `struct`, `union`, `enum`, `typedef_name`, combination
       of basic types (e.g., `unsigned char`)
    3. Type qualifier: `const`, `volatile`
    4. Compound initializer: `int a[] = { ... }`
    5. Abstract declarator: `int f(int *, char)`
- Expression
    1. Comma operator
    2. Compound assignment operators
    3. Check if target of assignment is unary expression
    4. Cast expression
    5. `sizeof` operator
    6. Postfix operators: `.`, `->`
    7. Constants: `char_const`, `float_const`, `enum_const`
