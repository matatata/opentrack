/* Copyright (c) 2013 Stanislaw Halik <sthalik@misaki.pl>
 *
 * 2025 matatata github.com/matatata/opentrack factored out C-library
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 */
#ifndef SHM_WRAPPER_H
#define SHM_WRAPPER_H


#if defined(_WIN32)
#include <windows.h>
#else
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/file.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <limits.h>
#include <unistd.h>
#include <sys/types.h>
#endif


#ifdef __cplusplus
extern "C"
{
#endif

typedef struct shm_wrapper_t
{
    void* mem;
#if defined(_WIN32)
    HANDLE mutex, mapped_file;
#else
    int fd;
    unsigned size;
#endif
} shm_wrapper_t;

bool shm_wrapper_init(shm_wrapper_t* self,const char *shm_name, const char *mutex_name, int mapSize,bool memset_zero);
// inline void* ptr(shm_wrapper_t* self) { return self->mem; }
void shm_wrapper_destroy(shm_wrapper_t* self);
#if defined(_WIN32)
bool shm_wrapper_lock(shm_wrapper_t* self);
#else
typedef enum {
    EXCLUSIVE=0,
    SHARED
} shm_lock_t;
bool shm_wrapper_lock(shm_wrapper_t* self,shm_lock_t shared);
#endif
bool shm_wrapper_unlock(shm_wrapper_t* self);

bool shm_wrapper_success(shm_wrapper_t *self);


#ifdef __cplusplus
}
#endif


#endif // SHM_WRAPPER_H
