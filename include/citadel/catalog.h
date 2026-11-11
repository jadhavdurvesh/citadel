#ifndef CITADEL_CATALOG_H
#define CITADEL_CATALOG_H
#include "citadel.h"
#ifdef __cplusplus
extern "C" {
#endif
#define CITADEL_CATALOG_CAPACITY 32u
#define CITADEL_APP_NAME_MAX 64u
typedef struct CitadelAppEntry {
 char id[CITADEL_ID_LENGTH];
 char name[CITADEL_APP_NAME_MAX];
} CitadelAppEntry;
typedef struct CitadelAppCatalog {
 uint32_t count;
 CitadelAppEntry entries[CITADEL_CATALOG_CAPACITY];
} CitadelAppCatalog;
CitadelStatus citadel_catalog_init(CitadelAppCatalog *catalog);
CitadelStatus citadel_catalog_add(CitadelAppCatalog *catalog, const char *id, const char *name);
const CitadelAppEntry *citadel_catalog_find(const CitadelAppCatalog *catalog, const char *id);
CitadelStatus citadel_catalog_format(const CitadelAppCatalog *catalog, char *out, size_t capacity);
#ifdef __cplusplus
}
#endif
#endif
