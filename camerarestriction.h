#ifndef CAMERARESTRICTION_H
#define CAMERARESTRICTION_H

#include <QSharedPointer>
#include <QString>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

class CameraRestriction
{
private:
    QString mType;
    int mValueX;
    int mValueY;
public:
    CameraRestriction();
    CameraRestriction(const QXmlStreamReader &stream);

    void writeToXML(QXmlStreamWriter &stream) const;
};

typedef QSharedPointer<CameraRestriction> CameraRestrictionPtr;
typedef QList<CameraRestrictionPtr> camera_restriction_list;
Q_DECLARE_METATYPE( CameraRestrictionPtr )

#endif // CAMERARESTRICTION_H
