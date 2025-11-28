#include <windows.h>

typedef void  FTData;

#define EXPORT(t) __declspec(dllexport) t __stdcall


EXPORT(BOOL) FTGetData(FTData* data)
{
    return FALSE;
}


EXPORT(void) FTReportName( int name )
{

}

EXPORT(void) FTReportID( int name )
{

}

EXPORT(const char*) FTGetDllVersion(void)
{
    return "0.0.0.0";
}

EXPORT(const char*) FTProvider(void)
{
    return "FakeDll";
}
