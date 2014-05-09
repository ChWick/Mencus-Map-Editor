#include "eventdatamask.h"

using namespace EventData;

EventDataMask EventDataMask::mSingleton;

EventDataMask::EventDataMask()
{
    addEventAttribute("type", "Type", SELECT_TYPE, LAYOUT_EVENT, QStringList() << "message" << "inject_message" << "change_tile" << "toggle");
    addEventAttribute("id", "Id", STRING_TYPE, LAYOUT_EVENT);
    addEventAttribute("title", "Title", STRING_TYPE, LAYOUT_EVENT)
            .addAttributeCondition("type", "message");
    addEventAttribute("msg_type", "Message type", SELECT_TYPE, LAYOUT_EVENT, QStringList() << "toggle_touch_input_blink" << "exit_reached")
            .addAttributeCondition("type", "inject_message");
    addEventAttribute("msg_button", "Message button", SELECT_TYPE, LAYOUT_EVENT, QStringList() << "left" << "right" << "jump" << "attack" << "activate" << "enter_link")
            .setAttributeConditionList(CONDITION_LIST()
                                       << CONDITION_PAIR("type", "inject_message")
                                       << CONDITION_PAIR("msg_type", "toggle_touch_input_blink"));
    addEventAttribute("msg_enabled", "Message enabled", BOOL_TYPE, LAYOUT_EVENT)
            .setAttributeConditionList(CONDITION_LIST()
                                       << CONDITION_PAIR("type", "inject_message")
                                       << CONDITION_PAIR("msg_type", "toggle_touch_input_blink"));

    addEventAttribute("initial", "Initial", BOOL_TYPE, LAYOUT_EVENT)
            .addAttributeCondition("type", "toggle");
    addEventAttribute("x", "x", POSITION_TYPE, LAYOUT_EVENT)
            .addAttributeCondition("type", "change_tile");
    addEventAttribute("y", "y", POSITION_TYPE, LAYOUT_EVENT)
            .addAttributeCondition("type", "change_tile");

    addEventAttribute("emitter", "Emitter", SELECT_TYPE, LAYOUT_EMITTER, QStringList() << "create" << "delete" << "collision" << "message_page_changed" << "user");
    addEventAttribute("repeat", "Repeat", SELECT_TYPE, LAYOUT_EMITTER, QStringList() << "once" << "infinite");
    addEventAttribute("src_id", "Source id", STRING_TYPE, LAYOUT_EMITTER)
            .setAttributeConditionList(CONDITION_LIST()
                                       << CONDITION_PAIR("emitter", "collision")
                                       << CONDITION_PAIR("emitter", "message_page_changed"), OP_OR);
    addEventAttribute("src_page", "Source page number", STRING_TYPE, LAYOUT_EMITTER)
            .addAttributeCondition("emitter", "message_page_changed");

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
