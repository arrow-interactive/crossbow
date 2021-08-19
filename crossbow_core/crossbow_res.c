#include"crossbow_res.h"
#include<stdio.h>
#include<string.h>
#include<stdint.h>
#include<stdlib.h>

cb_asset_t CB_LoadAsset(cb_path_t filepath, cb_bool_t binary_load)
{
    cb_path_t exe_dir;
    CB_GetExecutableDir(&exe_dir);
    cb_path_t path;
    strcpy(path, exe_dir);
    strcat(path, ASSETS_PATH);
    strcat(path, filepath);

    FILE* file = fopen(path, binary_load ? "rb" : "r");
    if(!file)
        return (cb_asset_t){ 0, NULL };

    cb_asset_t ret;
    fseek(file, 0, SEEK_END);
    ret.size = ftell(file);
    fseek(file, 0, SEEK_SET);

    ret.data = malloc(ret.size);
    fread(ret.data, 1, ret.size, file);

    fclose(file);

    return ret;
}
