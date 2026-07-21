/* Homepage         http://facetracknoir.sourceforge.net/home/default.htm        *
 *                                                                               *
 * ISC License (ISC)                                                             *
 *                                                                               *
 * Copyright (c) 2015, Wim Vriend                                                *
 *                                                                               *
 * Permission to use, copy, modify, and/or distribute this software for any      *
 * purpose with or without fee is hereby granted, provided that the above        *
 * copyright notice and this permission notice appear in all copies.             *
 */
#pragma once

#include "ui_ftnoir_ftncontrols.h"
#include "api/plugin-api.hpp"
#include "options/options.hpp"

#include <QDir>
#include <QHostAddress>
#include <QMutex>
#include <QObject>
#include <QString>
#include <QUdpSocket>

using namespace options;

struct settings : opts
{
    value<int> ip1, ip2, ip3, ip4, port;
    value<QString> bottles_root;
    value<QString> selected_bottle;

    settings() :
        opts("crossover-trackir-proto"),
        ip1(b, "ip1", 127),
        ip2(b, "ip2", 0),
        ip3(b, "ip3", 0),
        ip4(b, "ip4", 1),
        port(b, "port", 4242),
        bottles_root(
            b,
            "bottles-root",
            QDir::homePath() + "/Library/Application Support/CrossOver/Bottles"
        ),
        selected_bottle(
            b,
            "selected-bottle",
            ""
        )
    {}
};

class crossover_trackir : public QObject, public IProtocol
{
    Q_OBJECT

public:
    crossover_trackir();
    module_status initialize() override;
    void pose(const double* headpose, const double*) override;
    QString game_name() override { return tr("CrossOver TrackIR Bridge"); }

private:
    QUdpSocket outSocket;
    settings s;
    mutable QMutex lock;
    QHostAddress dest_ip { 127u << 24 | 1u };
    unsigned short dest_port = 65535;

private slots:
    void set_dest_address();
};

class CrossoverTrackIRControls : public IProtocolDialog
{
    Q_OBJECT

public:
    CrossoverTrackIRControls();
    void register_protocol(IProtocol*) override {}
    void unregister_protocol() override {}

private:
    Ui::UICrossoverTrackIRControls ui;
    settings s;

private slots:
    void doOK();
    void doCancel();
    void chooseBottlesRoot();
    void refreshBottleList();
    void installBridge();
    void updateInstallationStatus();
};

class crossover_trackir_dll : public Metadata
{
    Q_OBJECT

    QString name() override { return tr("CrossOver TrackIR Bridge"); }
    QIcon icon() override { return QIcon(":/images/opentrack.png"); }
};
