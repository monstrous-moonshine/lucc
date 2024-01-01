int switch_test(int a, int b)
{
    int c;
    int d;
    c = a + b;
    switch (c)
    {
    case 1: d = 1; break;
    case 2: d = 2; break;
    default: d = 3; break;
    }
    for(;;) {
        if (d--) continue;
    }
    return d;
}
