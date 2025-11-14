/* Copyright (c) 2025 Matteo Ceruti <matteo.ceruti@gmail.com>
 * https://github.com/matatata/opentrack
 *
 * This Opentrack[1]-Headtracking Client API aims to be a portable interface in order to
 * bring native Headtracking to macOS and Linux. It is heavily inspired and in parts based
 * on the Windows-only FreeTrackNoIR[2] and FreeTrack 2.0.
 *
 * I invite developers to integrate it into their macOS and/or Linux software.
 * Technically the interface and the implementation provided by opentrack could
 * be used on Windows as well, but we've got FT and other well established
 * solutions there already.
 *
 * [1] Based on and made for opentrack by Stanislaw Halik
 * Copyright (c) 2013 Stanislaw Halik <sthalik@misaki.pl>
 *
 * [2] FreeTrackNoIR:
 * * Wim Vriend (Developing)                                                        *
 * * Ron Hendriks (Testing and Research)                                            *
 * *                                                                                *
 * * Homepage               <http://facetracknoir.sourceforge.net/home/default.htm> *
 * *                                                                                *
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 */

#ifndef OPENTRACKCLIENT_H
#define OPENTRACKCLIENT_H

#include <stdbool.h>


#ifdef __cplusplus
extern "C"
{
#endif


// Headpose structure
// Depending on whether you use OTRGetData or OTRGetNPData the semantics of the fields differ.
// OTRGetData: the angles are in radians, opentrack's translation values scaled by 10
// OTRGetNPData: the angles are opentrack's degrees scaled by (16383/180), openbtrack's translation values scaled by 16383/50
typedef struct OTRHeadpose {
    float  Yaw; // positive yaw to the left
    float  Pitch; // positive pitch up
    float  Roll; // positive roll to the left
    float  X; // positive move right
    float  Y; // positive move up
    float  Z; // positive move away
} OTRHeadpose;


#if ! defined(OTR_OPTIONAL) && ! defined(OTR_REQUIRED)
// #ifndef __APPLE__
#error "Plase define OTR_REQUIRED if you require the opentrackclient library to be present and link against it statically or dynamically. Otherwise define OTR_OPTIONAL and link against the static library opentrackclient-loader on Linux or use -weak_framework opentrackclient on macOS."
// #else
// #define OTR_OPTIONAL
// #endif
#endif

#if defined(OTR_OPTIONAL) && ! defined(__APPLE__)

extern bool (*importedOTRGetData)(OTRHeadpose*);
extern bool (*importedOTRGetNPData)(OTRHeadpose*);
extern const char* (*importedOTRGetVersion)(void);
extern const char* (*importedOTRGetProvider)(void);

#define OTRGetData importedOTRGetData
#define OTRGetNPData importedOTRGetNPData
#define OTRGetVersion importedOTRGetVersion
#define OTRGetProvider importedOTRGetProvider

#else

#if defined(OTR_OPTIONAL) && defined(__APPLE__)
#define OTR_IMPORT __attribute__((weak_import))
#else
#define OTR_IMPORT
#endif

// Get the latest headpose data. Returns true on success, false on failure.
// This should be compatible with FreeTrackNoIR. So if your game already inplements FreeTrack in windows use this for macOS/Linux
extern bool OTRGetData(OTRHeadpose* headpose) OTR_IMPORT ;
// Fills OTRHeadpose in an alternative way. It may better fit your needs. See OTRHeadpose's description.
extern bool OTRGetNPData(OTRHeadpose* data) OTR_IMPORT;
// Get version string
extern const char* OTRGetVersion() OTR_IMPORT;
// Get implementation provider
extern const char* OTRGetProvider() OTR_IMPORT;

#endif


#ifdef __cplusplus
}
#endif

#endif // OPENTRACKCLIENT_H
