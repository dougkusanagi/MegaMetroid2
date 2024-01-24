#pragma once

#include <genesis.h>

typedef struct
{
    Vect2D_s16 d_pad;
    bool a;
    bool b;
    bool c;
    bool x;
    bool y;
    bool z;
} Control;

extern Control control;

void controlHandle(u16 joy, u16 changed, u16 state);
