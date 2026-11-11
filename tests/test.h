#ifndef CITADEL_TEST_H
#define CITADEL_TEST_H
#include <stdio.h>
#include <stdlib.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"FAIL %s:%d: %s\n",__FILE__,__LINE__,#x); return 1; } } while (0)
#define PASS() do { puts("PASS"); return 0; } while (0)
#endif
