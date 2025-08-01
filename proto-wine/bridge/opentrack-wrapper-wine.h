
#include <stdint.h>
#include <windows.h>

WINAPI int otrdllmain(int protocol);
WINAPI void quit();
// 1=FT,2=NPClient,3=both
WINAPI void createRegistryKeys(int32_t x,int32_t selection);

WINAPI void clearRegistryKeys();

