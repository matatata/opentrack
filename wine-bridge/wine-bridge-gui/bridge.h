
#pragma once

#include <windows.h>

#include "log.h"

#define OTR_GENERIC_EXPORT
#define OTR_GENERIC_IMPORT
#include "compat/shm.hpp"
#include "wine-bridge/wine-builtin-dlls/otrclient.h"
#include "freetrackclient/fttypes.h"

typedef enum bridge_status_t{
    bridge_started = 0,
    bridge_stopped,
    bridge_failed
} bridge_status_t;

class Bridge {

    public:
        Bridge(int fps,Log* logger, void (*callback)(bridge_status_t));

        virtual ~Bridge();

        void runAsync();
        void run();
        void stop();

        bool setFPS(int fps);
        int getFPS();
    private:

        bool loadOtrClient();
        void unloadOtrClient();

        void (*callback)(bridge_status_t) = NULL;

        volatile bool running = false;
        volatile int frame_time = 1000/25;

        HINSTANCE otrclientDll = NULL;
        f_otr_GetFTData getFTDataFunc = NULL;

        HANDLE thread = NULL;

        Log* logger = NULL;
        shm_wrapper freetrack_shm { FREETRACK_HEAP, FREETRACK_MUTEX, sizeof(FTHeap) };

};

