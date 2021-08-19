#ifndef ARROW_INTERACTIVE_CROSSBOW_RES_H
#define ARROW_INTERACTIVE_CROSSBOW_RES_H

#ifdef __cplusplus
extern "C"
{
#endif
#include"crossbow_fs.h"
#include"crossbow_bool.h"

#include<stdint.h>

typedef struct CB_ASSET_S
{
    size_t size;
    uint8_t* data;
} cb_asset_t;

cb_asset_t CB_LoadAsset(cb_path_t filepath, cb_bool_t binary_load);
#define CB_FreeAsset(asset) free(asset.data);

/* Assets path that can be relative to the executable directory or a real path */
extern const char* ASSETS_PATH;

#ifdef __cplusplus
}
#endif

#endif
