#ifndef NEWTAB_H
#define NEWTAB_H

#include "headers.h"
#include "codeeditor.h"
#include "csyntaxhighlighter.h"
#include "asmsyntaxhighlighter.h"
#include "memorytablewidget.h"
#include "uartdialog.h"

class NewTab : public QWidget
{
    Q_OBJECT
public:
    explicit NewTab(int coreID, QWidget *parent = 0);
    explicit NewTab(int coreID, QString filePath, bool exampleFlag, QWidget *parent = 0);
    ~NewTab();

    QString getFilePath();
    void setFilePath(QString filePath);
    void setDeviceType(QString deviceType);

signals:

public slots:
    void on_loadCCode_clicked();
    void on_editCCode_clicked();
    void on_compile_clicked();
    void on_editAssemblyCode_clicked();
    void on_run_clicked();
    void on_searchMemory_clicked();
    void on_uartButton_clicked();

private:
    void setupUI();
    void createCCodeArea();
    void createAssemblyCodeArea();
    void createMemoryBlockArea();

    void connectToHost();
    void setupInterConnect();
    void recvFromOr1ksim(int);
    void connectSignalsAndSlots();
    void delay(int);

    //QTcpSocket tcpSocket;
    QGroupBox *cCodeBox;
    QGroupBox *assemblyCodeBox;
    QGroupBox *memoryBlockBox;
    QPushButton *loadCCode;
    QPushButton *editCCode;
    QPushButton *compile;
    QPushButton *editAssemblyCode;
    QPushButton *run;
    QPushButton *searchMemory;
    QLineEdit *address;
    QPushButton *uartButton;
    CodeEditor *cCode;
    CodeEditor *assemblyCode;
    MemoryTableWidget *memoryBlock;
    UARTDialog *uart;
    QString cFilePath;
    QString cBaseName;
    QString assemblyFilename;
    QString deviceType;
    QTemporaryDir dir;

    int server_sock;
    int coreID;
};

#endif // NEWTAB_H
