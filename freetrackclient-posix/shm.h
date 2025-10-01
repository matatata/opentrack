#ifndef SHM_H
#define SHM_H

// This is highly coupled with compat/shm.cpp and shm.h
// but I chose to use plain C to save me to pull in std++ libraries.

typedef struct shm_wrapper
{
    void* mem;
    int fd;
    unsigned size;
} shm_wrapper;


shm_wrapper* shm_wrapper_init(const char *shm_name, const char *mutex_name, int mapSize);
void shm_wrapper_free(shm_wrapper* self);
void shm_wrapper_lock(shm_wrapper* self);
void shm_wrapper_unlock(shm_wrapper* self);
#endif
