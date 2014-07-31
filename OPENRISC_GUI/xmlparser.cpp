#include "xmlparser.h"

XMLParser::XMLParser(QString projectFileName, QWidget *parent) :
    QMainWindow(parent)
{
    projectInfo = parseXML(projectFileName);
}

XMLParser::~XMLParser() {}

ProjectInfo XMLParser::parseXML(QString projectFileName) {
    QFile* file = new QFile(projectFileName);

    if (!file->open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "QXSRExample::parseXML", "Couldn't open project file!", QMessageBox::Ok);
        exit(0);
    }
    QXmlStreamReader xml(file);

    ProjectInfo projInfo;

    while(!xml.atEnd() && !xml.hasError()) {
        QXmlStreamReader::TokenType token = xml.readNext();
        if(token == QXmlStreamReader::StartDocument) {
            continue;
        }
        if(token == QXmlStreamReader::StartElement) {
            if(xml.name() == "project") {
                continue;
            }

            if(xml.name() == "projectName") {
                this->addElementDataToMap(xml, projInfo.projectName);
            }
            if(xml.name() == "coreNum") {
                this->addElementDataToMap(xml, projInfo.coreNum);
            }
            if(xml.name() == "deviceType") {
                this->addElementDataToMap(xml, projInfo.deviceType);
            }
            if(xml.name() == "exampleFlag") {
                this->addElementDataToMap(xml, projInfo.exampleFlag);
            }

            if(xml.name() == "core") {
                projInfo.cores.append(this->parseCores(xml));
            }
        }
    }

    if(xml.hasError()) {
        QMessageBox::critical(this,
                              "QXSRExample::parseXML",
                              xml.errorString(),
                              QMessageBox::Ok);
    }

    xml.clear();
    file->close();

    return projInfo;
}

QMap<QString, QString> XMLParser::parseCores(QXmlStreamReader &xml) {
    QMap<QString, QString> core;

    if(xml.tokenType() != QXmlStreamReader::StartElement &&
            xml.name() == "core") {
        return core;
    }

    QXmlStreamAttributes attributes = xml.attributes();
    if(attributes.hasAttribute("id")) {
        core["id"] = attributes.value("id").toString();
    }

    xml.readNext();
    while(!(xml.tokenType() == QXmlStreamReader::EndElement &&
            xml.name() == "core")) {
        if(xml.tokenType() == QXmlStreamReader::StartElement) {
            if(xml.name() == "fileName") {
                this->addElementDataToMap(xml, core);
            }
        }
        xml.readNext();
    }
    return core;
}

void XMLParser::addElementDataToMap(QXmlStreamReader &xml, QMap<QString, QString> &map) const {
    if(xml.tokenType() != QXmlStreamReader::StartElement) {
        return;
    }

    QString elementName = xml.name().toString();
    xml.readNext();
    if(xml.tokenType() != QXmlStreamReader::Characters) {
        return;
    }

    map.insert(elementName, xml.text().toString());
}

