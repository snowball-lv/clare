#pragma once

#include <stdio.h>


int RegExMatchStream(const char *regex, FILE *input);

typedef struct RegEx RegEx;

RegEx *RegExCompile(const char *regex);
int RegExMatchStreamNew(RegEx *cr, FILE *input);
