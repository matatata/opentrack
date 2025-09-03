#include <stdio.h>
#include <stdbool.h>

#include <limits.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/file.h>
#include <sys/mman.h>
#include <unistd.h>

#include "opentrackclient_private.h"

#define EXPORT __attribute__((visibility("default")))

// #define DEBUG 1

#if DEBUG
#define dbg_report(...) { printf(__VA_ARGS__); }
#else
#define dbg_report(...)
#endif

static shm_wrapper *wrapper = 0;

__attribute__((constructor))
static void initializer(void) {
    dbg_report("opentrackclient library: initializer called\n");
}

__attribute__((destructor))
static void finalizer(void) {
    dbg_report("opentrackclient library: finalizer called\n");
}

shm_wrapper* _shm_wrapper_init(const char *shm_name, const char *mutex_name, int mapSize)
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
    // void volatile* restrict ptr = self->mem;
    // for (size_t i = 0; i < sizeof(WineSHM); i++)
    //     *((char volatile* restrict)ptr + i) = 0;
    // asm volatile("" ::: "memory");

    return self;
}


shm_wrapper* shm_wrapper_init(){
    return _shm_wrapper_init(WINE_SHM_NAME, WINE_MTX_NAME, sizeof(WineSHM));
}

void shm_wrapper_free(shm_wrapper* self)
{
    dbg_report("freed shm_wrapper\n");
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

// TODO use atomic instead of (b)locking
// #include <stdatomic.h>

EXPORT bool OTRGetHeadpose(OTRHeadpose *headpose)
{
    OTR_Priv_Lock();
    WineSHM *shm_posix = OTR_Priv_GetSHM();
    if(shm_posix){
        OTRHeadpose* data = headpose;
        data->Yaw = shm_posix->data[Yaw];
        data->Pitch = shm_posix->data[Pitch];
        data->Roll = - shm_posix->data[Roll];
        data->X = - shm_posix->data[TX];
        data->Y = shm_posix->data[TY];
        data->Z = shm_posix->data[TZ];
    }
    OTR_Priv_Unlock();
    return true;
}


EXPORT bool OTRInit() {
    dbg_report("OTRInit called()\n");
    if(wrapper != NULL)
        OTRClose();
    wrapper = shm_wrapper_init();
    return wrapper != NULL;
}

EXPORT void OTRClose() {
    dbg_report("OTRClose called()\n");
    if(wrapper!=NULL)
        shm_wrapper_free(wrapper);
    wrapper = NULL;
}

EXPORT void OTR_Priv_Lock()
{
    shm_wrapper_lock(wrapper);
}

EXPORT void OTR_Priv_Unlock()
{
    shm_wrapper_unlock(wrapper);
}

EXPORT WineSHM *OTR_Priv_GetSHM()
{
    return (WineSHM*)wrapper->mem;
}

