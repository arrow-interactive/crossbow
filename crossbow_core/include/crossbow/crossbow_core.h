#ifndef ARROW_INTERACTIVE_CROSSBOW_CORE_H
#define ARROW_INTERACTIVE_CROSSBOW_CORE_H

#ifdef __cplusplus
extern "C"
{
#endif
#include"crossbow_bool.h"
#include"crossbow_log.h"
#include"crossbow_fs.h"
#include"crossbow_res.h"

#include<stdint.h>
#include<SDL2/SDL.h>

typedef struct CB_INFO_S
{
    int argc;
    char** argv;
    // ...
} cb_info_t;

typedef struct CB_TICk_S
{
    double deltaTime;
    // ...
} cb_tick_t;

typedef struct CB_FRAME_S
{
    cb_tick_t tick;
    enum
    {
        CB_GRAPHICS_GL,
        CB_GRAPHICS_VK
    } graphic_api;
    // ...
} cb_frame_t;

typedef struct CB_SCENE_S
{
    void (*on_load)(cb_info_t info);
    void (*on_unload)(cb_info_t info);
    void (*on_tick)(cb_tick_t tick);
    void (*on_frame)(cb_frame_t frame);
} cb_scene_t;

extern void Startup(cb_info_t info);
extern void Shutdown(cb_info_t info);
extern void Tick(cb_tick_t tick);
extern void Frame(cb_frame_t frame);

/* Main window width */
extern const uint64_t WIDTH;
/* Main window height */
extern const uint64_t HEIGHT;

/* Main window title */
extern const char* TITLE;

cb_bool_t CB_IsPressed(SDL_Keycode key);
void CB_LoadScene(cb_scene_t* scene);
void CB_ExitProgram();

#ifdef __cplusplus
}
#endif

#endif
