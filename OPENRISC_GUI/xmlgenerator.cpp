#include "xmlgenerator.h"

XMLGenerator::XMLGenerator(QString projectPath, ProjectInfo projectInfo, QWidget *parent) :
    QMainWindow(parent)
{
    this->projectPath = projectPath;
    this->projectInfo = projectInfo;
}

XMLGenerator::~XMLGenerator() {}

bool XMLGenerator::createXMLFile() {
    QFile *file = new QFile(projectPath);
    if (!file->open(QIODevice::WriteOnly)) {
        QMessageBox::warning(0, "Read only", "The file is in read only mode");
        return false;
    } else {
        QXmlStreamWriter* xmlWriter = new QXmlStreamWriter();
        xmlWriter->setDevice(file);
        xmlWriter->writeStartDocument();
        xmlWriter->writeStartElement("project");

        xmlWriter->writeStartElement("projectName");
        xmlWriter->writeCharacters(projectInfo.projectName["projectName"]);
        xmlWriter->writeEndElement();
        xmlWriter->writeStartElement("coreNum");
        xmlWriter->writeCharacters(projectInfo.coreNum["coreNum"]);
        xmlWriter->writeEndElement();
        xmlWriter->writeStartElement("deviceType");
        xmlWriter->writeCharacters(projectInfo.deviceType["deviceType"]);
        xmlWriter->writeEndElement();
        xmlWriter->writeStartElement("exampleFlag");
        xmlWriter->writeCharacters(projectInfo.exampleFlag["exampleFlag"]);
        xmlWriter->writeEndElement();

        while(!projectInfo.cores.isEmpty()) {
            QMap<QString,QString> core = projectInfo.cores.takeFirst();
            xmlWriter->writeStartElement("core");
            xmlWriter->writeAttribute("id", core["id"]);
            xmlWriter->writeStartElement("fileName");
            xmlWriter->writeCharacters(core["fileName"]);
            xmlWriter->writeEndElement();
            xmlWriter->writeEndElement();
        }

        xmlWriter->writeEndElement();
        xmlWriter->writeEndDocument();

        file->close();
        return true;
    }
}
