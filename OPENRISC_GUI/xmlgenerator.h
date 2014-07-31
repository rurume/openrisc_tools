#ifndef GENERATEXML_H
#define GENERATEXML_H

#include "headers.h"
#include <QMainWindow>
#include <QXmlStreamWriter>
#include <QMapIterator>

namespace Ui {
class XMLGenerator;
}

class XMLGenerator : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit XMLGenerator(QString projectPath, ProjectInfo projectInfo, QWidget *parent = 0);
    ~XMLGenerator();
    bool createXMLFile();

private:
    QString projectPath;
    ProjectInfo projectInfo;
};

#endif // GENERATEXML_H
