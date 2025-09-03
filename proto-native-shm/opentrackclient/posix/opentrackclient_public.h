#ifndef OTR_CLIENT_POSIX_H
#define OTR_CLIENT_POSIX_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif

// angles are in radians
typedef struct OTRHeadpose {
    float  Yaw; // positive yaw to the right
    float  Pitch; // positive pitch up
    float  Roll; // positive roll right
    float  X; // positive move right
    float  Y; // positive move up
    float  Z; // positive move away
} OTRHeadpose;

// Call OTRInit() before using any other function in this library.
// return true on success, false on failure.
bool OTRInit();

// Get the latest headpose data. Returns true on success, false on failure.
bool OTRGetHeadpose(OTRHeadpose* headpose);

// Call OTRClose() when done using the library.
void OTRClose();

#ifdef __cplusplus
}
#endif

#endif
