#include "ftnoir_protocol_ftx.h"
#include <QDebug>
#include <QFileDialog>
#include <QDir>
#include <QDirIterator>
#include <QtWidgets/qpushbutton.h>
#include <qcombobox.h>
#include <qdebug.h>
#include <qdir.h>
#include <qradiobutton.h>
#include <QDialogButtonBox>

#include "api/plugin-api.hpp"
#include "options/tie.hpp"


FTControls::FTControls()
{
    ui.setupUi(this);

    // customButton = ui.buttonBox->addButton("CustomButton",QDialogButtonBox::AcceptRole);
    // connect(customButton, &QPushButton::clicked,this, &FTControls::doCustom);

    connect(ui.buttonBox, &QDialogButtonBox::accepted, this, &FTControls::doOK);
    connect(ui.buttonBox, &QDialogButtonBox::rejected, this, &FTControls::doCancel);
}


void FTControls::doOK()
{
    s.b->save();
    close();
}

void FTControls::doCancel()
{
    s.b->reload();
    close();
}


// void FTControls::doCustom()
// {
//     QDesktopServices::openUrl("file:///Library/Application Support/opentrack"));
// }



