#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QStringList deviceList;
    deviceList << "VEX_T2" << "VLX_S6" << "Or1ksim";
    deviceBox = new QComboBox;
    deviceBox->addItems(deviceList);
    deviceBox->setEnabled(false);
    ui->mainToolBar->addWidget(deviceBox);

    addNewCore = new QToolButton;
    ui->tabWidget->setCornerWidget(addNewCore, Qt::TopLeftCorner);
    addNewCore->setCursor(Qt::ArrowCursor);
    addNewCore->setAutoRaise(false);
    addNewCore->setIcon(QIcon(":/icons/Add-icon.png"));
    ui->tabWidget->setVisible(false);

    connectSignalsAndSlots();
    exampleFlag = false;
    coreID = 0;
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::connectSignalsAndSlots() {
    connect(deviceBox, SIGNAL(currentIndexChanged(int)), this, SLOT(changeDevice()));
    connect(addNewCore, SIGNAL(clicked()), this, SLOT(openNewTab()));
}

void MainWindow::openNewTab() {
    NewTab *core = new NewTab(coreID);
    cores.append(core);
    ui->tabWidget->addTab(core, tr("Core%1").arg(coreID++));
}

void MainWindow::newProject() {
    if(okToContinue()) {
        closeAllTabs();
        projectName = "";
        deviceBox->setCurrentIndex(0);
        if(saveAs()) {
            addNewCore->setEnabled(true);
            deviceBox->setEnabled(true);
            loadProject();

            NewTab *core = new NewTab(coreID);
            core->setDeviceType(deviceBox->currentText());
            cores.append(core);
            ui->tabWidget->addTab(core, tr("Core%1").arg(coreID++));

            saveFile();
        }
    }
}

void MainWindow::load() {
    if(okToContinue()) {
        closeAllTabs();

        projectName = QFileDialog::getOpenFileName(this, tr("Open project"),
                                                       QDir::currentPath(),
                                                       tr("OpenRISC Project file (*.orp)"));
        projectName = QDir::toNativeSeparators(projectName);

        loadProject();
    }
}

void MainWindow::loadExample() {
    if(okToContinue()) {
        closeAllTabs();

        projectName = QFileDialog::getOpenFileName(this, tr("Open project"),
                                                       QDir::currentPath()+"/Examples",
                                                       tr("OpenRISC Project file (*.orp)"));
        projectName = QDir::toNativeSeparators(projectName);

        loadProject();
    }
}

bool MainWindow::loadProject() {
    if(!projectName.isEmpty()) {
        XMLParser *xmlParser = new XMLParser(projectName);

        if(xmlParser->projectInfo.coreNum.value("coreNum").toInt() ==
                xmlParser->projectInfo.cores.size()) {
            ui->groupBox->setTitle(xmlParser->projectInfo.projectName.value("projectName"));
            ui->tabWidget->setVisible(true);

            QFileInfo fi(projectName);
            QString filePath = fi.path();

            deviceBox->setCurrentText(xmlParser->projectInfo.deviceType.value("deviceType"));

            if(xmlParser->projectInfo.exampleFlag.value("exampleFlag") == "true") exampleFlag = true;
            else exampleFlag = false;
            if(exampleFlag) {
                addNewCore->setEnabled(false);
                deviceBox->setEnabled(false);
            } else {
                addNewCore->setEnabled(true);
                deviceBox->setEnabled(true);
            }

            for(int i = 0; i < xmlParser->projectInfo.coreNum.value("coreNum").toInt(); i++) {
                NewTab *core = new NewTab(coreID,
                                          filePath+"/"+xmlParser->projectInfo.cores.at(i).value("fileName"),
                                          exampleFlag);
                core->setDeviceType(deviceBox->currentText());
                cores.append(core);
                ui->tabWidget->addTab(core, tr("Core%1").arg(coreID++));
            }

            ui->statusBar->showMessage(tr("Project loaded"), 2000);
            return true;
        } else {
            QMessageBox::critical(this, "ERROR", "There's something wrong in projec file!", QMessageBox::Ok);
            ui->statusBar->showMessage(tr("Operation canceled"), 2000);
            return false;
        }
    } else {
        ui->statusBar->showMessage(tr("Operation canceled"), 2000);
        return false;
    }
}

bool MainWindow::save() {
    if(projectName.isEmpty()) return saveAs();
    else return saveFile();
}

bool MainWindow::saveFile() {
    QFileInfo projectFi(projectName);
    ProjectInfo projectInfo;
    projectInfo.projectName.insert("projectName", projectFi.baseName());
    projectInfo.coreNum.insert("coreNum", QString::number(coreID));
    projectInfo.deviceType.insert("deviceType", deviceBox->currentText());
    if(exampleFlag) projectInfo.exampleFlag.insert("exampleFlag", "true");
    else projectInfo.exampleFlag.insert("exampleFlag", "false");
    for(int i = 0; i < coreID; i++) {
        QFileInfo fi(cores.at(i)->getFilePath());
        QMap<QString, QString> core;
        core.insertMulti("id", QString::number(i));
        if(fi.path() != projectFi.path()) {
            QFile::copy(fi.filePath(), projectFi.path()+"/"+fi.fileName());
            cores.at(i)->setFilePath(projectFi.path()+"/"+fi.fileName());
        }
        core.insertMulti("fileName", fi.fileName());
        projectInfo.cores.append(core);
    }

    XMLGenerator *xmlGenerator = new XMLGenerator(projectName, projectInfo);
    ui->statusBar->showMessage(tr("Project saved"), 2000);
    return xmlGenerator->createXMLFile();
}

bool MainWindow::saveAs() {
    projectName = QFileDialog::getSaveFileName(this, tr("Save Project"), QDir::currentPath());
    projectName = QDir::toNativeSeparators(projectName);

    if(projectName.isEmpty()) return false;

    QDir dir;
    if(!dir.exists(projectName)) {
        dir.mkpath(projectName);
    }

    QFileInfo fi(projectName);
    projectName = projectName +"/"+ fi.fileName() + ".orp";

    return saveFile();
}

void MainWindow::closeProject() {
    if(okToContinue()) {
        closeAllTabs();
        projectName.clear();
    }
}

void MainWindow::changeDevice() {
    foreach (NewTab *core, cores) {
        core->setDeviceType(deviceBox->currentText());
    }
}

bool MainWindow::okToContinue() {
    if(!projectName.isEmpty()) {
        int r = QMessageBox::warning(this, tr("Warning"),
                                     tr("A project is opened.\n""Are you sure?"),
                                     QMessageBox::Yes | QMessageBox::No);
        if(r == QMessageBox::Yes) return save();
        else {
            ui->statusBar->showMessage(tr("Operation canceled"), 2000);
            return false;
        }
    }

    return true;
}

void MainWindow::closeAllTabs() {
    if(ui->tabWidget->count() != 0) {
        for(int i = 0; i <= ui->tabWidget->count(); i++) {
            ui->tabWidget->removeTab(0);
            cores.takeFirst();
        }

        ui->groupBox->setTitle("");
        ui->tabWidget->setVisible(false);
        deviceBox->setEnabled(false);
        exampleFlag = false;
        coreID = 0;
    }
}

