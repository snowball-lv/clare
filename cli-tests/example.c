

#include <stdio.h>

extern int clare_main();
extern float clare_mainf();

int main(int argc, char **argv) {
    printf("clare_main: %d\n", clare_main());
    printf("clare_mainf: %f\n", clare_mainf());
    return 0;
}
