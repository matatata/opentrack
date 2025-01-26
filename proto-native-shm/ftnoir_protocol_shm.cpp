#include "ftnoir_protocol_shm.h"
#include <QString>
#include <QDebug>

#include "csv/csv.h"

native_shm::native_shm() = default;

native_shm::~native_shm()
{
    //shm_unlink("/" WINE_SHM_NAME);
}

void native_shm::pose(const double *headpose, const double*)
{
    static constexpr double d2r = M_PI/180;
    if (shm)
    {
        lck_shm.lock();
        for (int i = 3; i < 6; i++)
            shm->data[i] = headpose[i] * d2r;
        for (int i = 0; i < 3; i++)
            shm->data[i] = headpose[i] * 10;

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

module_status native_shm::initialize()
{

    if (lck_shm.success())
    {
        shm = (WineSHM*) lck_shm.ptr();
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

OPENTRACK_DECLARE_PROTOCOL(native_shm, FTControls, wine_metadata)
