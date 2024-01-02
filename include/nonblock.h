//
// Created by stass on 02.01.2024.
//

#ifndef MINIREDIS_NONBLOCK_H
#define MINIREDIS_NONBLOCK_H

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>

void fd_set_nb(int fd);

#endif //MINIREDIS_NONBLOCK_H
