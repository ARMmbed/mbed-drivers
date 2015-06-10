/*
 * Copyright (c) 2015 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "mbed.h"
#include "mbed/Event.h"
#include <stdio.h>

/******************************************************************************
 * Generic helpers
 *****************************************************************************/

template<typename R, typename Arg>
static void call_fp1(const char* name, FunctionPointer1<R, Arg>& fptr, const Arg& arg) {
    printf(">>>>>>>> Testing '%s' <<<<<<<<\r\n", name);
    printf("[Direct call] ");
    fptr(arg);
    printf("[Event call]  ");
    Event(fptr, arg)();
}

template<typename R>
static void call_fp0(const char* name, FunctionPointer0<R>& fptr) {
    printf(">>>>>>>> Testing '%s' <<<<<<<<\r\n", name);
    printf("[Direct call] ");
    fptr();
    printf("[Event call]  ");
    Event(fptr).call();
}

// Test calling event by value
static void call_event(const char* name, Event e) {
    printf("[Call event '%s'] ", name);
    e.call();
}

/******************************************************************************
 * Test with functions that are not part of a class
 *****************************************************************************/

static void sa_func_1(const char *msg) {
    printf("Calling sa_func_1 with msg=%s\r\n", msg);
}

static void sa_func_2(int arg) {
    printf("Calling sa_func_2 with arg=%d\r\n", arg);
}

static void sa_func_3() {
    printf("Calling sa_func_3 (no arguments)\r\n");
}

static void test_standalone_funcs() {
    printf("\r\n********** Starting test_standalone_funcs **********\r\n");
    const char *testmsg1 = "Test message 1";
    const char *testmsg2 = "Test message 2";
    const int testint1 = 13;

    // First call function pointers directly
    FunctionPointer1<void, const char*> fp1(sa_func_1);
    FunctionPointer1<void, int> fp2(sa_func_2);
    FunctionPointer0<void> fp3(sa_func_3);
    call_fp1("ptr to standalone func(char*)", fp1, testmsg1);
    call_fp1("ptr to standalone func(char*)", fp1, testmsg2);
    call_fp1("ptr to standalone func(int)", fp2, testint1);
    call_fp0("ptr to standalone func(void)", fp3);
}

/******************************************************************************
 * Test with functions that are part of a class (trivially copyable arguments)
 *****************************************************************************/

class VBase {
public:
    VBase(int arg): _arg(arg) {}
    void print_baseonly(const char * msg) {
        printf("VBase::print_baseonly: %s, _arg=%d\r\n", msg, _arg);
    }
    virtual void print_virtual_str(const char * msg) {
        printf("VBase::print_virtual_str: %s, _arg=%d\r\n", msg, _arg);
    }
    virtual void print_virtual_noargs() {
        printf("VBase::print_virtual_noargs, _arg=%d\r\n", _arg);
    }
    void print_non_virtual(const char* msg) {
        printf("VBase::print_non_virtual, _msg=%s, _arg=%d\r\n", msg, _arg);
    }
protected:
    int _arg;
};

class VDerived : public VBase {
public:
    VDerived(int arg1, int arg2): VBase(arg1), _arg2(arg2) {}
    void print_non_virtual(const char* msg) {
        printf("VDerived::print_non_virtual, _msg=%s, _arg=%d, _arg2=%d\r\n", msg, _arg, _arg2);
    }
    virtual void print_virtual_str(const char * msg) {
        printf("VDerived::print_virtual_str: %s, _arg=%d, _arg2=%d\r\n", msg, _arg, _arg2);
    }
    virtual void print_virtual_noargs() {
        printf("VDerived::print_virtual_noargs, _arg=%d, _arg2=%d\r\n", _arg, _arg2);
    }
private:
    int _arg2;
};

static void test_class_funcs_tca() {
    printf("\r\n********** Starting test_class_funcs_tca **********\r\n");
    VBase base(10);
    VDerived derived(20, 100);
    const char *testmsg1 = "Test message 1";
    const char *testmsg2 = "Test message 2";

    printf("---- Part 1: test virtual functions\r\n");
    FunctionPointer1<void, const char*> p1_fp1(&base, &VBase::print_virtual_str);
    FunctionPointer1<void, const char*> p1_fp2(&derived, &VDerived::print_virtual_str);
    FunctionPointer1<void, const char*> p1_fp3((VBase*)&derived, &VBase::print_virtual_str);
    FunctionPointer0<void> p1_fp4((VBase*)&derived, &VBase::print_virtual_noargs);
    call_fp1("ptr to base::print_virtual_str", p1_fp1, testmsg1);
    call_fp1("ptr to derived::print_virtual_str", p1_fp2, testmsg2);
    call_fp1("ptr to derived::print_virtual_str via VBase* pointer", p1_fp3, testmsg2);
    call_fp0("ptr to derived::print_virtual_noargs via VBase* pointer", p1_fp4);
    printf("---- Part 2: call base-only function from base and derived\r\n");
    FunctionPointer1<void, const char*> p2_fp1(&base, &VBase::print_baseonly);
    FunctionPointer1<void, const char*> p2_fp2((VBase*)&derived, &VBase::print_baseonly);
    call_fp1("ptr to base::print_baseonly", p2_fp1, testmsg1);
    call_fp1("ptr to base::print_baseonly using VDerived instance", p2_fp2, testmsg1);
    printf("---- Part 3: call non-virtual function from base and derived\r\n");
    FunctionPointer1<void, const char*> p3_fp1(&base, &VBase::print_non_virtual);
    FunctionPointer1<void, const char*> p3_fp2(&derived, &VDerived::print_non_virtual);
    FunctionPointer1<void, const char*> p3_fp3((VBase*)&derived, &VBase::print_non_virtual);
    call_fp1("ptr to base::print_non_virtual", p3_fp1, testmsg1);
    call_fp1("ptr to derived::print_non_virtual", p3_fp2, testmsg2);
    call_fp1("ptr to base::print_non_virtual via Derived* pointer", p3_fp3, testmsg2);
}

/******************************************************************************
 * Mixed test (stand alone functions and class function) using non-trivially
 * copyable arguments
 *****************************************************************************/

class MyArg {
public:
    MyArg(const char* id = "(none)", int arg1 = 0, int arg2 = 0): _id(id), _arg1(arg1), _arg2(arg2) {
        instcount ++;
    }

    MyArg(const MyArg& arg): _arg1(arg._arg1), _arg2(arg._arg2) {
        _id = !strcmp(arg._id, "test") ? "(copy)" : arg._id;
        instcount ++;
    }

    ~MyArg() {
        instcount --;
    }

    void print() const {
        printf("Instance '%s'[%p] of MyArg, arg1=%d, arg2=%d\r\n", _id, this, _arg1, _arg2);
    }

    const char *_id;
    int _arg1, _arg2;
    static int instcount;
};

int MyArg::instcount = 0;

static void sa_ntc(MyArg arg) {
    printf("Called sa_ntc with arg '%s': ", arg._id);
    arg.print();
}

class ABase {
public:
    ABase(int arg): _arg(arg) {}
    virtual void print_virtual_arg(MyArg a) {
        printf("ABase::print_virtual_arg: [%s, %d, %d], _arg=%d\r\n", a._id, a._arg1, a._arg2, _arg);
    }
protected:
    int _arg;
};

class ADerived : public ABase {
public:
    ADerived(int arg1, int arg2): ABase(arg1), _arg2(arg2) {}
    virtual void print_virtual_arg(MyArg a) {
        printf("ADerived::print_virtual_arg: [%s, %d, %d], _arg=%d, _arg2=%d\r\n", a._id, a._arg1, a._arg2, _arg, _arg2);
    }
private:
    int _arg2;
};

static void test_funcs_nontca() {
    printf("\r\n********** Starting test_funcs_nontca **********\r\n");

    FunctionPointer1<void, MyArg> fp1(sa_ntc);
    Event e1, e2, e3;
    {
        // Test binding argument that gets out of scope at the end of this block
        MyArg arg("test", 10, 20);
        call_fp1("ptr to standalone func taking non-trivial arg", fp1, arg);
        e1 = e2 = e3 = fp1.bind(arg);
    }
    e1.call(); // This should work, since it has a copy of 'arg' above
    // Test functions taking non-trivial arguments inside classes
    ADerived d(10, 100);
    ABase *pDerived = &d;
    FunctionPointer1<void, MyArg> fp2(&d, &ADerived::print_virtual_arg);
    FunctionPointer1<void, MyArg> fp3(pDerived, &ABase::print_virtual_arg);
    call_fp1("ptr to virtual method taking non-tc argument", fp2, MyArg("notest", 5, 8));
    call_fp1("ptr to virtual method taking non-tc argument (via base class pointer)", fp2, MyArg("notest", 3, 4));
    call_event("direct event (non-tca)", Event(pDerived, &ABase::print_virtual_arg, MyArg("evttest", 5, 30)));
}

/******************************************************************************
 * Create an array of events from different kinds of function pointers
 * Call each one in turn (unified interface)
 *****************************************************************************/

static void test_array_of_events() {
    printf("\r\n********** Starting test_array_of_events **********\r\n");
    const char* testmsg1 = "Test message 1";
    const char* testmsg2 = "Test message 2";
    const int testint = 13;
    VDerived derived(20, 100);
    MyArg arg("array", 5, 10);

    FunctionPointer1<void, const char*> fp1((VBase*)&derived, &VBase::print_virtual_str);
    FunctionPointer0<void> fp2(sa_func_3);
    FunctionPointer1<void, int> fp3(sa_func_2);
    FunctionPointer0<void> fp4(&derived, &VDerived::print_virtual_noargs);
    FunctionPointer1<void, MyArg> fp5(sa_ntc);
    Event events[] = {fp1.bind(testmsg1), fp1.bind(testmsg2), fp2.bind(), fp3.bind(testint),
                      fp4.bind(), fp5.bind(arg)};

    for (unsigned i = 0; i < sizeof(events)/sizeof(events[0]); i ++) {
        events[i].call();
    }
}

/******************************************************************************
 * Test assignment between various kinds of events
 *****************************************************************************/

static void swap_events_using_eq(Event &e1, Event &e2) {
    Event temp;
    temp = e1;
    e1 = e2;
    e2 = temp;
}

static void swap_events_using_cc(Event &e1, Event &e2) {
    Event temp = e1;
    e1 = e2;
    e2 = temp;
}

static void test_event_assignment_and_swap() {
    printf("\r\n********** Starting test_event_assignment_and_swap **********\r\n");
    ADerived aderived(10, 10);
    FunctionPointer1<void, const char*> fp_sa_tc1(sa_func_1);
    FunctionPointer1<void, int> fp_sa_tc2(sa_func_2);
    FunctionPointer0<void> fp_sa_noargs(sa_func_3);
    FunctionPointer1<void, MyArg> fp_sa_ntc(sa_ntc);
    FunctionPointer1<void, MyArg> fp_class_ntc(&aderived, &ADerived::print_virtual_arg);
    MyArg arg("test_event_assignment");
    Event e_sa_tc1(fp_sa_tc1.bind("test_event_assignment"));
    Event e_sa_tc2(fp_sa_tc2.bind(17));
    Event e_sa_noargs(fp_sa_noargs.bind());
    Event e_sa_ntc(fp_sa_ntc.bind(arg));
    Event e_class_ntc(fp_class_ntc.bind(arg));
    Event e1 = e_sa_tc1, e2 = e_class_ntc, e3;
    e3 = e_sa_noargs;

    // Swap them around like crazy. I'm going to regret this in the morning.
    swap_events_using_eq(e1, e_sa_noargs); // e1: fp_sa_noargs, e_sa_noargs: fp_sa_tcl
    swap_events_using_cc(e2, e_class_ntc); // Intentional NOOP, e2 = e_class_ntc: fp_class_ntc
    swap_events_using_eq(e_sa_ntc, e_sa_tc2); // e_sa_ntc: fp_sa_tc2, e_sa_tc2: fp_sa_ntc
    swap_events_using_cc(e3, e_sa_tc1); //e3: fp_sa_tc1, e_sa_tc1: fp_sa_noargs
    swap_events_using_eq(e_sa_noargs, e2); // e_sa_noargs: fp_class_ntc, e2: fp_sa_tc1
    swap_events_using_cc(e_sa_tc2, e_class_ntc); // e_sa_tc2: fp_class_ntc, e_class_ntc: fp_sa_ntc
    // Final assignments:
    //  e_sa_tc1: fp_sa_noargs
    //  e_sa_tc2: fp_class_ntc
    //  e_sa_noargs: fp_class_ntc
    //  e_sa_ntc: fp_sa_tc2
    //  e_class_ntc: fp_sa_ntc
    //  e1: fp_sa_noargs
    //  e2: fp_sa_tc1
    //  e3: fp_sa_tc1

    // Now call all of them and prepare for a headache
    call_event("e_sa_tc1", e_sa_tc1);
    call_event("e_sa_tc2", e_sa_tc2);
    call_event("e_sa_noargs", e_sa_noargs);
    call_event("e_sa_ntc", e_sa_ntc);
    call_event("e_class_ntc", e_class_ntc);
    call_event("e1", e1);
    call_event("e2", e2);
    call_event("e3", e3);
}

/******************************************************************************
 * Entry point
 *****************************************************************************/

int main(void)
{
    printf("========== Starting event handler test ==========\r\n");
    test_standalone_funcs();
    test_class_funcs_tca();
    test_funcs_nontca();
    test_array_of_events();
    test_event_assignment_and_swap();

    printf ("Final MyArg instance count (should be 0): %d\r\n", MyArg::instcount);
    printf ("\r\nTest Complete\r\n");
    while (true);
    return 0;
}
