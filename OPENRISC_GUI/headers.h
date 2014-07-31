#ifndef HEADERS_H
#define HEADERS_H

#include <QWidget>
#include <QPushButton>
#include <QToolButton>
#include <QComboBox>
#include <QTextEdit>
#include <QLineEdit>
#include <QLabel>
#include <QTableWidget>
#include <QHeaderView>
#include <QGroupBox>
#include <QGridLayout>
#include <QSplitter>
#include <QSpacerItem>
#include <QFileDialog>
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QProcess>
#include <QTemporaryDir>
#include <QTime>
#include <QSlider>
#include <QCoreApplication>
#include <QDesktopWidget>
#include <QMessageBox>
#include <QXmlStreamReader>
#include <QMap>

#include <QDebug>

typedef struct PROJECTINFO {
    QMap<QString, QString> projectName;
    QMap<QString, QString> coreNum;
    QMap<QString, QString> deviceType;
    QMap<QString, QString> exampleFlag;
    QList< QMap<QString,QString> > cores;
} ProjectInfo;

#endif // HEADERS_H
