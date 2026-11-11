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
    PASS();
}
