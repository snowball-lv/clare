

#include <stdio.h>

extern int clare_main();

int main(int argc, char **argv) {
    printf("clare_foo: %d\n", clare_main());
    return 0;
}
