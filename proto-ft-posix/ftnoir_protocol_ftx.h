#pragma once

#include "api/plugin-api.hpp"
#include "compat/shm.hpp"

// no we have our own now:
#define OTR_REQUIRED
#include "opentrackclient/opentrackclient-private.h"

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
    shm_wrapper lck_shm { OTR_SHM_NAME, OTR_MTX_NAME, sizeof(OTRPosixSHM) };
    OTRPosixSHM* shm = nullptr;
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
    QString name() override { return tr("opentrack 2.0 Enhanced"); }
    QIcon icon() override { return QIcon(":/images/freetrack.png"); }
};
