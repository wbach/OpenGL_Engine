#pragma once

namespace Input
{
enum class GameAction
{
    MOVE_FORWARD = 0,
    MOVE_BACKWARD,
    MOVE_LEFT,
    MOVE_RIGHT,
    TURN_LEFT,
    TURN_RIGHT,
    JUMP,
    ATTACK_1,
    ATTACK_2,
    ATTACK_3,
    SPELL_1,
    SPELL_2,
    SPELL_3,
    SPELL_4,
    GUI_STATS_WINDOW,
    GUI_INVENTORY_WINDOW,
    GUI_PAUSE_MENU_WINDOW,
    ITEM_1,
    ITEM_2,
    ITEM_3,
    ITEM_4,
    ITEM_5,
    ITEM_6,
    WORLD_MAP,
    INCREASE_PITCH_UP,
    INCREASE_PITCH_DOWN
};
}
