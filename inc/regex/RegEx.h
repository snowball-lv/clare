#pragma once

#include <stdio.h>


typedef struct RegEx RegEx;

RegEx *RegExCompile(const char *regex);
int RegExMatchStream(RegEx *cr, FILE *input);
