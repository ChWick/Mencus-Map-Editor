#ifndef EVENTDATAMASK_H
#define EVENTDATAMASK_H

#include <QString>
#include <QStringList>
#include <QVector>
#include <QPair>
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
    POSITION_TYPE,
};
enum LayoutType {
    LAYOUT_EVENT,
    LAYOUT_EMITTER,
    LAYOUT_CHILD_DATA,
};

enum BoolOperation {
    OP_AND,
    OP_OR,
};
enum EqualsCondition {
    EQUAL,
    NOT_EQUAL,
};

struct AttributeCheckBase {
    virtual bool isActive(const Event::Entry &ent) const = 0;
};

struct AttributeCondition : public AttributeCheckBase {
    AttributeCondition(const QString &key, const QString &value, EqualsCondition cond = EQUAL)
        : mKey(key), mValue(value), mCondition(cond) {

    }

    QString mKey;
    QString mValue;
    EqualsCondition mCondition;

    virtual bool isActive(const Event::Entry &ent) const  {
        if (ent.mData.contains(mKey) == false) {return mCondition == NOT_EQUAL;}
        if (mCondition == EQUAL) {
            return ent.mData[mKey] == mValue;
        }
        return ent.mData[mKey] != mValue;
    }
};

struct AttributeOperation : public AttributeCheckBase {
    BoolOperation mOperation;
    std::shared_ptr<AttributeCheckBase> mFirst;
    std::shared_ptr<AttributeCheckBase> mSecond;


    virtual bool isActive(const Event::Entry &ent) const  {
        if (mOperation == OP_AND) {
            return mFirst->isActive(ent) && mSecond->isActive(ent);
        }
        else {
            return mFirst->isActive(ent) || mSecond->isActive(ent);
        }
    }
};

typedef QMap<QString, QString> ATTRIBUTE_MAP;
typedef QPair<QString, QString> CONDITION_PAIR;
typedef QVector<CONDITION_PAIR> CONDITION_LIST;
struct EventAttribute {
    QString mID;
    QString mLabel;
    PropertyTypes mPropertyType;
    LayoutType mLayoutType;
    QStringList mAllowedValues;
    std::shared_ptr<AttributeCheckBase> mAttributeConditions;

    void addAttributeCondition(const QString &key, const QString &value) {
        mAttributeConditions = std::shared_ptr<AttributeCheckBase>(new AttributeCondition(key, value));
    }
    void setAttributeConditionList(const CONDITION_LIST &pairs, BoolOperation op = OP_AND) {
        int index = 0;
        std::shared_ptr<AttributeCheckBase> *condition = &mAttributeConditions;

        while (index < pairs.size() - 1) {
            AttributeOperation *pOp = new AttributeOperation();
            pOp->mOperation = op;
            pOp->mFirst = std::shared_ptr<AttributeCheckBase>(new AttributeCondition(pairs[index].first, pairs[index].second));
            *condition = std::shared_ptr<AttributeCheckBase>(pOp);
            condition = &pOp->mSecond;

            index++;
        }
        *condition = std::shared_ptr<AttributeCheckBase>(new AttributeCondition(pairs[index].first, pairs[index].second));
    }

    virtual bool isActive(const Event::Entry &ent) const {
        if (mAttributeConditions) {return mAttributeConditions->isActive(ent);}

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
