void foo();
void bar();
int goto_test() {
my_label:
    foo();
    bar();
    goto 3;
}
