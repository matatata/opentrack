/* Copyright (c) 2013 Stanislaw Halik <sthalik@misaki.pl>

 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 */

#include "shm.hpp"

// #include "shm.c"

shm_wrapper::shm_wrapper(const char *shm_name, const char* mutex_name, int map_size)
{
    shm_wrapper_init(this,shm_name,mutex_name,map_size,false);
}

shm_wrapper::~shm_wrapper()
{
    shm_wrapper_destroy(this);
}

bool shm_wrapper::lock()
{
#ifdef _WIN32
    return shm_wrapper_lock(this);
#else
    return shm_wrapper_lock(this,EXCLUSIVE);
#endif
}

bool shm_wrapper::unlock()
{
    return shm_wrapper_unlock(this);
}

bool shm_wrapper::success()
{
    return shm_wrapper_success(this);
}

