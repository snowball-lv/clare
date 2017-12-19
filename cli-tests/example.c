

#include <stdio.h>

extern int clare_fib(int n);

int main(int argc, char **argv) {
    printf("clare_fib(0): %d\n", clare_fib(0));
    printf("clare_fib(1): %d\n", clare_fib(1));
    printf("clare_fib(2): %d\n", clare_fib(2));
    printf("clare_fib(3): %d\n", clare_fib(3));
    printf("clare_fib(4): %d\n", clare_fib(4));
    return 0;
}
