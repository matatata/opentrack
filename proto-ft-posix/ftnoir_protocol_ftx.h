#pragma once

#include "api/plugin-api.hpp"
#include "compat/shm.h"

// no we have our own now:
#include "freetrackclient_posix_private.h"

#include "ui_ftnoir_ftxcontrols.h"

#include "options/options.hpp"
using namespace options;

#include <QMutex>
#include <QProcess>
#include <QString>
#include <QVariant>

#include <QDebug>

struct settings : opts
{
    settings() : opts{"proto-freetrack-posix"} {}

};

class freetrackx : TR, public IProtocol
{
    Q_OBJECT

public:
    freetrackx();
    ~freetrackx() override;

    module_status initialize() override;
    void pose(const double* headpose, const double*) override;

    QString game_name() override
    {
        QMutexLocker foo(&game_name_mutex);
        return connected_game;
    }
private:
    shm_wrapper lck_shm { FTX_SHM_NAME, FTX_MTX_NAME, sizeof(FTPosixSHM) };
    FTPosixSHM* shm = nullptr;
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

class ftx_metadata : public Metadata
{
    Q_OBJECT

public:
    QString name() override { return tr("freetrack 2.0 Posix"); }
    QIcon icon() override { return QIcon(":/images/freetrack.png"); }
};
