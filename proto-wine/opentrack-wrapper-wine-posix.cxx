#ifdef _WIN32
#   undef _WIN32
#endif

#define shm_wrapper ShmPosix
#define shm_wrapper_t shm_posix_wrapper_t
#define shm_wrapper_init shm_posix_wrapper_init
#define shm_wrapper_lock shm_posix_wrapper_lock
#define shm_wrapper_unlock shm_posix_wrapper_unlock
#define shm_wrapper_destroy shm_posix_wrapper_destroy
#define shm_warn shm_posix_warn
#define shm_wrapper_success shm_posix_wrapper_success
#include "compat/shm.hpp"
#include "compat/shm.cpp"
