#pragma once

#include "api/plugin-api.hpp"
#include "compat/shm.h"

// We share the same memory-layout used by the wine-proto
#include "proto-wine/wine-shm.h"

#include "ui_ftnoir_shmcontrols.h"

#include "options/options.hpp"
using namespace options;

#include <QMutex>
#include <QProcess>
#include <QString>
#include <QVariant>

#include <QDebug>

struct settings : opts
{
    settings() : opts{"proto-native-shm"} {}

};

class native_shm : TR, public IProtocol
{
    Q_OBJECT

public:
    native_shm();
    ~native_shm() override;

    module_status initialize() override;
    void pose(const double* headpose, const double*) override;

    QString game_name() override
    {
        QMutexLocker foo(&game_name_mutex);
        return connected_game;
    }
private:
    shm_wrapper lck_shm { WINE_SHM_NAME, WINE_MTX_NAME, sizeof(WineSHM) };
    WineSHM* shm = nullptr;
    settings s;
    int gameid = 0;
    QString connected_game;
    QMutex game_name_mutex;
};

class FTControls: public IProtocolDialog
{
    Q_OBJECT

public:
    FTControls();
    void register_protocol(IProtocol *) override {}
    void unregister_protocol() override {}

private:
    Ui::UICFTControls ui;
    settings s;

private slots:
    void doOK();
    void doCancel();
};

class wine_metadata : public Metadata
{
    Q_OBJECT

public:
    QString name() override { return tr("Opentrack Shared Memory"); }
    QIcon icon() override { return QIcon(":/images/opentrack.png"); }
};
