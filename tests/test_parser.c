#include "test.h"
#include "citadel/citadel.h"
#include <string.h>
int test_parser(void) {
 CitadelCommand c; const char *s="open settings"; char large[CITADEL_MAX_INPUT+1];
 CHECK(citadel_parse(s,strlen(s),&c)==CITADEL_OK);
 CHECK(c.token_count==2); CHECK(c.tokens[0].length==4);
 CHECK(citadel_parse("",0,&c)!=CITADEL_OK);
 memset(large,'x',sizeof(large));
 CHECK(citadel_parse(large,sizeof(large),&c)==CITADEL_ERR_LIMIT);
 PASS();
}
