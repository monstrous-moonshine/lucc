int printf(char *s, int i);
void for_test() {
    int i;
    for (i = 0; i < 10; i = i + 1) {
        printf("%d\n", i);
    }
}
