

#include <stdio.h>

extern int clare_main(int x, int y);
// extern int clare_main_2();

int main(int argc, char **argv) {
    printf("clare_main: %d\n", clare_main(3, 8));
    // printf("clare_main_2: %d\n", clare_main_2());
    // printf("clare_mainf: %f\n", clare_mainf());
    return 0;
}
