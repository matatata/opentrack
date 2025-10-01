/* Copyright (c) 2025 Matteo Ceruti <matteo.ceruti@gmail.com>
 *
 * Based in parts on the work of Stanislaw Halik:
 *
 * Copyright (c) 2013 Stanislaw Halik <sthalik@misaki.pl>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 */
#include <stdio.h>

#include <limits.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/file.h>
#include <sys/mman.h>
#include <unistd.h>


#include "shm.h"

shm_wrapper* shm_wrapper_init(const char *shm_name, const char *mutex_name, int mapSize)
{
    (void)mutex_name;
    shm_wrapper* self = malloc(sizeof(shm_wrapper));
    char shm_filename[NAME_MAX];
    shm_filename[0] = '/';
    strncpy(shm_filename+1, shm_name, NAME_MAX-2);
    shm_filename[NAME_MAX-1] = '\0';
    /* (void) shm_unlink(shm_filename); */

    self->fd = shm_open(shm_filename, O_RDWR | O_CREAT, 0600);
    if(self->fd == -1){
        fprintf(stderr,"shm_open failed errno = %i\n",errno);
        free(self);
        return NULL;
    }
    ftruncate(self->fd, mapSize);

    self->mem = mmap(NULL, mapSize, PROT_READ|PROT_WRITE, MAP_SHARED, self->fd, (off_t)0);

    if(self->mem == MAP_FAILED ){
        fprintf(stderr,"mmap failed errno = %i\n",errno);
        free(self);
        return NULL;
    }

    // I'm not sure why we would need to clear the shm, but I'll keee the snippet:
    void volatile* restrict ptr = self->mem;
    for (size_t i = 0; i < mapSize; i++)
        *((char volatile* restrict)ptr + i) = 0;
    asm volatile("" ::: "memory");

    return self;
}

void shm_wrapper_free(shm_wrapper* self)
{
    /*(void) shm_unlink(shm_filename);*/
    (void) munmap(self->mem, self->size);
    (void) close(self->fd);
    free(self);
}

void shm_wrapper_lock(shm_wrapper* self)
{
    flock(self->fd, LOCK_SH);
}

void shm_wrapper_unlock(shm_wrapper* self)
{
    flock(self->fd, LOCK_UN);
}


