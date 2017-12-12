

#include <stdio.h>

extern int clare_main();
extern int clare_main_2();

int main(int argc, char **argv) {
    printf("clare_main: %d\n", clare_main());
    printf("clare_main_2: %d\n", clare_main_2());
    // printf("clare_mainf: %f\n", clare_mainf());
    return 0;
}
