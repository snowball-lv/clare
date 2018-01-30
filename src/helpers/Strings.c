#include <clare/helpers/Strings.h>

#include <stddef.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdio.h>

#include <clare/mem/Mem.h>

const char *ToString(const char *format, ...) {
    va_list args;
    va_start(args, format);
    int size = vsnprintf(0, 0, format, args);
    char *buffer = MemAlloc(size + 1);
    vsprintf(buffer, format, args);
    va_end(args);
    return buffer;
}
