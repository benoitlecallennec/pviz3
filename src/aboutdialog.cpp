// Credit to:
// qt-creator-2.2.1-src/src/plugins/coreplugin/versiondialog.cpp
#include "aboutdialog.h"
#include "ui_aboutdialog.h"
#include "version.h"

//#define IDE_VERSION_MAJOR 0
//#define IDE_VERSION_MINOR 5
//#define IDE_VERSION_RELEASE 9

#define STRINGIFY_INTERNAL(x) #x
#define STRINGIFY(x) STRINGIFY_INTERNAL(x)

#define IDE_VERSION STRINGIFY(PVIZ3_VERSION_MAJOR) \
"." STRINGIFY(CPACK_PACKAGE_VERSION_MINOR) \
"." STRINGIFY(CPACK_PACKAGE_VERSION_RELEASE)

//const char * const IDE_VERSION_LONG      = IDE_VERSION;
const char * const IDE_VERSION_LONG      = PVIZ3_VERSION;
#ifdef PVIZ3_REVISION
const char * const IDE_REVISION_LONG      = PVIZ3_REVISION;
#else
const char * const IDE_REVISION_LONG      = "0000"
#endif

AboutDialog::AboutDialog(QWidget *parent) :
QDialog(parent),
ui(new Ui::AboutDialog)
{
    ui->setupUi(this);
    
    setWindowTitle(tr("About PlotViz3"));
    
    QString version = QLatin1String(IDE_VERSION_LONG);
    QString revision = QLatin1String(IDE_REVISION_LONG);
    
    const QString desc = tr(
                            "<h3>PlotViz3 ver. %1</h3>"
                            /*
                             "Based on Qt %2 (%3 bit)<br/>"
                             "<br/>"
                             */
                             "Revision: %2. Built on %3 at %4<br />"
                            /*
                             "<br/>"
                             "%9"
                             */
                            "<br/>"
                            "This software developed by the Community Grid Labs. "
                            "For further information contact the Community Grid Labs at "
                            "<a href=http://www.pti.iu.edu>http://www.pti.iu.edu</a><br/>"
                            "<br/>"
                            "Copyright 2011 The Trustees of Indiana University. All rights reserved.<br/>"
                            "<br/>"
                            "The program is provided AS IS with NO WARRANTY OF ANY KIND, "
                            "INCLUDING THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A "
                            "PARTICULAR PURPOSE.<br/>")
    .arg(version, revision, QLatin1String(__DATE__), QLatin1String(__TIME__));
    
    ui->about->setText(desc);
    ui->about->setWordWrap(true);
    ui->about->setOpenExternalLinks(true);
    ui->about->setTextInteractionFlags(Qt::TextBrowserInteraction);
    
}

AboutDialog::~AboutDialog()
{
    delete ui;
}
