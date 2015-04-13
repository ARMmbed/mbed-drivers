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
#include "Event.h"
#include "FunctionPointer.h"

class VBase {
public:
    virtual void vprint0(const char * msg) {
        printf("VBase0 msg: %s\r\n", msg);
    }
    virtual void vprint1(const char * msg) {
        printf("VBase1 msg: %s\r\n", msg);
    }
};
class VDerived : public VBase {
public:
    virtual void vprint1(const char * msg) {
        printf("VDerived1 msg: %s\r\n", msg);
    }
};

void bareprint(const char * msg) {
    if (msg) {
        printf("Bare msg: %s\r\n",msg);
    } else {
        printf("Bare Null Arg\r\n");
    }
}

int main(void)
{
    VDerived test;
    printf("Testing mbed Event Handler...\r\n");
    const char * message1 = "Message 1";
    const char * message2 = "Message 2";
    const char * message3 = "Message 3";

    FunctionPointer1<void,const char*> fp1((VBase*)&test, &VBase::vprint0);
    FunctionPointer1<void,const char*> fp2((VBase*)&test, &VBase::vprint1);
    FunctionPointer1<void,const char*> fp3(bareprint);

    fp1(message1);
    fp2(message1);
    fp3(message1);

    Event e1, e2, e3;
    if (fp1.bind(e1,message2)) {
        printf("failed to bind fp1\r\n");
    } else {
        e1.call();
    }

    if (fp2.bind(e2,message2)) {
        printf("failed to bind fp2\r\n");
    } else {
        e2.call();
    }

    if (fp3.bind(e3,message2)) {
        printf("failed to bind fp3\r\n");
    } else {
        e3.call();
    }

    Event events[3];
    FunctionPointer1<void, const char*> fps[3] = {fp1,fp2,fp3};
    for(int i = 0; i < 3; i++) {
        Event e;
        if (fps[i].bind(e,message3)) {
            printf("failed to bind fp%i\r\n",i);
        }
        events[i] = e;
    }
    for(int i = 0; i<3; i++) {
        events[i].call();
    }

    printf ("Test Complete\r\n");
    while(1){__WFI();}
    return 0;
}
