
#include <stdint.h>
#include <windows.h>

WINAPI int mainloop(int protocol);
WINAPI void quit();
// 1=FT,2=NPClient,3=both
WINAPI void createRegistryKeys(int32_t x,int32_t selection);

// WINAPI void getHeadpose(double *yaw,double *pitch,double *roll,double *x,double *y,double *z);
WINAPI bool getDllPath(char *dir,uint32_t bufSize);
WINAPI void clearRegistryKeys();

