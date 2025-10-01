#include "opentrackclient_posix.h"


// Initializer.
__attribute__((constructor))
static void initializer(void) {
    OTRInit();
}

// Finalizer.
__attribute__((destructor))
static void finalizer(void) {
    OTRClose();
}
