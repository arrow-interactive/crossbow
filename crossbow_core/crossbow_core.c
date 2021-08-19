#include"crossbow_core.h"
#include<SDL2/SDL_opengl.h>
#include<SDL2/SDL_vulkan.h>
#include<SDL2/SDL_thread.h>

typedef enum CB_THREAD_ID_E { CB_PROCESS_THREAD, CB_LOGIC_THREAD, CB_RENDER_THREAD } cb_thread_id;
static SDL_Thread* cb_threads[3];

static cb_bool_t should_shutdown = CB_FALSE;
static cb_bool_t keypresses[SDL_NUM_SCANCODES];
static cb_scene_t* current_scene = NULL;
static int current_graphic_api = CB_GRAPHICS_GL;
static cb_bool_t changing_graphic_api = CB_FALSE;

static SDL_mutex* should_shutdown_mutex = NULL;
static SDL_mutex* keypresses_mutex = NULL;
static SDL_mutex* current_scene_mutex = NULL;
static SDL_mutex* current_graphic_api_mutex = NULL;
static SDL_mutex* changing_graphic_api_mutex = NULL;

static cb_info_t info;
static cb_tick_t tick;
static cb_frame_t frame;

static SDL_mutex* info_mutex = NULL;
static SDL_mutex* tick_mutex = NULL;
static SDL_mutex* frame_mutex = NULL;

cb_bool_t CB_IsPressed(SDL_Keycode key)
{
    SDL_LockMutex(keypresses_mutex);
    cb_bool_t ret = keypresses[SDL_GetScancodeFromKey(key)];
    SDL_UnlockMutex(keypresses_mutex);
    return ret;
}

static int CB_ProcessThread(void* data)
{
    Startup(info);

    for(;;)
    {
        SDL_LockMutex(should_shutdown_mutex);
        if(should_shutdown)
        {
            SDL_UnlockMutex(should_shutdown_mutex);
            break;
        }
        SDL_UnlockMutex(should_shutdown_mutex);
    }
    
    SDL_LockMutex(current_scene_mutex);
    if(current_scene != NULL)
        current_scene->on_unload(info);
    SDL_UnlockMutex(current_scene_mutex);

    Shutdown(info);

    return 0;
}

static int CB_LogicThread(void* data)
{
    uint64_t current_frame = SDL_GetTicks(), last_frame = 0;

    for(;;)
    {
        SDL_Event e;
        if(SDL_PollEvent(&e))
        {
            switch(e.type)
            {
                case SDL_QUIT:
                {
                    CB_ExitProgram();
                } break;

                case SDL_KEYDOWN:
                {
                    SDL_LockMutex(keypresses_mutex);
                    keypresses[e.key.keysym.scancode] = CB_TRUE;
                    SDL_UnlockMutex(keypresses_mutex);
                } break;

                case SDL_KEYUP:
                {
                    SDL_LockMutex(keypresses_mutex);
                    keypresses[e.key.keysym.scancode] = CB_FALSE;
                    SDL_UnlockMutex(keypresses_mutex);
                } break;
            }
        }

        Tick(tick);

        SDL_LockMutex(current_scene_mutex);
        if(current_scene != NULL)
            current_scene->on_tick(tick);
        SDL_UnlockMutex(current_scene_mutex);

        SDL_LockMutex(should_shutdown_mutex);
        if(should_shutdown)
        {
            SDL_UnlockMutex(should_shutdown_mutex);
            break;
        }
        SDL_UnlockMutex(should_shutdown_mutex);

        last_frame = current_frame;

        SDL_LockMutex(tick_mutex);
        current_frame = SDL_GetTicks();
        tick.deltaTime = (double)(current_frame - last_frame) / 1000.0;
        SDL_UnlockMutex(tick_mutex);
    }

    return 0;
}

static int CB_RenderThread(void** data)
{
    SDL_Window* window = (SDL_Window*)data[0];
    SDL_Renderer* renderer = (SDL_Renderer*)data[1];

    for(;;)
    {
        {
            SDL_LockMutex(changing_graphic_api_mutex);
            cb_bool_t changing_graph_api = changing_graphic_api;
            SDL_UnlockMutex(changing_graphic_api_mutex);

            if(changing_graph_api)
            {
                // Change Graphic api
            }
        }
            
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        Frame(frame);

        SDL_LockMutex(current_scene_mutex);
        if(current_scene != NULL)
            current_scene->on_frame(frame);
        SDL_UnlockMutex(current_scene_mutex);

        SDL_RenderPresent(renderer);

        SDL_LockMutex(should_shutdown_mutex);
        if(should_shutdown)
        {
            SDL_UnlockMutex(should_shutdown_mutex);
            break;
        }
        SDL_UnlockMutex(should_shutdown_mutex);

        {
            SDL_LockMutex(tick_mutex);
            cb_tick_t tick_tmp = tick;
            SDL_UnlockMutex(tick_mutex);

            SDL_LockMutex(frame_mutex);
            frame.tick = tick_tmp;
            SDL_UnlockMutex(frame_mutex);
        }
    }

    return 0;
}

void CB_LoadScene(cb_scene_t* scene)
{
    SDL_LockMutex(current_scene_mutex);
    if(current_scene != NULL)
        current_scene->on_unload(info);
    current_scene = scene;
    if(current_scene != NULL)
        current_scene->on_load(info);
    SDL_UnlockMutex(current_scene_mutex);
}

void CB_ExitProgram()
{
    SDL_LockMutex(should_shutdown_mutex);
    should_shutdown = CB_TRUE;
    SDL_UnlockMutex(should_shutdown_mutex);
}

int main(int argc, char* argv[])
{
    if(SDL_Init(SDL_INIT_EVERYTHING) == -1)
    {
        CB_LogFatal("Couldn't initialize SDL2: %s", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    SDL_Window* window = SDL_CreateWindow(TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
    if(window == NULL)
    {
        CB_LogFatal("Couldn't create window: %s", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if(renderer == NULL)
    {
        CB_LogFatal("Couldn't create renderer: %s", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    for(size_t i = 0; i < SDL_NUM_SCANCODES; i++)
        keypresses[i] = CB_FALSE;

    cb_path_t exedir;
    CB_GetExecutableDir(&exedir);
    #ifdef _WIN32
    _chdir(exedir);
    #else
    chdir(exedir);
    #endif

    should_shutdown_mutex = SDL_CreateMutex();
    keypresses_mutex = SDL_CreateMutex();
    current_scene_mutex = SDL_CreateMutex();
    current_graphic_api_mutex = SDL_CreateMutex();
    changing_graphic_api_mutex = SDL_CreateMutex();

    info_mutex = SDL_CreateMutex();
    tick_mutex = SDL_CreateMutex();
    frame_mutex = SDL_CreateMutex();

    info.argc = argc;
    info.argv = argv;

    tick.deltaTime = 0;

    frame.tick = tick;
    frame.graphic_api = CB_GRAPHICS_GL;

    cb_threads[CB_PROCESS_THREAD] = SDL_CreateThread(CB_ProcessThread, "ProcessThread", NULL);
    cb_threads[CB_LOGIC_THREAD] = SDL_CreateThread(CB_LogicThread, "LogicThread", NULL);
    cb_threads[CB_RENDER_THREAD] = SDL_CreateThread((SDL_ThreadFunction)CB_RenderThread, "RenderThread", (void*[]){ window, renderer });

    for(;;)
    {
        SDL_LockMutex(should_shutdown_mutex);
        if(should_shutdown)
        {
            SDL_UnlockMutex(should_shutdown_mutex);
            break;
        }
        SDL_UnlockMutex(should_shutdown_mutex);
    }

    SDL_WaitThread(cb_threads[CB_PROCESS_THREAD], NULL);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
