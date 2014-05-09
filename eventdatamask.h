#ifndef EVENTDATAMASK_H
#define EVENTDATAMASK_H

#include <QString>
#include <QStringList>
#include "map.h"

namespace EventData {

enum PropertyTypes {
    EVENT_TYPE,
    EMITTER_TYPE,
    REPEAT_TYPE,
    STRING_TYPE,
    MESSAGE_TYPE,
    BUTTON_TYPE,
    BOOL_TYPE,
    SELECT_TYPE,
};
enum LayoutType {
    LAYOUT_EVENT,
    LAYOUT_EMITTER,
    LAYOUT_CHILD_DATA,
};

typedef QMap<QString, QString> ATTRIBUTE_MAP;
struct EventAttribute {
    QString mID;
    QString mLabel;
    PropertyTypes mPropertyType;
    LayoutType mLayoutType;
    ATTRIBUTE_MAP mAttributeConditions;
    QStringList mAllowedValues;

    EventAttribute &addAttributeCondition(const QString &key, const QString &value) {
        mAttributeConditions[key] = value;
        return *this;
    }

    virtual bool isActive(const Event::Entry &ent) const {
        for (ATTRIBUTE_MAP::const_iterator it = mAttributeConditions.cbegin(); it != mAttributeConditions.cend(); it++) {
            if (ent.mData.contains(it.key()) == false) {return false;}
            if (ent.mData[it.key()] != it.value()) {
                return false;
            }
        }
        return true;
    }
};

typedef EventAttribute ChildEventType;

struct ChildEventAttribute : public EventAttribute {
    QString mChildEventType;

    bool isActive(const Event::Entry &ent, const QString &childEventType) const {
        if (childEventType != mChildEventType) {
            return false;
        }
        return EventAttribute::isActive(ent);
    }
};

class EventDataMask
{
private:
    static EventDataMask mSingleton;

    QMap<QString, EventAttribute> mEventAttributes;
    QMap<QString, ChildEventType> mChildEventTypes;
    QMap<QString, ChildEventAttribute> mChildEventAttributes;
public:
    static EventDataMask &singleton() {return mSingleton;}
    static EventDataMask *singletonPtr() {return &mSingleton;}

    const QMap<QString, EventAttribute> &eventAttributes() const {return mEventAttributes;}
    const QMap<QString, ChildEventType> &childEventTypes() const {return mChildEventTypes;}
    const QMap<QString, ChildEventAttribute> &childEventAttributes() const {return mChildEventAttributes;}
private:
    EventDataMask();

    EventAttribute &addEventAttribute(const QString &id, const QString &label, PropertyTypes propertyType, LayoutType layoutType, const QStringList &allowedValues = QStringList());
    ChildEventType &addChildEventType(const QString &id, const QString &label);
    ChildEventAttribute &addChildEventAttribute(const QString &id, const QString &label, PropertyTypes propertyType, LayoutType layoutType, const QString &childEventType);
};

}

#endif // EVENTDATAMASK_H
