/*
 * Copyright (c) 2006-2015 ARM Limited
 * PackageLicenseDeclared: Apache-2.0
 */

#include "EventHandler.h"

namespace mbed {

EventHandler::EventHandler(pintfvoid_t function)
{
    attach(function);
}
EventHandler::EventHandler(const EventHandler &__x)
{
    *this = __x;
}

void EventHandler::swap(EventHandler &__x) {
    char tmpMember[16];
    int (*tmpcaller)(void*, char*, void*) = __x._membercaller;
    void *tmpobj = __x._p.object;
    memcpy(tmpMember,__x._member,sizeof(tmpMember));
    memcpy(__x._member,_member,sizeof(_member));
    __x._p.object = _p.object;
    __x._membercaller = _membercaller;
    memcpy(_member,tmpMember,sizeof(tmpMember));
    _p.object = tmpobj;
    _membercaller = tmpcaller;
}

void EventHandler::attach(pintfvoid_t function) {
    _p.function = function;
    _membercaller = 0;
}

int EventHandler::call(void *arg) {
    if (_membercaller == 0 && _p.function) {
        return _p.function(arg);
    } else if (_membercaller && _p.object) {
        return _membercaller(_p.object, _member, arg);
    }
    return -1; //TODO: This looks dangerous
}

int EventHandler::operator ()(void *arg) {
    return call(arg);
}

EventHandler::operator bool(void) const {
    return _p.function != 0;
}
EventHandler & EventHandler::operator = (pintfvoid_t function)
{
    attach(function);
    return *this;
}
EventHandler & EventHandler::operator = (const EventHandler &__x)
{
    _membercaller = __x._membercaller;
    _p = __x._p;
    memcpy(_member,__x._member,sizeof(_member));
    return *this;
}

} // namespace mbed
