#include"crossbow_fs.h"
#include<string.h>
#include<stdio.h>

#ifdef _WIN32
#include<windows.h>
#include<Shlwapi.h>
#include<io.h> 

#define access _access_s
#endif
 
#ifdef __APPLE__
#define _XOPEN_SOURCE 700
#define __USE_XOPEN_EXTENDED 1
#include<libgen.h>
#include<mach-o/dyld.h>
#endif
 
#ifdef __linux__
#define _XOPEN_SOURCE 700
#define __USE_XOPEN_EXTENDED 1
#include<stdlib.h>
#include<libgen.h>

#ifdef __sun
#define PROC_SELF_EXE "/proc/self/path/a.out"
#else
#define PROC_SELF_EXE "/proc/self/exe"
#endif

#endif

#ifdef _BSD
#define _XOPEN_SOURCE 700
#define __USE_XOPEN_EXTENDED 1
#include<libgen.h>

#define PROC_SELF_EXE "/proc/curproc/file"      /* Requires procfs on FreeBSD */
#endif

void CB_RemoveFilename(cb_path_t path, cb_path_t* out)
{
    size_t i;
    for(i = strlen(path) - 1; i >= 0; i--)
    {
        #ifdef _WIN32
        if(path[i] == '/' || path[i] == '\\')
            break;
        #else
        if(path[i] == '/')
            break;
        #endif
    }

    path[i + 1] = 0;

    memcpy(*out, path, i + 1);
}
 
#ifdef _WIN32
 
void CB_GetExecutablePath(cb_path_t* out)
{
    GetModuleFileNameA(NULL, *out, PATH_MAX);
}
 
#endif
 
#ifdef __APPLE__
void CB_GetExecutablePath(cb_path_t* out)
{
    char raw_out[PATH_MAX];
    uint32_t raw_out_size = (uint32_t)sizeof(raw_out);

    if(!_NSGetExecutablePath(raw_out, &raw_out_size))
    {
        realpath(raw_out, *out);
    }
}
#endif

#ifdef __linux__
 
void CB_GetExecutablePath(cb_path_t* out)
{
    realpath(PROC_SELF_EXE, *out);
}
 
#endif

#ifdef _BSD
 
void CB_GetExecutablePath(cb_path_t* out)
{
    realpath(PROC_SELF_EXE, *out);
}
 
#endif

void CB_GetExecutableDir(cb_path_t* out)
{
    CB_GetExecutablePath(out);
    CB_RemoveFilename(*out, out);
}
