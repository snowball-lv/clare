

#include <stdio.h>

extern int clare_foo();

int main(int argc, char **argv) {
    printf("clare_foo: %d\n", clare_foo());
    return 0;
}
