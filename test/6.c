int switch_test(int a, int b)
{
    int c;
    int d;
    c = a + b;
    switch (c)
    {
    case 1: d = 1;
    case 2: d = 2;
    default: d = 3;
    }
    return d;
}
