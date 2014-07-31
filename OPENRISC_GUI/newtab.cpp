#include "newtab.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

NewTab::NewTab(int coreID, QWidget *parent) :
    QWidget(parent)
{
    this->coreID = coreID;
    setupUI();
}

NewTab::NewTab(int coreID, QString filePath, bool exampleFlag, QWidget *parent) :
    QWidget(parent)
{
    this->coreID = coreID;
    setupUI();

    cFilePath = QDir::toNativeSeparators(filePath);
    if(!cFilePath.isEmpty()) {
        QFile f(cFilePath);
        if(f.open(QFile::ReadOnly | QFile::Text)) {
            QTextStream in(&f);
            cCode->setText(in.readAll());
            cCode->moveCursor(QTextCursor::Start);
            if(exampleFlag) {
                loadCCode->setEnabled(false);
                editCCode->setEnabled(false);
            } else {
                loadCCode->setEnabled(true);
                editCCode->setEnabled(true);
            }
            compile->setEnabled(true);
            QFileInfo fi(f);
            cBaseName = fi.baseName();
        }
    }
}

NewTab::~NewTab() {}

void NewTab::setupUI() {
    createCCodeArea();
    createAssemblyCodeArea();
    createMemoryBlockArea();

    connectSignalsAndSlots();
    connectToHost();

    QSplitter *splitter = new QSplitter(Qt::Horizontal);
    splitter->addWidget(cCodeBox);
    splitter->addWidget(assemblyCodeBox);
    splitter->addWidget(memoryBlockBox);
    splitter->setCollapsible(0, false);
    splitter->setCollapsible(1, false);
    splitter->setCollapsible(2, false);
    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addWidget(splitter);
    setLayout(mainLayout);

    // Setup UART dialog
    uart = new UARTDialog(coreID);
}

void NewTab::createCCodeArea() {
    cCodeBox = new QGroupBox(tr("C Code Area"));

    loadCCode = new QPushButton(tr("Load file"));
    editCCode = new QPushButton(tr("Edit"));
    editCCode->setEnabled(false);
    compile = new QPushButton(tr("Compile"));
    compile->setEnabled(false);

    QWidget *empty = new QWidget;
    empty->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    cCode = new CodeEditor;
    cCode->setReadOnly(true);
    CSyntaxHighlighter *highlighter = new CSyntaxHighlighter(cCode->document());

    QGridLayout *cCodeLayout = new QGridLayout;
    cCodeLayout->addWidget(loadCCode, 0, 0);
    cCodeLayout->addWidget(editCCode, 0, 1);
    cCodeLayout->addWidget(compile, 0, 2);
    cCodeLayout->addWidget(empty, 0, 3);
    cCodeLayout->addWidget(cCode, 1, 0, 1, 0);
    cCodeBox->setLayout(cCodeLayout);
}

void NewTab::createAssemblyCodeArea() {
    assemblyCodeBox = new QGroupBox(tr("Assembly Code Area"));

    editAssemblyCode = new QPushButton(tr("Edit"));
    editAssemblyCode->setEnabled(false);
    run = new QPushButton(tr("Run"));
    run->setEnabled(false);

    QWidget *empty = new QWidget;
    empty->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    assemblyCode = new CodeEditor;
    assemblyCode->setReadOnly(true);
    asmSyntaxHighlighter *highlighter = new asmSyntaxHighlighter(assemblyCode->document());

    QGridLayout *assemblyCodeLayout = new QGridLayout;
    assemblyCodeLayout->addWidget(editAssemblyCode, 0, 0);
    assemblyCodeLayout->addWidget(run, 0, 1);
    assemblyCodeLayout->addWidget(empty, 0, 2);
    assemblyCodeLayout->addWidget(assemblyCode, 1, 0, 1, 0);
    assemblyCodeBox->setLayout(assemblyCodeLayout);
}

void NewTab::createMemoryBlockArea() {
    memoryBlockBox = new QGroupBox(tr("Memory Block Area"));

    searchMemory = new QPushButton(tr("Goto"));
    searchMemory->setEnabled(false);
    address = new QLineEdit;
    address->setPlaceholderText(tr("range: 0000~1fff"));
    address->setMaximumWidth(120);
    address->setEnabled(false);
    uartButton = new QPushButton(tr("UART"));

    QWidget *empty = new QWidget;
    empty->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    memoryBlock = new MemoryTableWidget;

    QGridLayout *memoryBlockLayout = new QGridLayout;
    memoryBlockLayout->addWidget(searchMemory, 0, 0);
    memoryBlockLayout->addWidget(address, 0, 1);
    memoryBlockLayout->addWidget(empty, 0, 2);
    memoryBlockLayout->addWidget(uartButton, 0, 3);
    memoryBlockLayout->addWidget(memoryBlock, 1, 0, 1, 0);
    memoryBlockBox->setLayout(memoryBlockLayout);
}

void NewTab::connectToHost() {
    //Using QTcpSocket
    //tcpSocket.connectToHost(QHostAddress::LocalHost, 8888);

    //Using sys/socket.h
    struct sockaddr_in server;

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if(server_sock == -1) {
       qDebug() << "Could not create socket";
       exit(-1);
    }
    qDebug() << "Socket created";

    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons(8888);

    //Connect to remote server
    if(::connect(server_sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        qDebug() << "Connect failed, ERROR";
        exit(1);
    }
    qDebug() << "Connected";
}

void NewTab::setupInterConnect() {
    int socket_desc, client_sock, c;
    struct sockaddr_in server, client;

    //Create socket
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if(socket_desc == -1) {
        qDebug() << "Could not create socket.";
    }
    qDebug() << "Socket created.";

    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(7777);

    //Bind
    if(bind(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0) {
        qDebug() << "Bind failed, ERROR.";
        exit(1);
    }
    qDebug() << "Bind done.";

    //Listen
    listen(socket_desc, 3);

    while(1) {
        c = sizeof(struct sockaddr_in);
        client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
        if(client_sock < 0) {
            qDebug() << "Accept failed.";
            break;
        }

        recvFromOr1ksim(client_sock);
        break;
    }
}

void NewTab::recvFromOr1ksim(int socket_desc) {
    int sock = socket_desc;
    char *client_message = (char*)malloc(sizeof(char) * 2);

    memset(client_message, 0, 2);
    while(1) {
        ::read(sock, client_message, sizeof(client_message) * 2);
        if(!strcmp(client_message, "")) break;
        if(uart->isVisible()) uart->setData(client_message);

        write(sock, "ACK\0", 4);
        memset(client_message, 0, 2);
    }
}

void NewTab::connectSignalsAndSlots() {
    connect(loadCCode, SIGNAL(clicked()), this, SLOT(on_loadCCode_clicked()));
    connect(editCCode, SIGNAL(clicked()), this, SLOT(on_editCCode_clicked()));
    connect(compile, SIGNAL(clicked()), this, SLOT(on_compile_clicked()));
    connect(editAssemblyCode, SIGNAL(clicked()), this, SLOT(on_editAssemblyCode_clicked()));
    connect(run, SIGNAL(clicked()), this, SLOT(on_run_clicked()));
    connect(searchMemory, SIGNAL(clicked()), this, SLOT(on_searchMemory_clicked()));
    connect(uartButton, SIGNAL(clicked()), this, SLOT(on_uartButton_clicked()));
}

void NewTab::on_loadCCode_clicked() {
    QString initialName = cFilePath;
    if(initialName.isEmpty())
        initialName = QDir::homePath();
    cFilePath = QFileDialog::getOpenFileName(this, tr("Choose File"), initialName);
    cFilePath = QDir::toNativeSeparators(cFilePath);
    if(!cFilePath.isEmpty()) {
        QFile f(cFilePath);
        if(f.open(QFile::ReadOnly | QFile::Text)) {
            QTextStream in(&f);
            cCode->setText(in.readAll());
            cCode->moveCursor(QTextCursor::Start);
            editCCode->setEnabled(true);
            compile->setEnabled(true);
            QFileInfo fi(f);
            cBaseName = fi.baseName();
        }
    }
}

void NewTab::on_editCCode_clicked() {
    QProcess::execute("gedit", QStringList() << cFilePath);

    QFile inFile(cFilePath);
    if(!inFile.open(QIODevice::ReadOnly)) return;

    QTextStream in(&inFile);
    cCode->setText(in.readAll());
}

void NewTab::on_compile_clicked() {
    dir.setAutoRemove(false);
    if(dir.isValid()) {
        QFile::copy(cFilePath, dir.path()+"/"+cBaseName+".c");
        QFile::copy("device/"+deviceType+"/reset.o", dir.path()+"/reset.o");
        QFile::copy("device/"+deviceType+"/uart.o", dir.path()+"/uart.o");
        QFile::copy("device/"+deviceType+"/ram.ld", dir.path()+"/ram.ld");
        assemblyFilename = dir.path()+"/"+cBaseName+".S";

        if(memoryBlock->rowCount() != 2) memoryBlock->setRowCount(2);

        char cmd[1000];
        strcpy(cmd, "GAC");
        if(::send(server_sock, cmd, strlen(cmd), 0) < 0) {
            qDebug() << "Send message failed";
            exit(1);
        }

        char server_reply[1000];
        if(::recv(server_sock, server_reply, 1000, 0) < 0) {
            qDebug() << "Receive failed";
            exit(1);
        }

        if(!strcmp(server_reply, "ACK")) {
            QString path = dir.path();
            if(::send(server_sock, path.toStdString().c_str(), path.length(), 0) < 0) {
                qDebug() << "Send message failed";
                exit(1);
            }

            delay(3);

            if(::send(server_sock, cBaseName.toStdString().c_str(), cBaseName.length(), 0) < 0) {
                qDebug() << "Send message failed";
                exit(1);
            }

            delay(3);

            if(::send(server_sock, deviceType.toStdString().c_str(), deviceType.length(), 0) < 0) {
                qDebug() << "Send message failed";
                exit(1);
            }

            memset(server_reply, 0, 1000);
            if(::recv(server_sock, server_reply, 1000, 0) < 0) {
                qDebug() << "Receive failed";
                exit(1);
            }

            delay(3);

            if(!strcmp(server_reply, "ACK")) {
                QFile inFile(assemblyFilename);
                if(!inFile.open(QIODevice::ReadOnly)) return;

                QTextStream in(&inFile);
                assemblyCode->setText(in.readAll());

                editAssemblyCode->setEnabled(true);
                run->setEnabled(true);

                QFile hexFile(dir.path()+"/"+cBaseName+".hex");
                hexFile.open(QIODevice::ReadOnly);
                QTextStream hex(&hexFile);
                QStringList input;
                int lineCount = 0;
                while(!hex.atEnd()) {
                    /*QString tmp = hex.readLine().rightJustified(8, '0');
                    for(int i = 2; i < 11; i+=3) tmp.insert(i, " ");
                    QString str;
                    for(int i = 0; i < 11; i+=3) {
                        if(tmp.at(i) == '0' && tmp.at(i+1) =='0') {
                            str.append('0');
                        } else {
                            str.append(tmp.at(i));
                            str.append(tmp.at(i+1));
                        }
                        if(i == 3) str.append("  ");
                        if(i == 0 || i == 6) str.append(' ');
                    }*/
                    input << hex.readLine();
                    lineCount++;
                }

                if(lineCount > 65536) lineCount = 65536;
                int length = QString::number(lineCount, 16).length();
                for(int i = 0; i < lineCount; i++) {
                    memoryBlock->insertRow(i+2);

                    memoryBlock->setItem(i+2, 0, new QTableWidgetItem(QString::number(i, 16).rightJustified(length, '0')));
                    memoryBlock->setItem(i+2, 1, new QTableWidgetItem(input.at(i)));
                    if(i < 2048) {
                        memoryBlock->setRowBackgroundColor(0, i+2, Qt::gray);
                    } else {
                        memoryBlock->setRowBackgroundColor(0, i+2, Qt::darkGray);
                    }
                }

                searchMemory->setEnabled(true);
                address->setEnabled(true);
            }
        }
    }
}

void NewTab::on_editAssemblyCode_clicked() {
    QProcess::execute("gedit", QStringList() << assemblyFilename);

    QFile inFile(assemblyFilename);
    if(!inFile.open(QIODevice::ReadOnly)) return;

    QTextStream in(&inFile);
    assemblyCode->setText(in.readAll());
}

void NewTab::on_run_clicked() {
    if(dir.isValid()) {
        if(memoryBlock->rowCount() != 2) {
            for(int i = 2; i < memoryBlock->rowCount(); i++) {
                memoryBlock->setItem(i, 2, new QTableWidgetItem(""));
                memoryBlock->setItem(i, 3, new QTableWidgetItem(""));
                memoryBlock->setItem(i, 4, new QTableWidgetItem(""));
            }
        }

        // Send message by C/C++ method
        char cmd[1000];
        strcpy(cmd, "GHF");
        if(::send(server_sock, cmd, strlen(cmd), 0) < 0) {
            qDebug() << "Send message failed";
            exit(1);
        }

        char server_reply[1000];
        if(::recv(server_sock, server_reply, 1000, 0) < 0) {
            qDebug() << "Receive failed";
            exit(1);
        }

        if(!strcmp(server_reply, "ACK")) {
            QString path = dir.path();
            if(::send(server_sock, path.toStdString().c_str(), path.length(), 0) < 0) {
                qDebug() << "Send message failed";
                exit(1);
            }

            delay(3);

            if(::send(server_sock, cBaseName.toStdString().c_str(), cBaseName.length(), 0) < 0) {
                qDebug() << "Send message failed";
                exit(1);
            }

            if(deviceType == "VEX_T2" || deviceType == "VLX_S6") {
                char server_reply[2000];
                while(1) {
                    ::read(server_sock, server_reply, sizeof(server_reply));
                    if(!strcmp(server_reply, "")) break;
                    if(uart->isVisible()) uart->setData(server_reply);
                    write(server_sock, "ACK\0", 4);
                    memset(server_reply, 0, 2000);
                }
            } else if(deviceType == "Or1ksim") {
                setupInterConnect();
            }

            delay(3);

            QFile outFile(dir.path()+"/"+cBaseName+".out");
            int lineCount = 0;
            outFile.open(QIODevice::ReadOnly);
            QTextStream out(&outFile);
            QStringList output;
            while(!out.atEnd()) {
                output << out.readLine();
                lineCount++;
            }

            //memoryBlock->setRowCount(lineCount);
            if(lineCount > 65536) lineCount = 65536;
            int length = QString::number(lineCount, 16).length();
            for(int i = 0; i < lineCount; i++) {
                memoryBlock->setItem(i+2, 2, new QTableWidgetItem(""));
                memoryBlock->setItem(i+2, 3, new QTableWidgetItem(QString::number(i, 16).rightJustified(length, '0')));
                memoryBlock->setItem(i+2, 4, new QTableWidgetItem(output.at(i)));
                if(i < 2048) {
                    memoryBlock->setRowBackgroundColor(1, i+2, Qt::gray);
                } else {
                    memoryBlock->setRowBackgroundColor(1, i+2, Qt::darkGray);
                }

                if(!memoryBlock->compareContent(i+2)) memoryBlock->setRowTextColor(i+2, Qt::red);
            }
        }
    }
}

void NewTab::on_searchMemory_clicked() {
    QList<QTableWidgetItem *> tmp = memoryBlock->findItems(address->text(), Qt::MatchExactly);
    foreach(QTableWidgetItem *rowptr, tmp) {
        memoryBlock->setCurrentCell(rowptr->row(), rowptr->column());
    }
}

void NewTab::on_uartButton_clicked() {
    if(!uart->isVisible()) uart->show();
}

QString NewTab::getFilePath() {
    return cFilePath;
}

void NewTab::setFilePath(QString filePath) {
    cFilePath = filePath;
}

void NewTab::setDeviceType(QString deviceType) {
    this->deviceType = deviceType;
}

void NewTab::delay(int sec) {
    QTime dieTime = QTime::currentTime().addSecs(sec);
    while(QTime::currentTime() < dieTime)
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}
