#include <clare/dummy/dummy.h>

#include <assert.h>
#include <string.h>

int main() {

    const char *v = dummy_version();
    assert(strcmp(v, "dummy-dev") == 0);

    return 0;
}
