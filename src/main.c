#include <genesis.h>
#include "../res/resources.h"

#include "../inc/global.h"
#include "../inc/camera.h"
#include "../inc/player.h"
#include "../inc/control.h"

u16 ind = TILE_USER_INDEX;
bool debug_mode = FALSE;
u16 palette[64];

typedef struct
{
    Map *map_fg;
    Map *map_bg;

    u16 width;
    u16 height;

    u16 ind_tileset;
    u16 curr_level;
} Level;

Level level = {
    .map_fg = NULL,
    .map_bg = NULL,

    .width = 640,
    .height = 448,

    .ind_tileset = TILE_USER_INDEX,
    .curr_level = 0,
};

void controlHandle(u16 joy, u16 changed, u16 state);
void controlHandle2(void);
void levelChange(u16 new_level);
void fadeOut(void);
void cameraSetPosition(s16 x, s16 y);
void cameraUpdate();

int main()
{

    SYS_disableInts();
    VDP_setScreenWidth256();
    SPR_init();
    JOY_setEventHandler(&controlHandle);

    fadeOut();

    PAL_setPalette(PAL0, map_test_palette.data, DMA);
    VDP_loadTileSet(&map_test_tileset, ind, DMA);
    level.map_fg = MAP_create(&map_test_map, BG_A, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, ind));

    ind += map_test_tileset.numTile;

    cameraSetPosition(0, level.height - SCREEN_HEIGHT);
    SYS_doVBlankProcess();

    // prepare palettes (BGB image contains the 4 palettes data)
    memcpy(&palette[0], map_test_palette.data, 64 * 2);
    //    memcpy(&palette[16], bga_image.palette->data, 16 * 2);
    //    memcpy(&palette[32], sonic_sprite.palette->data, 16 * 2);
    //    memcpy(&palette[48], enemies_sprite.palette->data, 16 * 2);

    // fade in
    PAL_fadeIn(0, (4 * 16) - 1, palette, 30, TRUE);

    SYS_showFrameLoad(TRUE);

    KLog_S1("start camera.position.x", camera.position.x);
    KLog_S1("start camera.position.y", camera.position.y);

    while (1)
    {
        controlHandle2();
        SPR_update();
        SYS_doVBlankProcess();
    }

    return (0);
}

void fadeOut(void)
{
    PAL_setColors(0, (u16 *)palette_black, 64, CPU);
}

void cameraSetPosition(s16 x, s16 y)
{
    KLog_S1("x", x);
    KLog_S1("y", y);

    if ((x != camera.position.x) || (y != camera.position.y))
    {
        if (x < 0)
        {
            x = 0;
        }

        if (y < 0)
        {
            y = 0;
        }

        if (x > (level.width - SCREEN_WIDTH))
        {
            x = (level.width - SCREEN_WIDTH);
        }

        if (y > (level.height - SCREEN_HEIGHT))
        {
            y = (level.height - SCREEN_HEIGHT);
        }

        camera.position.x = x;
        camera.position.y = y;

        MAP_scrollTo(level.map_fg, x, y);
    }
}

void controlHandle2()
{
    if (debug_mode)
    {
        u16 value = JOY_readJoypad(JOY_1);

        if (value & BUTTON_RIGHT)
        {
            cameraSetPosition(camera.position.x + 2, camera.position.y);
        }

        if (value & BUTTON_LEFT)
        {
            cameraSetPosition(camera.position.x - 2, camera.position.y);
        }

        if (value & BUTTON_DOWN)
        {
            cameraSetPosition(camera.position.x, camera.position.y + 2);
        }

        if (value & BUTTON_UP)
        {
            cameraSetPosition(camera.position.x, camera.position.y - 2);
        }
    }
}

void controlHandle(u16 joy, u16 changed, u16 state)
{
    if (state & BUTTON_MODE)
    {
        debug_mode = !debug_mode;
        KLog_S1("debug_mode", debug_mode);
    }

    if (debug_mode)
    {
        return;
    }

    // Update x velocity
    if (state & BUTTON_RIGHT)
    {
        control.d_pad.x = 1;
    }
    else if (state & BUTTON_LEFT)
    {
        control.d_pad.x = -1;
    }
    else if ((changed & BUTTON_RIGHT) | (changed & BUTTON_LEFT))
    {
        control.d_pad.x = 0;
    }

    // Update x velocity
    if (state & BUTTON_DOWN)
    {
        control.d_pad.y = 1;
    }
    else if (state & BUTTON_UP)
    {
        control.d_pad.y = -1;
    }
    else if ((changed & BUTTON_DOWN) | (changed & BUTTON_UP))
    {
        control.d_pad.y = 0;
    }

    if (state & BUTTON_C)
    {
        if (player.is_on_floor)
        {
            player.velocity.y = -JUMP_FORCE;
        }
    }

    if (state & BUTTON_X)
    {
        if (level.curr_level == 1)
        {
            level.curr_level = 2;
        }

        if (level.curr_level == 2)
        {
            level.curr_level = 1;
        }

        levelChange(level.curr_level);
    }
}

void levelChange(u16 new_level)
{
    if (new_level == 1)
    {
        fadeOut();

        ind = TILE_USER_INDEX;

        PAL_setPalette(PAL0, map_test_palette.data, DMA);
        VDP_loadTileSet(&map_test_tileset, ind, DMA);
        level.map_fg = MAP_create(&map_test_map, BG_A, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, ind));

        ind += map_test_tileset.numTile;
        level.ind_tileset = ind;

        SYS_doVBlankProcess();

        memcpy(&palette[0], map_test_palette.data, 16 * 2);
        //    memcpy(&palette[16], bga_image.palette->data, 16 * 2);
        //    memcpy(&palette[32], sonic_sprite.palette->data, 16 * 2);
        //    memcpy(&palette[48], enemies_sprite.palette->data, 16 * 2);

        PAL_fadeIn(0, (4 * 16) - 1, palette, 30, TRUE);
    }

    if (new_level == 2)
    {
        fadeOut();

        ind = TILE_USER_INDEX;

        PAL_setPalette(PAL0, map_test_palette_2.data, DMA);
        VDP_loadTileSet(&map_test_tileset_2, ind, DMA);
        level.map_fg = MAP_create(&map_test_map_2, BG_A, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, ind));

        ind += map_test_tileset_2.numTile;
        level.ind_tileset = ind;

        SYS_doVBlankProcess();

        memcpy(&palette[0], map_test_palette_2.data, 16 * 2);

        PAL_fadeIn(0, (4 * 16) - 1, palette, 30, TRUE);
    }

    MAP_scrollTo(level.map_fg, -1, -1);
    cameraSetPosition(0, (level.height - SCREEN_HEIGHT));
}
