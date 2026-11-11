#include "test.h"
#include "citadel/sand_veyra.h"
#include <string.h>

int test_sand_veyra(void) {
    CitadelVeyraInterfaceInfo info;
    memset(&info, 0, sizeof(info));
    info.header.magic = CITADEL_VEYRA_MAGIC;
    info.header.abi_major = CITADEL_VEYRA_ABI_MAJOR;
    info.header.abi_minor = 7u; /* additive minor versions are accepted */
    info.header.struct_size = (uint32_t)sizeof(info);
    info.header.flags = 0x80000000u; /* unknown flags are ignored */
    info.interface_id = 42u;
    info.capabilities = CITADEL_VEYRA_CAP_READ;

    CHECK(citadel_veyra_validate_descriptor(&info, sizeof(info), 16u) == CITADEL_OK);
    CHECK(citadel_veyra_validate_descriptor(&info, 8u, 16u) == CITADEL_ERR_ABI_MISMATCH);
    CHECK(citadel_veyra_validate_descriptor(&info, sizeof(info), sizeof(info) + 1u) == CITADEL_ERR_ABI_MISMATCH);

    info.header.magic ^= 1u;
    CHECK(citadel_veyra_validate_descriptor(&info, sizeof(info), 16u) == CITADEL_ERR_ABI_MISMATCH);
    info.header.magic = CITADEL_VEYRA_MAGIC;
    info.header.abi_major++;
    CHECK(citadel_veyra_validate_descriptor(&info, sizeof(info), 16u) == CITADEL_ERR_ABI_MISMATCH);

    CHECK(citadel_veyra_map_status(0) == CITADEL_OK);
    CHECK(citadel_veyra_map_status(-6) == CITADEL_ERR_DENIED);
    CHECK(citadel_veyra_map_status(-9) == CITADEL_ERR_BUFFER_TOO_SMALL);
    CHECK(citadel_veyra_map_status(-999) == CITADEL_ERR_INTERNAL);
    PASS();
}
