#include <genesis.h>
#include "../res/resources.h"

#include "../inc/global.h"
#include "../inc/camera.h"
#include "../inc/player.h"
#include "../inc/control.h"

u16 tile_index = TILE_USER_INDEX;
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
void boot(void);
void clearLevel(void);
int count_digits(int number);
void write_int_text(int number, s16 x, s16 y);

void debug_camera(void)
{
    if (debug_mode)
    {
        VDP_drawText("camera.X:", 0, 0);
        write_int_text(camera.position.x, 10, 0);

        VDP_drawText("camera.y:", 0, 1);
        write_int_text(camera.position.y, 10, 1);
    }
    else
    {
        VDP_clearText(0, 0, 20);
        VDP_clearText(0, 1, 20);
    }
}

int main()
{
    boot();

    levelChange(1);

    SYS_showFrameLoad(TRUE);

    KLog_S1("start camera.position.x", camera.position.x);
    KLog_S1("start camera.position.y", camera.position.y);

    PAL_setPalette(PAL2, text_palette.data, DMA);
    VDP_setTextPalette(PAL2);

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

        debug_camera();
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
        KLog("BUTTON_X");
        KLog_S1("changed", changed);
        KLog_S1("state", state);

        if (level.curr_level == 1)
        {
            levelChange(2);
        }
        else if (level.curr_level == 2)
        {
            levelChange(1);
        }
    }
}

void levelChange(u16 new_level)
{
    KLog_S1("new_level", new_level);

    level.curr_level = new_level;
    tile_index = TILE_USER_INDEX;

    fadeOut();
    clearLevel();

    switch (new_level)
    {
    case 1:
        PAL_setPalette(PAL0, map_test_palette.data, DMA);
        VDP_loadTileSet(&map_test_tileset, tile_index, DMA);
        level.map_fg = MAP_create(&map_test_map, BG_B, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, tile_index));

        tile_index += map_test_tileset.numTile;
        level.ind_tileset = tile_index;

        SYS_doVBlankProcess();

        memcpy(&palette[0], map_test_palette.data, 16 * 2);
        //    memcpy(&palette[16], bga_image.palette->data, 16 * 2);
        //    memcpy(&palette[32], sonic_sprite.palette->data, 16 * 2);
        //    memcpy(&palette[48], enemies_sprite.palette->data, 16 * 2);

        PAL_fadeIn(0, (4 * 16) - 1, palette, 30, TRUE);
        break;

    case 2:
        PAL_setPalette(PAL0, map_test_palette_2.data, DMA);
        VDP_loadTileSet(&map_test_tileset_2, tile_index, DMA);
        level.map_fg = MAP_create(&map_test_map_2, BG_B, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, tile_index));

        tile_index += map_test_tileset_2.numTile;
        level.ind_tileset = tile_index;

        SYS_doVBlankProcess();

        memcpy(&palette[0], map_test_palette_2.data, 16 * 2);

        PAL_fadeIn(0, (4 * 16) - 1, palette, 30, TRUE);
        break;

    default:
        break;
    }

    cameraSetPosition(0, 0);
    // cameraSetPosition(0, (level.height - SCREEN_HEIGHT));
    cameraSetPosition(0, (448 - SCREEN_HEIGHT));
}

void boot(void)
{
    SYS_disableInts();
    VDP_setScreenWidth256();
    SPR_init();
    JOY_setEventHandler(&controlHandle);
}

void clearLevel(void)
{
    VDP_clearPlane(BG_B, TRUE);
    VDP_clearSprites();
}

void write_int_text(int number, s16 x, s16 y)
{
    int n = count_digits(number);
    char *str = (char *)malloc((n + 1) * sizeof(char));

    if (str == NULL)
    {
        return;
    }

    sprintf(str, "%d", number);
    VDP_drawText(str, x, y);
    free(str);
}

int count_digits(int number)
{
    if (number == 0)
    {
        return 1;
    }

    int count = 0;
    while (number != 0)
    {
        number /= 10;
        ++count;
    }

    return count;
}
