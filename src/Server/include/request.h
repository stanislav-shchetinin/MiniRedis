//
// Created by stass on 13.12.2023.
//

#ifndef MINIREDIS_REQUEST_H
#define MINIREDIS_REQUEST_H

#include <unistd.h>
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

int32_t request(int client_fd);

#endif //MINIREDIS_REQUEST_H
