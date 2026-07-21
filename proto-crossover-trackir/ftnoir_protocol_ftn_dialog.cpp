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
#include "ftnoir_protocol_ftn.h"
#include "api/plugin-api.hpp"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QProcess>
#include <QProcessEnvironment>

namespace
{
QString crossoverWinePath()
{
    return QStringLiteral(
        "/Applications/CrossOver.app/Contents/SharedSupport/CrossOver/"
        "CrossOver-Hosted Application/wine"
    );
}

QString bundledDllPath()
{
    QDir executableDirectory(QCoreApplication::applicationDirPath());

    return executableDirectory.absoluteFilePath(
        QStringLiteral("../Resources/crossover-trackir/NPClient64.dll")
    );
}

QString bottleDirectory(
    const QString& bottlesRoot,
    const QString& bottleName
)
{
    return QDir(bottlesRoot).absoluteFilePath(bottleName);
}

QString installedDllPath(
    const QString& bottlesRoot,
    const QString& bottleName
)
{
    return QDir(bottleDirectory(bottlesRoot, bottleName))
        .absoluteFilePath(QStringLiteral("drive_c/TrackIR/NPClient64.dll"));
}
}

CrossoverTrackIRControls::CrossoverTrackIRControls()
{
    ui.setupUi(this);

    s.ip1 = 127;
    s.ip2 = 0;
    s.ip3 = 0;
    s.ip4 = 1;
    s.port = 4242;

    tie_setting(s.ip1, ui.spinIPFirstNibble);
    tie_setting(s.ip2, ui.spinIPSecondNibble);
    tie_setting(s.ip3, ui.spinIPThirdNibble);
    tie_setting(s.ip4, ui.spinIPFourthNibble);
    tie_setting(s.port, ui.spinPortNumber);

    ui.editBottlesRoot->setText(s.bottles_root);

    connect(
        ui.buttonBox,
        &QDialogButtonBox::accepted,
        this,
        &CrossoverTrackIRControls::doOK
    );

    connect(
        ui.buttonBox,
        &QDialogButtonBox::rejected,
        this,
        &CrossoverTrackIRControls::doCancel
    );

    connect(
        ui.buttonChooseBottlesRoot,
        &QPushButton::clicked,
        this,
        &CrossoverTrackIRControls::chooseBottlesRoot
    );

    connect(
        ui.buttonRefreshBottles,
        &QPushButton::clicked,
        this,
        &CrossoverTrackIRControls::refreshBottleList
    );

    connect(
        ui.buttonInstallBridge,
        &QPushButton::clicked,
        this,
        &CrossoverTrackIRControls::installBridge
    );

    connect(
        ui.comboBottle,
        &QComboBox::currentTextChanged,
        this,
        [this](const QString&) {
            updateInstallationStatus();
        }
    );

    refreshBottleList();
}

void CrossoverTrackIRControls::doOK()
{
    s.bottles_root = ui.editBottlesRoot->text().trimmed();
    s.selected_bottle = ui.comboBottle->currentText().trimmed();

    s.b->save();
    close();
}

void CrossoverTrackIRControls::doCancel()
{
    close();
}

void CrossoverTrackIRControls::chooseBottlesRoot()
{
    const QString currentPath = ui.editBottlesRoot->text().trimmed();

    const QString selectedPath = QFileDialog::getExistingDirectory(
        this,
        tr("Choose the CrossOver bottles folder"),
        currentPath,
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
    );

    if (selectedPath.isEmpty())
        return;

    ui.editBottlesRoot->setText(QDir::cleanPath(selectedPath));
    refreshBottleList();
}

void CrossoverTrackIRControls::refreshBottleList()
{
    const QString bottlesRoot = ui.editBottlesRoot->text().trimmed();
    const QString previousSelection =
        ui.comboBottle->currentText().isEmpty()
            ? QString(s.selected_bottle)
            : ui.comboBottle->currentText();

    ui.comboBottle->clear();

    QDir rootDirectory(bottlesRoot);

    if (!rootDirectory.exists())
    {
        ui.labelInstallationStatus->setText(
            tr("Bottles folder not found.")
        );
        return;
    }

    const QFileInfoList bottleDirectories = rootDirectory.entryInfoList(
        QDir::Dirs | QDir::NoDotAndDotDot,
        QDir::Name | QDir::IgnoreCase
    );

    for (const QFileInfo& directory : bottleDirectories)
    {
        const QString driveCPath =
            QDir(directory.absoluteFilePath())
                .absoluteFilePath(QStringLiteral("drive_c"));

        if (QDir(driveCPath).exists())
            ui.comboBottle->addItem(directory.fileName());
    }

    const int previousIndex =
        ui.comboBottle->findText(previousSelection);

    if (previousIndex >= 0)
        ui.comboBottle->setCurrentIndex(previousIndex);
    else if (ui.comboBottle->count() > 0)
        ui.comboBottle->setCurrentIndex(0);

    updateInstallationStatus();
}

void CrossoverTrackIRControls::installBridge()
{
    const QString bottlesRoot =
        ui.editBottlesRoot->text().trimmed();

    const QString bottleName =
        ui.comboBottle->currentText().trimmed();

    if (bottlesRoot.isEmpty() || bottleName.isEmpty())
    {
        QMessageBox::warning(
            this,
            tr("CrossOver TrackIR Bridge"),
            tr("Select a valid bottles folder and a bottle.")
        );
        return;
    }

    const QString sourceDll = bundledDllPath();

    if (!QFileInfo::exists(sourceDll))
    {
        QMessageBox::critical(
            this,
            tr("CrossOver TrackIR Bridge"),
            tr("Bundled NPClient64.dll was not found:\n%1")
                .arg(sourceDll)
        );
        return;
    }

    const QString bottlePath =
        bottleDirectory(bottlesRoot, bottleName);

    if (!QDir(bottlePath).exists())
    {
        QMessageBox::critical(
            this,
            tr("CrossOver TrackIR Bridge"),
            tr("The selected bottle does not exist:\n%1")
                .arg(bottlePath)
        );
        return;
    }

    const QString targetDirectory =
        QDir(bottlePath).absoluteFilePath(
            QStringLiteral("drive_c/TrackIR")
        );

    if (!QDir().mkpath(targetDirectory))
    {
        QMessageBox::critical(
            this,
            tr("CrossOver TrackIR Bridge"),
            tr("Unable to create the TrackIR folder:\n%1")
                .arg(targetDirectory)
        );
        return;
    }

    const QString targetDll =
        QDir(targetDirectory).absoluteFilePath(
            QStringLiteral("NPClient64.dll")
        );

    if (QFileInfo::exists(targetDll) && !QFile::remove(targetDll))
    {
        QMessageBox::critical(
            this,
            tr("CrossOver TrackIR Bridge"),
            tr("Unable to replace the existing DLL:\n%1")
                .arg(targetDll)
        );
        return;
    }

    if (!QFile::copy(sourceDll, targetDll))
    {
        QMessageBox::critical(
            this,
            tr("CrossOver TrackIR Bridge"),
            tr("Unable to copy NPClient64.dll to:\n%1")
                .arg(targetDll)
        );
        return;
    }

    const QString wineExecutable = crossoverWinePath();

    if (!QFileInfo::exists(wineExecutable))
    {
        QMessageBox::critical(
            this,
            tr("CrossOver TrackIR Bridge"),
            tr("CrossOver was not found in /Applications.")
        );
        return;
    }

    QProcess registryProcess;

    QProcessEnvironment environment =
        QProcessEnvironment::systemEnvironment();

    environment.insert(
        QStringLiteral("CX_BOTTLE_PATH"),
        bottlesRoot
    );

    registryProcess.setProcessEnvironment(environment);

    const QStringList arguments = {
        QStringLiteral("--bottle"),
        bottleName,
        QStringLiteral("reg.exe"),
        QStringLiteral("add"),
        QStringLiteral(
            "HKCU\\Software\\NaturalPoint\\NATURALPOINT\\NPClient Location"
        ),
        QStringLiteral("/v"),
        QStringLiteral("Path"),
        QStringLiteral("/t"),
        QStringLiteral("REG_SZ"),
        QStringLiteral("/d"),
        QStringLiteral("C:\\TrackIR\\"),
        QStringLiteral("/f")
    };

    registryProcess.start(wineExecutable, arguments);

    if (!registryProcess.waitForStarted(10000))
    {
        QMessageBox::critical(
            this,
            tr("CrossOver TrackIR Bridge"),
            tr("Unable to start the CrossOver registry command:\n%1")
                .arg(registryProcess.errorString())
        );
        return;
    }

    if (!registryProcess.waitForFinished(30000))
    {
        registryProcess.kill();
        registryProcess.waitForFinished();

        QMessageBox::critical(
            this,
            tr("CrossOver TrackIR Bridge"),
            tr("The CrossOver registry command timed out.")
        );
        return;
    }

    const QString standardOutput =
        QString::fromUtf8(registryProcess.readAllStandardOutput());

    const QString standardError =
        QString::fromUtf8(registryProcess.readAllStandardError());

    if (
        registryProcess.exitStatus() != QProcess::NormalExit ||
        registryProcess.exitCode() != 0
    )
    {
        QMessageBox::critical(
            this,
            tr("CrossOver TrackIR Bridge"),
            tr(
                "The DLL was copied, but the registry configuration failed.\n\n"
                "Output:\n%1\n\nError:\n%2"
            ).arg(standardOutput, standardError)
        );

        updateInstallationStatus();
        return;
    }

    s.bottles_root = bottlesRoot;
    s.selected_bottle = bottleName;
    s.b->save();

    updateInstallationStatus();

    QMessageBox::information(
        this,
        tr("CrossOver TrackIR Bridge"),
        tr(
            "The bridge was installed successfully in bottle \"%1\".\n\n"
            "NPClient64.dll:\n%2\n\n"
            "Registry path:\nC:\\TrackIR\\"
        ).arg(bottleName, targetDll)
    );
}

void CrossoverTrackIRControls::updateInstallationStatus()
{
    const QString bottlesRoot =
        ui.editBottlesRoot->text().trimmed();

    const QString bottleName =
        ui.comboBottle->currentText().trimmed();

    if (bottleName.isEmpty())
    {
        ui.labelInstallationStatus->setText(
            tr("No CrossOver bottle found.")
        );
        return;
    }

    const QString targetDll =
        installedDllPath(bottlesRoot, bottleName);

    if (QFileInfo::exists(targetDll))
    {
        ui.labelInstallationStatus->setText(
            tr("Bridge DLL installed in bottle \"%1\".")
                .arg(bottleName)
        );
    }
    else
    {
        ui.labelInstallationStatus->setText(
            tr("Bridge not installed in bottle \"%1\".")
                .arg(bottleName)
        );
    }
}
