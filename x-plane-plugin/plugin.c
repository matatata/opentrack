/* Copyright (c) 2013, Stanislaw Halik <sthalik@misaki.pl>

 * Permission to use, copy, modify, and/or distribute this
 * software for any purpose with or without fee is hereby granted,
 * provided that the above copyright notice and this permission
 * notice appear in all copies.
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <sys/file.h>
#include <sys/mman.h>
#include <limits.h>
#include <unistd.h>
#include <math.h>

#include <XPLMPlugin.h>
#include <XPLMDataAccess.h>
#include <XPLMProcessing.h>
#include <XPLMUtilities.h>
#include <XPLMMenus.h>

#include "freetrackclient.h"

#ifndef PLUGIN_API
#define PLUGIN_API
#endif

#pragma GCC diagnostic ignored "-Wunused-parameter"

#define COMMAND_ID_TRACKING_TOGGLE "opentrack/toggle"
#define COMMAND_ID_TRANSLATION_TOGGLE "opentrack/toggle_translation"

#include "compat/linkage-macros.hpp"

#ifdef __GNUC__
#   pragma GCC diagnostic ignored "-Wimplicit-float-conversion"
#   pragma GCC diagnostic ignored "-Wdouble-promotion"
#   pragma GCC diagnostic ignored "-Wlanguage-extension-token"
#endif

static FTXData ftx_headpose = {0};

static void *view_x, *view_y, *view_z, *view_heading, *view_pitch, *view_roll;
static float offset_x, offset_y, offset_z;
static void *xcam_mode, *xcam_ht_on, *xcam_offset_h, *xcam_offset_p, *xcam_offset_r, *xcam_offset_x, *xcam_offset_y, *xcam_offset_z;
static XPLMCommandRef track_toggle = NULL, translation_disable_toggle = NULL;
static XPLMDataRef StatusDataRef = NULL;
static bool track_status = false;
static int translation_disabled = 0; // 0 = translation is processed 1 = translation data ignored (you're able move around)
static int XCameraStatus = 0; // 0 = plugin not present, 1 = plugin present but output stopped, 2 = output started

XPLMMenuID g_menu_id;
int g_menu_container_idx;
int g_menu_tracking_idx;
int g_menu_translation_disable_idx;
int g_menu_xcam_status_idx;

void init_menu_items();
void update_menu_items();

#define DEBUG_STRINGS 0

/*
 * Debug strings
 */
static void debug_string(char *message)
{
    #if DEBUG_STRINGS
    XPLMDebugString(message);
    #endif
}

static void reinit_offset() {
    debug_string("reinit offsets\n");
    offset_x = XPLMGetDataf(view_x) -  ftx_headpose.X * 1e-3 ;
    offset_y = XPLMGetDataf(view_y) -  ftx_headpose.Y * 1e-3 ;
    offset_z = XPLMGetDataf(view_z) -  ftx_headpose.Z * 1e-3 ;
}

void GetHeadpose(){
    FTXGetData(&ftx_headpose);
    ftx_headpose.Pitch *= -1;
    ftx_headpose.Yaw *= -1;
    ftx_headpose.X *= -1;
}

float write_head_position(float inElapsedSinceLastCall,
                          float inElapsedTimeSinceLastFlightLoop,
                          int   inCounter,
                          void* inRefcon)
{
    GetHeadpose();

    if (translation_disabled==0)
    {
        if (XCameraStatus < 2) {
            XPLMSetDataf(view_x, ftx_headpose.X * 1e-3 + offset_x);
            XPLMSetDataf(view_y, ftx_headpose.Y * 1e-3 + offset_y);
            XPLMSetDataf(view_z, ftx_headpose.Z * 1e-3 + offset_z);
        } else {
            if (XPLMGetDatai(xcam_mode) == 2) {
                XPLMSetDataf(xcam_offset_x, ftx_headpose.X * 1e-3);
                XPLMSetDataf(xcam_offset_y, ftx_headpose.Y * 1e-3);
                XPLMSetDataf(xcam_offset_z, ftx_headpose.Z * 1e-3);
            }
        }
    }
    if (XCameraStatus < 2) {
        XPLMSetDataf(view_heading, ftx_headpose.Yaw * 180 / M_PI);
        XPLMSetDataf(view_pitch, ftx_headpose.Pitch * 180 / M_PI);
        XPLMSetDataf(view_roll, ftx_headpose.Roll * 180 / M_PI);

    } else {
        if (XPLMGetDatai(xcam_mode) == 2) {
            XPLMSetDataf(xcam_offset_h, ftx_headpose.Yaw * 180 / M_PI);
            XPLMSetDataf(xcam_offset_p, ftx_headpose.Pitch * 180 / M_PI);
            XPLMSetDataf(xcam_offset_r, ftx_headpose.Roll * 180 / M_PI);
        }
    }


    return -1.0;
}

/*
 * X-Camera initialization
 */
 static void Xcam_init()
 {
    debug_string("Opentrack: X-Camera init\n");

    xcam_mode = XPLMFindDataRef("SRS/X-Camera/integration/X-Camera_enabled"); //2 = tir mode 1 = enabled
    xcam_ht_on = XPLMFindDataRef("SRS/X-Camera/integration/headtracking_present");
    xcam_offset_h = XPLMFindDataRef("SRS/X-Camera/integration/headtracking_heading_offset");
    xcam_offset_p = XPLMFindDataRef("SRS/X-Camera/integration/headtracking_pitch_offset");
    xcam_offset_r = XPLMFindDataRef("SRS/X-Camera/integration/headtracking_roll_offset");
    xcam_offset_x = XPLMFindDataRef("SRS/X-Camera/integration/headtracking_x_offset");
    xcam_offset_y = XPLMFindDataRef("SRS/X-Camera/integration/headtracking_y_offset");
    xcam_offset_z = XPLMFindDataRef("SRS/X-Camera/integration/headtracking_z_offset");

    if (xcam_mode && xcam_ht_on && xcam_offset_h && xcam_offset_p && xcam_offset_r && xcam_offset_x && xcam_offset_y && xcam_offset_z) {

        if(XPLMGetDatai(xcam_mode) != 0){
            XCameraStatus = 2;
            view_x = view_y = view_z = 0;
            debug_string("Opentrack: X-Camera is enabled. Redirecting output.\n");
        }
        else
        {
            XCameraStatus = 1;
            debug_string("Opentrack: X-Camera is disabled.\n");
        }

        XPLMSetDatai(xcam_ht_on,1);
    }
 }

 static void Xcam_deinit()
 {
    XCameraStatus = 1;
    XPLMSetDatai(xcam_ht_on,0);
    debug_string("Opentrack: X-Camera deinit\n");
 }

/*
 * isntall/uninstalls the Flight loop callback
 */
static void setTrackingStatus(bool tracking_enabled)
{

    if((track_status == 1) == tracking_enabled)
        return; // already in the right status

    if (tracking_enabled) { // tracking has been turned on
        debug_string("Opentrack: Enabled tracking\n");
        if(XCameraStatus != 2) // if X-Camera out is not active take current position as new offset
        {
            reinit_offset();
        }

        if (XCameraStatus == 1) { // X-Camera was off try to enable this time
            Xcam_init();
        }
        XPLMRegisterFlightLoopCallback(write_head_position, -1, NULL);
        track_status = 1;
    }
    else { // tracking has been turned off
        debug_string("Opentrack: Disabled tracking\n");
        if (XCameraStatus > 1) {
            Xcam_deinit();
        }

        XPLMUnregisterFlightLoopCallback(write_head_position, NULL);
        track_status = 0;
    }

    update_menu_items();
}

static void stopTracking()
{
    setTrackingStatus(false);
}

static void startTracking()
{
    setTrackingStatus(true);
}

/*
 *Command Handlers
 */

static int TrackToggleHandler(XPLMCommandRef inCommand,
                              XPLMCommandPhase inPhase,
                              void* inRefCon)
{
    if (inPhase == xplm_CommandBegin) {
        if (track_status == 0) {
            startTracking();
        }
        else if (track_status == 1) {
            stopTracking();
        }
    }
    return 0;
}


static int TranslationToggleHandler(XPLMCommandRef inCommand,
                                    XPLMCommandPhase inPhase,
                                    void* inRefCon)
{
    if (inPhase != xplm_CommandBegin) {
        return 0;
    }

    if (translation_disabled) {
        translation_disabled = 0;
        debug_string("Opentrack: Translations enabled\n");
        // Reinit the offsets when we re-enable the translations so that we can "move around"
        reinit_offset();
    }
    else {
        translation_disabled = 1;
        debug_string("Opentrack: Translation disabled\n");
    }

    update_menu_items();
    return 0;
}

/* Dataref Handlers */

static int getTrackingStatusCallback(void * inRefcon)
{
    return track_status;
}

static void setTrackingStatusCallback(void * inRefcon, int inValue)
{
    setTrackingStatus(inValue!=0);
}

static inline
void volatile_explicit_bzero(void volatile* restrict ptr, size_t len)
{
    for (size_t i = 0; i < len; i++)
        *((char volatile* restrict)ptr + i) = 0;

    asm volatile("" ::: "memory");
}

void init_menu_items()
{
    g_menu_container_idx = XPLMAppendMenuItem(XPLMFindPluginsMenu(), "Opentrack", 0, 0);
    g_menu_id = XPLMCreateMenu("Opentrack", XPLMFindPluginsMenu(), g_menu_container_idx, NULL/*menu_handler*/, NULL);
    g_menu_tracking_idx = XPLMAppendMenuItemWithCommand(g_menu_id,"Head tracking",XPLMFindCommand(COMMAND_ID_TRACKING_TOGGLE));
    g_menu_translation_disable_idx = XPLMAppendMenuItemWithCommand(g_menu_id,"Input translation",XPLMFindCommand(COMMAND_ID_TRANSLATION_TOGGLE));

    XPLMAppendMenuSeparator(g_menu_id);
    g_menu_xcam_status_idx = XPLMAppendMenuItem(g_menu_id, "X-Camera Status", NULL, 1);

    XPLMEnableMenuItem(g_menu_id,g_menu_xcam_status_idx,0); // disable menu
    update_menu_items();
}


void update_menu_items(){
    XPLMCheckMenuItem( g_menu_id,g_menu_tracking_idx,track_status == 1 ? xplm_Menu_Checked : xplm_Menu_Unchecked);
    XPLMCheckMenuItem( g_menu_id,g_menu_translation_disable_idx,translation_disabled == 0 ? xplm_Menu_Checked : xplm_Menu_Unchecked );

    const char *xcam_satus_text = "Output: Off";
    if(track_status) {
        if(XCameraStatus==2)
            xcam_satus_text = "Output: X-Camera";
        else
            xcam_satus_text = "Output: X-Plane";
    }
    XPLMSetMenuItemName( g_menu_id,g_menu_xcam_status_idx,xcam_satus_text,1);
}

PLUGIN_API OTR_GENERIC_EXPORT
int XPluginStart (char* outName, char* outSignature, char* outDescription) {

    strcpy(outName, "opentrack");
    strcpy(outSignature, "opentrack - freetrack lives!");
    strcpy(outDescription, "head tracking view control");

    view_x = XPLMFindDataRef("sim/graphics/view/pilots_head_x");
    view_y = XPLMFindDataRef("sim/graphics/view/pilots_head_y");
    view_z = XPLMFindDataRef("sim/graphics/view/pilots_head_z");
    view_heading = XPLMFindDataRef("sim/graphics/view/pilots_head_psi");
    view_pitch = XPLMFindDataRef("sim/graphics/view/pilots_head_the");
    view_roll = XPLMFindDataRef("sim/graphics/view/pilots_head_phi");

    track_toggle = XPLMCreateCommand(COMMAND_ID_TRACKING_TOGGLE, "Disable/Enable head tracking");
    translation_disable_toggle = XPLMCreateCommand(COMMAND_ID_TRANSLATION_TOGGLE, "Disable/Enable input translation from opentrack");

    XPLMRegisterCommandHandler(track_toggle,
                               TrackToggleHandler,
                               1,
                               NULL);

    XPLMRegisterCommandHandler(translation_disable_toggle,
                               TranslationToggleHandler,
                               1,
                               NULL);

    // make current state available other plugins or tools (DateRefTool for instance) to read adn write
    StatusDataRef = XPLMRegisterDataAccessor(
                                "opentrack/tracking",
                                xplmType_Int,								/* The types we support */
                                1,											/* Writable */
                                getTrackingStatusCallback, setTrackingStatusCallback,
                                NULL, NULL,									/* No accessors for floats */
                                NULL, NULL,									/* No accessors for doubles */
                                NULL, NULL,									/* No accessors for int arrays */
                                NULL, NULL,									/* No accessors for float arrays */
                                NULL, NULL,									/* No accessors for raw data */
                                NULL, NULL);								/* Refcons not used */

    init_menu_items();

    if (view_x && view_y && view_z && view_heading && view_pitch && track_toggle && translation_disable_toggle) {
        debug_string("opentrack: Plugin loaded\n");
        return 1;
    }
    return 0;
}

PLUGIN_API OTR_GENERIC_EXPORT
void XPluginStop (void) {

    if(track_status) {
        stopTracking();
    }

    if (StatusDataRef)
        XPLMUnregisterDataAccessor(StatusDataRef);
    if(translation_disable_toggle)
        XPLMUnregisterCommandHandler(translation_disable_toggle,TranslationToggleHandler,0,0);
    if(track_toggle)
        XPLMUnregisterCommandHandler(track_toggle,TrackToggleHandler,0,0);

    for(int i= 0 ; i<=g_menu_xcam_status_idx;i++)
        XPLMRemoveMenuItem(g_menu_id,i);

    XPLMDestroyMenu(g_menu_id);
    XPLMRemoveMenuItem(XPLMFindPluginsMenu(), g_menu_container_idx);
}

PLUGIN_API OTR_GENERIC_EXPORT
int XPluginEnable (void) {

    XPLMPluginID XCam = XPLMFindPluginBySignature("SRS.X-Camera");
    if (XCam != XPLM_NO_PLUGIN_ID) {
        debug_string("Opentrack: X-Camera plugin found. Enabling output.\n");
        XCameraStatus = 1;
    }

    update_menu_items();

    return 1;
}

PLUGIN_API OTR_GENERIC_EXPORT
void XPluginDisable (void) {
    if (XCameraStatus != 0) {
        XCameraStatus = 0;
        debug_string("Opentrack: Disabling X-Camera output mode\n");
    }

    stopTracking();
}

PLUGIN_API OTR_GENERIC_EXPORT
void XPluginReceiveMessage(XPLMPluginID    inFromWho,
                           int             inMessage,
                           void *          inParam)
{
    if (inMessage == XPLM_MSG_AIRPORT_LOADED)
        reinit_offset();
}
