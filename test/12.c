int printf(char *s, ...);
int main() {
    int i;
    switch (i)
    begin:
        if (i < 10) {
        case 0:
            printf("I'm here (%d)!\n", i++);
            goto begin;
        }
}
