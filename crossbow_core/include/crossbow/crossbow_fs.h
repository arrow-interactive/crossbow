#ifndef ARROW_INTERACTIVE_CROSSBOW_FS_H
#define ARROW_INTERACTIVE_CROSSBOW_FS_H

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef _WIN32
#include<windows.h>
#define PATH_MAX MAX_PATH
#endif
 
#ifdef __APPLE__
#include<sys/syslimits.h>
#include<unistd.h>
#endif
 
#ifdef __linux__
#include<linux/limits.h>
#include<unistd.h>
#endif

#ifdef _BSD
#include<sys/syslimits.h>
#include<unistd.h>
#endif

typedef char cb_path_t[PATH_MAX];
void CB_RemoveFilename(cb_path_t path, cb_path_t* out);
void CB_GetExecutablePath(cb_path_t* out);
void CB_GetExecutableDir(cb_path_t* out);

#ifdef __cplusplus
}
#endif

#endif
