#ifndef MINIREDIS_UTIL_H
#define MINIREDIS_UTIL_H

#include <stdint.h>
#include <unistd.h>

uint64_t str_hash(const uint8_t *data, size_t len);

#endif //MINIREDIS_UTIL_H
