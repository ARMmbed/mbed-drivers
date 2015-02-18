/*
 * Copyright (c) 2006-2015 ARM Limited
 * PackageLicenseDeclared: Apache-2.0
 */
#ifndef MBED_EVENTHANDLER_H
#define MBED_EVENTHANDLER_H

#include <string.h>

namespace mbed {

typedef int (*pintfvoid_t)(void *);

/** A class for storing and calling a pointer to a static or member void function
 */
class EventHandler {
public:

    EventHandler(pintfvoid_t function = 0);
    EventHandler(const EventHandler & __x);

    void swap(EventHandler &__x);

    template<typename T>
    EventHandler(T *object, int (T::*member)(void *)) {
        attach(object, member);
    }

    void attach(pintfvoid_t function = 0);

    template<typename T>
    void attach(T *object, void (T::*member)(void)) {
        _p.object = static_cast<void*>(object);
        memcpy(_member, (char*)&member, sizeof(member));
        _membercaller = &EventHandler::membercaller<T>;
    }

    int call(void * arg);

    pintfvoid_t get_function() const {

        return (pintfvoid_t)_p.function;
    }

    operator bool () const;
    int operator()(void *);
    EventHandler & operator = (pintfvoid_t function);
    EventHandler & operator = (const EventHandler &__x);

private:
    template<typename T>
    static int membercaller(void *object, char *member, void* arg) {
        T* o = static_cast<T*>(object);
        int (T::**m)(void*) = static_cast<int (T::**)(void*)>(member);
        return (o->**m)(arg);
    }

    union {
        pintfvoid_t function;             // static function pointer - 0 if none attached
        void *object;                       // object this pointer - 0 if none attached
    } _p;
    int (*_membercaller)(void*, char*, void*); // registered membercaller function to convert back and call _member on _object
    char _member[16];                    // raw member function pointer storage - converted back by registered _membercaller
};

} // namespace mbed

#endif // MBED_EVENTHANDLER_H
