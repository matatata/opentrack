#include "ftnoir_protocol_ftx.h"
#include <QString>
#include <QDebug>

#include "csv/csv.h"

freetrackx::freetrackx() = default;

freetrackx::~freetrackx() = default;

void freetrackx::pose(const double *headpose, const double*)
{
    static constexpr double d2r = M_PI/180;
    if (shm)
    {
        shm->data.Yaw = - headpose[Yaw] * d2r;
        shm->data.Pitch = - headpose[Pitch] * d2r;
        shm->data.Roll = headpose[Roll] * d2r;

        shm->data.X = headpose[TX] * 10;
        shm->data.Y = headpose[TY] * 10;
        shm->data.Z = headpose[TZ] * 10;

        if (shm->gameid != gameid)
        {
            //qDebug() << "proto/wine: looking up gameData";
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
