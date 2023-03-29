#include <stdio.h>
#ifndef _countof
    #define _countof(x) (sizeof(x) / sizeof((x)[0]))
#endif

typedef int (*test_t)(int *);

int no_test(int * t) { *t = 0; return 0; }

int main()
{
    static test_t const testfns[] = {
        &no_test
    };
    int tests = 0;
    int succs = 0;
    for(unsigned i = 0; i < _countof(testfns); i++)
    {
        int tcount;
        succs += testfns[i](&tcount);
        tests += tcount;
    }

    fprintf(succs != tests ? stderr : stdout, "%d/%d tests passed.\n", succs, tests);
    return succs != tests;
}