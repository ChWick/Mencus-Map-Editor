#include "languageresources.h"
#include <QFile>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QDir>
#include <quazip/quazipdir.h>

using namespace language;

LanguageResources::LanguageResources()
{
    mLanguageIds["en"] = "English";
    mLanguageIds["de"] = "German";
}

void LanguageResources::loadFromFileSystem(const QString &rootDirectory) {
    for (QString id : mLanguageIds.keys()) {
        QString filename = rootDirectory + "/res/values";
        if (id != "en") {
            // default language
            filename += "-" + id;
        }
        filename += "/strings.xml";

        QFile file(filename);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qWarning("Language file %s does not exist", id.toStdString().c_str());
            return;
        }
        QXmlStreamReader xml(&file);

        while(!xml.atEnd() &&
            !xml.hasError()) {

            /* Read next element.*/
            QXmlStreamReader::TokenType token = xml.readNext();
            /* If token is just StartDocument, we'll go to next.*/
            if(token == QXmlStreamReader::StartDocument) {
                continue;
            }
            /* If token is StartElement, we'll see if we can read it.*/
            if(token == QXmlStreamReader::StartElement) {
                /* If it's named persons, we'll go to the next.*/
                if(xml.name() == "string") {
                    QString stringid = xml.attributes().value("name").toString();
                    getResourceByLanguageId(id).getStringDataById(stringid).mString = xml.readElementText();
                    if (!mStringIds.contains(stringid)) {
                        mStringIds.push_back(stringid);
                    }
                }
            }
        }
    }
}

void LanguageResources::writeToFileSystem(const QString &rootDirectory) {
    for (QString id : mLanguageIds.keys()) {
        QString filename = rootDirectory + "/res/values";
        if (id != "en") {
            // default language
            filename += "-" + id;
        }
        QDir dir(rootDirectory);
        dir.mkpath(filename);

        filename += "/strings.xml";
        QFile file(filename);

        if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
            qWarning("Language file %s does not exist", id.toStdString().c_str());
            continue;
        }
        file.write(writeToString(id).toUtf8());
        file.close();
    }
}

void LanguageResources::writeToZipFile(QuaZip &zip) {
    for (QString id : mLanguageIds.keys()) {
        QString filename = "res/values";
        if (id != "en") {
            // default language
            filename += "-" + id;
        }
        filename += "/strings.xml";

        QuaZipFile mapFile(&zip);
        QuaZipNewInfo info(filename);
        info.setPermissions(QFile::WriteOwner | QFile::ReadOwner);
        if (!mapFile.open(QIODevice::WriteOnly | QIODevice::Truncate, info)) {
            qWarning("Map zip pack could not open a file");
        }
        if (!mapFile.isOpen()) {
            qWarning("Map zip pack could not open");
        }
        mapFile.write(writeToString(id).toUtf8());
        mapFile.close();
    }
}

QString LanguageResources::writeToString(const QString &id) {
    QString out;
    QXmlStreamWriter xml(&out);
    xml.setAutoFormatting(true);
    xml.writeStartDocument();
    xml.writeStartElement("resources");

    for (StringData &s : getResourceByLanguageId(id).mStringData) {
        xml.writeStartElement("string");
        xml.writeAttribute("name", s.mId);
        xml.writeCharacters(s.mString);
        xml.writeEndElement();
    }

    xml.writeEndElement();
    xml.writeEndDocument();

    return out;
}
