#include <backends/i386/i386.new.h>

#include <helpers/Unused.h>

static void _Select(PAsmModule *mod, IRFunction *func) {
    UNUSED(mod);
    UNUSED(func);
}

Backend i386_Backend = {
    .dummy = 0,
    .Select = _Select,
};
