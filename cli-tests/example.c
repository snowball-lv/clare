

#include <stdio.h>

extern int clare_fib(int n);

int main(int argc, char **argv) {
    for (int i = 0; i < 10; i++) {
        printf("clare_fib(%d): %d\n", i, clare_fib(i));
    }
    return 0;
}
