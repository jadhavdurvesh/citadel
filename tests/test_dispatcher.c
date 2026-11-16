#include "test.h"
#include "citadel/citadel.h"
#include <string.h>

static CitadelStatus run(const CitadelCommand *command, CitadelResult *result, void *user_data) {
    (void)command;
    (void)user_data;
    strcpy(result->text, "executed");
    return CITADEL_OK;
}

int test_dispatcher(void) {
    CitadelRegistry registry;
    CitadelCapability descriptor;
    CitadelPolicy policy;
    CitadelCommand command;
    CitadelResult result;
    const char *phrase = "change setting";
    const char *extra = "change setting now";

    memset(&descriptor, 0, sizeof(descriptor));
    strcpy(descriptor.id, "sys.change");
    descriptor.required_permissions = CITADEL_PERMISSION_MODIFY_SETTINGS;
    descriptor.requires_confirmation = 1;

    CHECK(citadel_registry_init(&registry) == CITADEL_OK);
    CHECK(citadel_register_capability(&registry, &descriptor, run, NULL) == CITADEL_OK);
    CHECK(citadel_register_intent(&registry, "intent.change", phrase, "sys.change") == CITADEL_OK);
    CHECK(citadel_parse(phrase, strlen(phrase), &command) == CITADEL_OK);

    policy.granted_permissions = CITADEL_PERMISSION_MODIFY_SETTINGS;
    policy.confirmation_granted = 0;
    CHECK(citadel_dispatch(&registry, &command, &policy, &result) == CITADEL_ERR_DENIED);

    policy.confirmation_granted = 1;
    CHECK(citadel_dispatch(&registry, &command, &policy, &result) == CITADEL_OK);
    CHECK(strcmp(result.text, "executed") == 0);

    CHECK(citadel_parse(extra, strlen(extra), &command) == CITADEL_OK);
    CHECK(citadel_dispatch(&registry, &command, &policy, &result) == CITADEL_ERR_NOT_FOUND);
    
    /* trailing wildcard intents */
    {
        CitadelRegistry r2; CitadelCapability d2; CitadelPolicy open_policy = { 0u, 0 };
        const char *good = "open locus", *two = "open a b", *bare = "open", *other = "close locus";
        memset(&d2, 0, sizeof(d2)); strcpy(d2.id, "app.open");
        CHECK(citadel_registry_init(&r2) == CITADEL_OK);
        CHECK(citadel_register_capability(&r2, &d2, run, NULL) == CITADEL_OK);
        CHECK(citadel_register_intent(&r2, "intent.open", "open *", "app.open") == CITADEL_OK);
        CHECK(citadel_register_intent(&r2, "i.bad1", "* open", "app.open") == CITADEL_ERR_INVALID_ARGUMENT);
        CHECK(citadel_register_intent(&r2, "i.bad2", "*", "app.open") == CITADEL_ERR_INVALID_ARGUMENT);
        CHECK(citadel_register_intent(&r2, "i.bad3", "open * now", "app.open") == CITADEL_ERR_INVALID_ARGUMENT);
        CHECK(citadel_register_intent(&r2, "i.bad4", "op*n x", "app.open") == CITADEL_ERR_INVALID_ARGUMENT);
        CHECK(citadel_parse(good, strlen(good), &command) == CITADEL_OK);
        CHECK(citadel_dispatch(&r2, &command, &open_policy, &result) == CITADEL_OK);
        CHECK(citadel_parse(two, strlen(two), &command) == CITADEL_OK);
        CHECK(citadel_dispatch(&r2, &command, &open_policy, &result) == CITADEL_OK);       /* several arguments */
        CHECK(citadel_parse(bare, strlen(bare), &command) == CITADEL_OK);
        CHECK(citadel_dispatch(&r2, &command, &open_policy, &result) == CITADEL_ERR_NOT_FOUND);   /* needs an argument */
        CHECK(citadel_parse(other, strlen(other), &command) == CITADEL_OK);
        CHECK(citadel_dispatch(&r2, &command, &open_policy, &result) == CITADEL_ERR_NOT_FOUND);
    }
    PASS();
}
