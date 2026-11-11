#include "test.h"
int test_parser(void); int test_registry(void); int test_dispatcher(void); int test_provider(void);
int main(void) {
 int failures=0; puts("Citadel M1 tests");
 failures+=test_parser(); failures+=test_registry(); failures+=test_dispatcher(); failures+=test_provider();
 if (failures) fprintf(stderr,"%d test group(s) failed\n",failures); else puts("All test groups passed.");
 return failures?EXIT_FAILURE:EXIT_SUCCESS;
}
