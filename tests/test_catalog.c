#include "test.h"
#include "citadel/catalog.h"
#include <string.h>
int test_catalog(void){
 CitadelAppCatalog c;char out[256];unsigned i;char id[32];
 CHECK(citadel_catalog_init(&c)==CITADEL_OK);
 CHECK(citadel_catalog_format(&c,out,sizeof(out))==CITADEL_OK);
 CHECK(strstr(out,"No applications registered")!=NULL);
 CHECK(citadel_catalog_add(&c,"editor","Text Editor")==CITADEL_OK);
 CHECK(citadel_catalog_add(&c,"editor","Duplicate")==CITADEL_ERR_DENIED);
 CHECK(citadel_catalog_add(&c,"","Invalid")==CITADEL_ERR_INVALID_ARGUMENT);
 CHECK(citadel_catalog_add(&c,"bad","line\nbreak")==CITADEL_ERR_INVALID_ARGUMENT);
 CHECK(citadel_catalog_find(&c,"editor")!=NULL);
 CHECK(strcmp(citadel_catalog_find(&c,"editor")->name,"Text Editor")==0);
 CHECK(citadel_catalog_find(&c,"missing")==NULL);
 CHECK(citadel_catalog_format(&c,out,sizeof(out))==CITADEL_OK);
 CHECK(strstr(out,"Text Editor (editor)")!=NULL);
 CHECK(citadel_catalog_format(&c,out,8)==CITADEL_ERR_BUFFER_TOO_SMALL);
 for(i=1;i<CITADEL_CATALOG_CAPACITY;i++){
  snprintf(id,sizeof(id),"app%u",i);
  CHECK(citadel_catalog_add(&c,id,"Example")==CITADEL_OK);
 }
 CHECK(citadel_catalog_add(&c,"overflow","Example")==CITADEL_ERR_LIMIT);
 PASS();
}
