//
// Created by stass on 02.01.2024.
//

#ifndef MINIREDIS_QUERY_H
#define MINIREDIS_QUERY_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

int32_t query(int fd, const char *text);

#endif //MINIREDIS_QUERY_H
