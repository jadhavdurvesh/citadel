#include "test.h"
#include "citadel/host.h"
#include "demo_adapter.h"
#include "native_adapter.h"
#include <string.h>
int test_host(void){CitadelSystemSnapshot s;CitadelFileSearchResult f;CitadelRegistry r;CitadelPolicy p;CitadelCommand c;CitadelResult out;
 CHECK(citadel_host_system_snapshot(&s)==CITADEL_OK);CHECK(s.logical_cpu_count>0);
 CHECK(citadel_host_search_files("..","CMakeLists",&f)==CITADEL_OK);CHECK(f.count>0);
 CHECK(citadel_host_search_files(NULL,"x",&f)==CITADEL_ERR_INVALID_ARGUMENT);
 CHECK(citadel_registry_init(&r)==CITADEL_OK);CHECK(citadel_demo_register(&r)==CITADEL_OK);CHECK(citadel_native_register(&r)==CITADEL_OK);
 p.granted_permissions=CITADEL_PERMISSION_READ_SYSTEM;p.confirmation_granted=0;
 CHECK(citadel_parse("capabilities list",17,&c)==CITADEL_OK);CHECK(citadel_dispatch(&r,&c,&p,&out)==CITADEL_OK);
 CHECK(strstr(out.text,"system.status")!=NULL);CHECK(strstr(out.text,"system.open_settings")==NULL);
 CHECK(citadel_parse("open settings",13,&c)==CITADEL_OK);CHECK(citadel_dispatch(&r,&c,&p,&out)==CITADEL_ERR_DENIED);PASS();}
