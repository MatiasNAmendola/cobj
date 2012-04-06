#include <stdio.h>

int
fibo(int n)
{
    if (n == 0)
        return 0;
    if (n == 1)
        return 1;
    return fibo(n - 1) + fibo(n - 2);
}

int
main(int argc, const char **argv)
{
    int i;
    for (i = 0; i < 30; i++) {
        printf("%d\n", fibo(i));
    }
}
