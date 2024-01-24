#include "../inc/player.h"

Player player = {
    .position = {0, 0},
    .velocity = {0, 0},
    .is_on_floor = FALSE,
    .is_flipped = FALSE,
    .sprite = NULL,
    .current_animation = 0,
};
