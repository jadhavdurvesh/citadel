#include "test.h"
#include "citadel/host.h"
int test_host(void){CitadelSystemSnapshot s;CitadelFileSearchResult f;
 CHECK(citadel_host_system_snapshot(&s)==CITADEL_OK);CHECK(s.logical_cpu_count>0);
 CHECK(citadel_host_search_files(".","CMakeLists",&f)==CITADEL_OK);CHECK(f.count>0);
 CHECK(citadel_host_search_files(NULL,"x",&f)==CITADEL_ERR_INVALID_ARGUMENT);PASS();}
