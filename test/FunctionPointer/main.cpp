
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
