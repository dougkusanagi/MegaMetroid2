#pragma once

#include <genesis.h>

typedef struct
{
    Vect2D_s16 position;
    Vect2D_f16 velocity;

    bool is_on_floor;
    bool is_flipped;

    Sprite *sprite;
    u16 current_animation;
} Player;

extern Player player;
