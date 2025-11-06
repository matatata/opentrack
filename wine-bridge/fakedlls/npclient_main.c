
#include <windows.h>

typedef void  tir_signature_t;
typedef void  tir_data_t;

#define NP_EXPORT(t) __declspec(dllexport) t __stdcall


NP_EXPORT(int) NPPriv_ClientNotify(void)
{
    return 0;
}

NP_EXPORT(int) NPPriv_GetLastError(void)
{
    return 0;
}


NP_EXPORT(int) NPPriv_SetData(void)
{
    return 0;
}

NP_EXPORT(int) NPPriv_SetLastError(void)
{
    return 0;
}


NP_EXPORT(int) NPPriv_SetParameter(void)
{
    return 0;
}


NP_EXPORT(int) NPPriv_SetSignature(void)
{
    return 0;
}


NP_EXPORT(int) NPPriv_SetVersion(void)
{
    return 0;
}

NP_EXPORT(int) NP_GetData(tir_data_t * data)
{
   return 0;
}


NP_EXPORT(int) NP_GetParameter(int arg0, int arg1)
{
    return 0;
}

NP_EXPORT(int) NP_GetSignature(tir_signature_t * sig)
{
    return 0;
}

NP_EXPORT(int) NP_QueryVersion(unsigned short * version)
{
    return 0;
}


NP_EXPORT(int) NP_ReCenter(void)
{
    return 0;
}



NP_EXPORT(int) NP_RegisterProgramProfileID(unsigned short id)
{
    return 0;
}


NP_EXPORT(int) NP_RegisterWindowHandle(HWND hwnd)
{
    return 0;
}


NP_EXPORT(int) NP_RequestData(unsigned short req)
{
    return 0;
}


NP_EXPORT(int) NP_SetParameter(int arg0, int arg1)
{
    return 0;
}


NP_EXPORT(int) NP_StartCursor(void)
{
   return 0;
}


NP_EXPORT(int) NP_StartDataTransmission(void)
{
    return 0;
}


NP_EXPORT(int) NP_StopCursor(void)
{
    return 0;
}


NP_EXPORT(int) NP_StopDataTransmission(void)
{
    return 0;
}


NP_EXPORT(int) NP_UnregisterWindowHandle(void)
{
    return 0;
}
