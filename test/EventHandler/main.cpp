
#include "mbed.h"
#include "EventHandler.h"
class VTest {
public:
    int vprint(void *arg) {
        if (arg) {
            int* i = static_cast<int *>(arg);
            printf("Class Print Integer: %d from %p\r\n", *i, arg);
        } else {
            printf("Class Null Arg\r\n");
        }
        return (int)(arg != 0);
    }

};

int barevprint(void *arg) {
    if (arg) {
        int* i = static_cast<int *>(arg);
        printf("Bare Print Integer: %d from %p\r\n", *i, arg);
    } else {
        printf("Class Null Arg\r\n");
    }
    return (int)(arg != 0);
}

int main(void)
{
    VTest test;
    printf("Testing mbed Event Handler...\r\n");
    int i1 = 1;
    int i2 = 2;
    EventHandler ebp(barevprint);
    EventHandler ecp(&test, &VTest::vprint);
    size_t ebsize = sizeof(ebp);
    size_t ecsize = sizeof(ecp);
    printf("sizeof(bp) = %d\r\n", ebsize);
    printf("sizeof(cp) = %d\r\n", ecsize);
    ebp.call(&i1);
    ecp.call(&i2);

    printf ("Test Complete\r\n");
    while(1){__WFI();}
    return 0;
}
