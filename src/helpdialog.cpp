// Credit to:
// qt-creator-2.2.1-src/src/plugins/coreplugin/versiondialog.cpp
#include "helpdialog.h"
#include "ui_helpdialog.h"
#include "version.h"

HelpDialog::HelpDialog(QWidget *parent) :
QDialog(parent),
ui(new Ui::HelpDialog)
{
    ui->setupUi(this);
    
    setWindowTitle(tr("Help PlotViz3"));
    
    const QString desc = tr(
                            "<style type=\"text/css\">"
                            "table {"
                            "border-width: 1px;"
                            "background-color: white;"
                            "}"
                            "td {"
                            "padding: 5px;"
                            "text-align: right;"
                            "}"
                            "</style>"
                            "<h3>Interactor</h3>"
                            "<table>"
                            "<tr>"
                            "<th>Mouse/Keyboard</th>"
                            "<th>Operation</th>"
                            "</tr>"
                            "<tr>"
                            "<td class=\"key\">Mouse left button</td>"
                            "<td>rotate trackball camera</td>"
                            "</tr>"
                            "<tr>"
                            "<td>Mouse right button</td>"
                            "<td>zoom</td>"
                            "</tr>"
                            "<tr>"
                            "<td>Mouse wheel</td>"
                            "<td>zoom</td>"
                            "</tr>"
                            "<tr>"
                            "<td>Shift + Mouse left button</td>"
                            "<td>pan</td>"
                            "</tr>"
                            "<tr>"
                            "<td>Ctrl + Mouse left button</td>"
                            "<td>spin</td>"
                            "</tr>"
                            "<tr>"
                            "<td>Keypress f</td>"
                            "<td>fly to the selected point</td>"
                            "</tr>"
                            "<tr>"
                            "<td>Keypress s</td>"
                            "<td>show surfaces</td>"
                            "</tr>"
                            "<tr>"
                            "<td>Keypress w</td>"
                            "<td>show wireframe</td>"
                            "</tr>"
                            "<tr>"
                            "<td>Keypress 2</td>"
                            "<td>2D interaction only (no rotation)</td>"
                            "</tr>"
                            "<tr>"
                            "<td>Keypress 3</td>"
                            "<td>3D interaction</td>"
                            "</tr>"
                            "</table>"
                            );
    
    ui->help->setText(desc);
    ui->help->setWordWrap(true);
    ui->help->setOpenExternalLinks(true);
    ui->help->setTextInteractionFlags(Qt::TextBrowserInteraction);
    
}

HelpDialog::~HelpDialog()
{
    delete ui;
}
