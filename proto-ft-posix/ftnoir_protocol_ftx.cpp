#include "ftnoir_protocol_ftx.h"
#include <QString>
#include <QDebug>

#include "csv/csv.h"

freetrackx::freetrackx() = default;

freetrackx::~freetrackx() = default;

void freetrackx::pose(const double *headpose, const double*)
{
    if (shm)
    {
        lck_shm.lock();

        _OTR_SetSHM_Yaw(shm->data,headpose[Yaw]);
        _OTR_SetSHM_Pitch(shm->data,headpose[Pitch]);
        _OTR_SetSHM_Roll(shm->data,headpose[Roll]);
        _OTR_SetSHM_X(shm->data,headpose[TX]);
        _OTR_SetSHM_Y(shm->data,headpose[TY]);
        _OTR_SetSHM_Z(shm->data,headpose[TZ]);

        if (shm->gameid != gameid)
        {
            QString gamename;
            QMutexLocker foo(&game_name_mutex);
            /* only EZCA for FSX requires dummy process, and FSX doesn't work on Linux */
            /* memory-hacks DLL can't be loaded into a Linux process, either */
            getGameData(shm->gameid, shm->table, gamename);
            gameid = shm->gameid2 = shm->gameid;
            connected_game = gamename;
        }
        lck_shm.unlock();
    }
}

module_status freetrackx::initialize()
{

    if (lck_shm.success())
    {
        shm = (FTPosixSHM*) lck_shm.ptr();
        lck_shm.lock();
        memset(shm, 0, sizeof(*shm));
        lck_shm.unlock();
        qDebug() << "proto/native_shm: shm success";
        return status_ok();
    }
    else {
        qDebug() << "proto/native_shm: shm no success";
        return error(tr("Can't open shared memory mapping"));
    }
}

OPENTRACK_DECLARE_PROTOCOL(freetrackx, FTControls, ftx_metadata)
