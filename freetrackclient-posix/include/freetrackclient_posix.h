/* Copyright (c) 2025 Matteo Ceruti <matteo.ceruti@gmail.com>
 * https://github.com/matatata/opentrack
 *
 * Freetrack for Opentrack on Posix platforms inspired and based on FreeTrackNoIR:
 *
 * * Wim Vriend (Developing)                                                        *
 * * Ron Hendriks (Testing and Research)                                            *
 * *                                                                                *
 * * Homepage               <http://facetracknoir.sourceforge.net/home/default.htm> *
 * *                                                                                *
 *
 * This program is distributed in the hope that it will be useful, but            *
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY     *
 * or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 */
#ifndef FREETRACKCLIENT_POSIX_H
#define FREETRACKCLIENT_POSIX_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif


// angles are in radians
typedef struct FTXData {
    float  Yaw; // positive yaw to the left
    float  Pitch; // positive pitch up
    float  Roll; // positive roll to the left
    float  X; // positive move right
    float  Y; // positive move up
    float  Z; // positive move away
} FTXData;



// Get the latest headpose data. Returns true on success, false on failure.
bool FTXGetData(FTXData* headpose);

// Get Version string
const char* FTXGetVersion();

// Get Library Provider
const char* FTXGetProvider();

typedef bool (*importedFTXGetData)(FTXData*);

typedef const char* (*importedFTXGetVersion)(void);

typedef const char* (*importedFTXGetProvider)(void);


#ifdef __cplusplus
}
#endif

#endif
