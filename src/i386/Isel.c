#include <i386/Isel.h>

#include <mem/Mem.h>
#include <helpers/Unused.h>

struct Isel {
    int dummy;
};

Isel *NewIsel() {
    Isel *isel = ALLOC(Isel);
    isel->dummy = 0;
    return isel;
}

void DeleteIsel(Isel *isel) {
    MemFree(isel);
}

void IselSelect(Node *root) {
    UNUSED(root);
}
