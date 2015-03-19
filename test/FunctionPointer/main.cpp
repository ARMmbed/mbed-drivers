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
#include "FunctionPointer.h"
class VTest {
public:
    void print() {
        printf("Class Print\r\n");
    }

};

void bareprint() {
    printf("Bare Print\r\n");
}

int main(void)
{
    VTest test;
    printf("Testing mbed FunctionPointer...\r\n");

    FunctionPointer ebp(bareprint);
    FunctionPointer ecp(&test, &VTest::print);

    size_t ebsize = sizeof(ebp);
    size_t ecsize = sizeof(ecp);
    printf("sizeof(bp) = %d\r\n", ebsize);
    printf("sizeof(cp) = %d\r\n", ecsize);
    ebp.call();
    ecp.call();

    printf ("Test Complete\r\n");
    while(1){__WFI();}
    return 0;
}
