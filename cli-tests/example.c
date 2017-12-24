

#include <stdio.h>

extern int clare_fib(int n);
extern float clare_fibf(float n);

int main(int argc, char **argv) {
    
    for (int i = 0; i < 10; i++) {
        printf("clare_fib(%d): %d\n", i, clare_fib(i));
    }
    
    for (int i = 0; i < 10; i++) {
        printf("clare_fibf(%i): %f\n", i, clare_fibf(i));
    }
    
    return 0;
}
