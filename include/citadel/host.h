#ifndef CITADEL_HOST_H
#define CITADEL_HOST_H
#include <stddef.h>
#include <stdint.h>
#include "citadel.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct CitadelSystemSnapshot { uint64_t total_memory_bytes,available_memory_bytes; uint32_t logical_cpu_count; } CitadelSystemSnapshot;
#define CITADEL_FILE_SEARCH_MAX_RESULTS 16u
#define CITADEL_FILE_SEARCH_MAX_DEPTH 8u
typedef struct CitadelFileSearchResult { uint32_t count; char paths[CITADEL_FILE_SEARCH_MAX_RESULTS][260]; uint8_t truncated; } CitadelFileSearchResult;
CitadelStatus citadel_host_system_snapshot(CitadelSystemSnapshot *);
CitadelStatus citadel_host_search_files(const char *,const char *,CitadelFileSearchResult *);
#ifdef __cplusplus
}
#endif
#endif
