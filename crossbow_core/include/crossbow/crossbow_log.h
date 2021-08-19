#ifndef ARROW_INTERACTIVE_CROSSBOW_CB_Log_H
#define ARROW_INTERACTIVE_CROSSBOW_CB_Log_H

#ifdef __cplusplus
extern "C"
{
#endif
#include"crossbow_bool.h"

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<stdarg.h>

#ifdef __unix__

#define CB_LogInfo(fmt, ...)    CB_Log("\033[94minfo\033[0m", __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define CB_LogDebug(fmt, ...)   CB_Log("\033[96mdebug\033[0m", __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define CB_LogTest(fmt, ...)    CB_Log("\033[92mtest\033[0m", __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define CB_LogWarn(fmt, ...)    CB_Log("\033[93mwarn\033[0m", __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define CB_LogError(fmt, ...)   CB_Log("\033[91merror\033[0m", __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define CB_LogFatal(fmt, ...)   CB_Log("\033[31mfatal\033[0m", __FILE__, __LINE__, fmt, ##__VA_ARGS__)

#else

#define CB_LogInfo(fmt, ...)    CB_Log("info", __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define CB_LogDebug(fmt, ...)   CB_Log("debug", __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define CB_LogTest(fmt, ...)    CB_Log("test", __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define CB_LogWarn(fmt, ...)    CB_Log("warn", __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define CB_LogError(fmt, ...)   CB_Log("error", __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define CB_LogFatal(fmt, ...)   CB_Log("fatal", __FILE__, __LINE__, fmt, ##__VA_ARGS__)

#endif


static void CB_Log(const char* logstatus, const char* file, int line, const char* format, ...)
{
    va_list vl;
    va_start(vl, format);

    char* targ = (char*)malloc(strlen(format) + 256);
    vsprintf(targ, format, vl);

    printf("[%s:%s:%d] %s\n", logstatus, file, line, targ);

    free(targ);

    va_end(vl);
}

#ifdef __cplusplus
}
#endif

#endif
