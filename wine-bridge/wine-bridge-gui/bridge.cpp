

#include "freetrackclient/fttypes.h"
#include <stdio.h>
#include <iostream>
#include <windows.h>


#include "bridge.h"
#include "defs.h"
#include "compat/shm.c"
#include "compat/shm.cpp"

using namespace std;


Bridge::Bridge(int fps,Log* logger, void (*callback)(bridge_status_t)):
    frame_time(1000/fps),logger(logger),callback(callback)
{
    loadOtrClient();
}

Bridge::~Bridge(){
    unloadOtrClient();
}

bool Bridge::setFPS(int fps) {
    if(fps <= 0 || fps > 80) {
        logger->logln("Bad FPS value: " + to_string(fps));
        return false;
    }


    int old = frame_time;
    frame_time = 1000/fps;

    if(old != frame_time) {
        logger->logln("FPS changed to " + to_string(fps));
    }

    return true;

}

int Bridge::getFPS() {
    return 1000/frame_time;
}


bool Bridge::loadOtrClient(){
    string dllpath = WINEBRIDGE_BIN_PATH;
    dllpath += "\\";
    dllpath += OTRCLIENT_DLLNAME;

    logger->logln(string("Loading ") + dllpath);

    otrclientDll = LoadLibrary(dllpath.c_str());
    if (otrclientDll) {
        getFTDataFunc = (f_otr_GetFTData)GetProcAddress(otrclientDll, OTR_GET_FT_HEAP_FUNC_NAME);
    }
    else {
         logger->logln(string("Could not LoadLibrary ") + dllpath);
         return false;
    }

    if(!getFTDataFunc)
    {
         logger->logln(string("Could not get ProcAddress ")  + OTR_GET_FT_HEAP_FUNC_NAME);
         return false;
    }


    return true;
}

void Bridge::unloadOtrClient(){
    if(otrclientDll){
        // logger->logln(string("Unloading ") + OTRCLIENT_DLLNAME);
        FreeLibrary(otrclientDll);
        getFTDataFunc = NULL;
        otrclientDll = NULL;
    }
}

DWORD WINAPI ThreadFunc(void* data) {
    Bridge *bridge = (Bridge*)data;

    bridge->run();

    return 0;
}

void Bridge::run(){

    if(getFTDataFunc==NULL)
        return;



    FTHeap *ftheap = (FTHeap*)freetrack_shm.ptr();
    bool stopped = false;
    while(running) {
        int tracking = 0;

        Sleep(frame_time);

        tracking = getFTDataFunc(ftheap);

        if(!tracking && !stopped){
            logger->logln("Tracking stopped");
            stopped = true;
            Sleep(1000);
        }

        if(stopped && tracking) {
            logger->logln("Tracking continued");
            stopped = false;
        }


        // std::cout <<
        //     ftheap->data.X << " " <<
        //     ftheap->data.Y << " " <<
        //     ftheap->data.Z << " " <<
        //     ftheap->data.Yaw << " " <<
        //     ftheap->data.Pitch << " " <<
        //     ftheap->data.Roll << endl;
    }

}

void Bridge::runAsync(){
    running = true;

    if(thread!=NULL){
        logger->logln("Still running aborting");
        return;
    }

    thread = CreateThread(NULL, 0, ThreadFunc, this, 0, NULL);
    if (!thread) {
        running = false;
        logger->logln("Could not create thread");
        callback(bridge_failed);
    }
    else {
        logger->logln("Bridge started");
        callback(bridge_started);
    }
}

 void Bridge::stop(){
    running = false;

    if(thread) {
        WaitForSingleObject(thread,INFINITE);
        CloseHandle(thread);
        thread = NULL;
        logger->logln("Bridge stopped");
    }

    callback(bridge_stopped);
 }
