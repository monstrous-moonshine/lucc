int printf(char *s, ...);
void for_test() {
    int i;
    int *j;
    j = &i;
    for (i = 0; i < 10; i++) {
        printf("%d\n", i);
        printf("%d\n", *j);
    }
}
