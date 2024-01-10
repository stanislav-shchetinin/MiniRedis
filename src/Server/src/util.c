#include "util.h"

#define ST_VAL 0x811C9DC5
#define P 0x01000193

uint64_t str_hash(const uint8_t *data, size_t len) {
    uint32_t h = ST_VAL;
    for (size_t i = 0; i < len; i++) {
        h = (h + data[i]) * P;
    }
    return h;
}