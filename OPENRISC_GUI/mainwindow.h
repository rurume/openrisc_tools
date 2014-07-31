#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include "newtab.h"
#include "xmlgenerator.h"
#include "xmlparser.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    int coreID;

public slots:
    void openNewTab();
    void newProject();
    void load();
    void loadExample();
    bool save();
    bool saveAs();
    void closeProject();
    void changeDevice();

private:
    void connectSignalsAndSlots();

    bool loadProject();
    bool saveFile();
    bool okToContinue();
    void closeAllTabs();

    Ui::MainWindow *ui;
    QComboBox *deviceBox;
    QToolButton *addNewCore;
    QString projectName;
    QList<NewTab*> cores;
    bool exampleFlag;
};

#endif // MAINWINDOW_H
