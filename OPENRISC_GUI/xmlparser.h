#ifndef XMLPARSER_H
#define XMLPARSER_H

#include "headers.h"
#include <QMainWindow>

class XMLParser : public QMainWindow
{
    Q_OBJECT
public:
    explicit XMLParser(QString proojectFileName, QWidget *parent = 0);
    ~XMLParser();

    ProjectInfo projectInfo;

signals:
    
public slots:

private:
    ProjectInfo parseXML(QString projectFileName);
    QMap<QString, QString> parseCores(QXmlStreamReader& xml);
    void addElementDataToMap(QXmlStreamReader& xml, QMap<QString, QString>& map) const;
};

#endif // XMLPARSER_H
