#include "eventdatamask.h"

using namespace EventData;

EventDataMask EventDataMask::mSingleton;

EventDataMask::EventDataMask()
{
    addEventAttribute("type", "Type", SELECT_TYPE, LAYOUT_EVENT, QStringList() << "message" << "inject_message");
    addEventAttribute("id", "Id", STRING_TYPE, LAYOUT_EVENT);
    addEventAttribute("title", "Title", STRING_TYPE, LAYOUT_EVENT)
            .addAttributeCondition("type", "message");
    addEventAttribute("msg_type", "Message type", SELECT_TYPE, LAYOUT_EVENT, QStringList() << "button" << "exit_reached")
            .addAttributeCondition("type", "inject_message");
    addEventAttribute("msg_button", "Message button", SELECT_TYPE, LAYOUT_EVENT, QStringList() << "left" << "right" << "jump" << "attack" << "activate" << "enter_link")
            .addAttributeCondition("type", "inject_message")
            .addAttributeCondition("msg_type", "button");
    addEventAttribute("msg_enabled", "Message enabled", BOOL_TYPE, LAYOUT_EVENT)
            .addAttributeCondition("type", "inject_message")
            .addAttributeCondition("msg_type", "button");

    addEventAttribute("emitter", "Emitter", SELECT_TYPE, LAYOUT_EMITTER, QStringList() << "create" << "delete" << "collision");
    addEventAttribute("repeat", "Repeat", SELECT_TYPE, LAYOUT_EMITTER, QStringList() << "once" << "infinite");
    addEventAttribute("src_id", "Source id", STRING_TYPE, LAYOUT_EMITTER)
            .addAttributeCondition("emitter", "collision");

    addChildEventType("page", "Page")
            .addAttributeCondition("type", "message");

    addChildEventAttribute("text", "Text", STRING_TYPE, LAYOUT_CHILD_DATA, "page");
}

EventAttribute &EventDataMask::addEventAttribute(const QString &id, const QString &label, PropertyTypes propertyType, LayoutType layoutType, const QStringList &allowedValues) {
    EventAttribute a;
    a.mID = id;
    a.mLabel = label;
    a.mPropertyType = propertyType;
    a.mLayoutType = layoutType;
    a.mAllowedValues = allowedValues;
    mEventAttributes[id] = a;
    return mEventAttributes[id];
}

ChildEventType &EventDataMask::addChildEventType(const QString &id, const QString &label) {
    ChildEventType a;
    a.mID = id;
    a.mLabel = label;
    mChildEventTypes[id] = a;
    return mChildEventTypes[id];
}

ChildEventAttribute &EventDataMask::addChildEventAttribute(const QString &id, const QString &label, PropertyTypes propertyType, LayoutType layoutType, const QString &childEventType) {
    ChildEventAttribute a;
    a.mID = id;
    a.mLabel = label;
    a.mPropertyType = propertyType;
    a.mLayoutType = layoutType;
    a.mChildEventType = childEventType;
    mChildEventAttributes[id] = a;
    return mChildEventAttributes[id];
}
