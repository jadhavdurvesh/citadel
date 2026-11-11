#include "citadel/citadel.h"
#include "demo_adapter.h"
#include <stdio.h>
#include <string.h>

int main(void) {
    CitadelRegistry registry;
    CitadelPolicy policy;
    char line[CITADEL_MAX_INPUT + 2];
    CitadelCommand command;
    CitadelResult result;

    puts("Citadel 0.1.0 - offline assistant core. Type help or exit.");
    if (citadel_registry_init(&registry) != CITADEL_OK ||
        citadel_demo_register(&registry) != CITADEL_OK) {
        fputs("Initialization failed.\n", stderr);
        return 1;
    }

    policy.granted_permissions = CITADEL_PERMISSION_LAUNCH_APP |
                                 CITADEL_PERMISSION_READ_SYSTEM;
    policy.confirmation_granted = 0;

    for (;;) {
        size_t length;
        fputs("citadel> ", stdout);
        fflush(stdout);
        if (fgets(line, sizeof(line), stdin) == NULL) break;
        length = strlen(line);
        if (length > 0 && (unsigned char)line[length - 1] == 10u)
            line[--length] = 0;
        if (strcmp(line, "exit") == 0 || strcmp(line, "quit") == 0) break;
        if (citadel_parse(line, length, &command) != CITADEL_OK) {
            puts("Invalid or oversized command.");
            continue;
        }
        if (citadel_dispatch(&registry, &command, &policy, &result) == CITADEL_OK)
            puts(result.text);
        else
            printf("Rejected (status %d).\n", (int)result.status);
    }
    puts("Citadel stopped.");
    return 0;
}
