#include "camerarestriction.h"

CameraRestriction::CameraRestriction()
{
    mType = "horizontal";
    mValueX = 0;
    mValueY = 0;
}

CameraRestriction::CameraRestriction(const QXmlStreamReader &stream) {
    mType = stream.attributes().value("type").toString();
    mValueX = stream.attributes().value("x").toInt();
    mValueY = stream.attributes().value("y").toInt();
}


void CameraRestriction::writeToXML(QXmlStreamWriter &stream) const {
    stream.writeStartElement("restriction");
    stream.writeAttribute("type", mType);
    if (mType == "horizontal") {
        stream.writeAttribute("y", QString("%1").arg(mValueY));
    }
    else {
        stream.writeAttribute("x", QString("%1").arg(mValueX));
    }
    stream.writeEndElement();
}
