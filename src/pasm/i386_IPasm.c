#include <pasm/i386_IPasm.h>

static const char *version() {
    return "i386-pasm-dev";
}

IPasm i386_IPasm = {
    .version = version
};
