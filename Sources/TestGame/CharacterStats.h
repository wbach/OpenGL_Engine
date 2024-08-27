#pragma once
#include <Utils/math.hpp>

struct SCharacterStats
{
    int currentHealth = 60;
    int maxHealth     = 100;

    int currentMana = 10;
    int maxMana     = 10;

    int currentStamina = 50;
    int maxStamina     = 50;

    float range            = 1.f;
    float runSpeed         = Utils::KmToMs(45.f);
    float walkSpeed        = Utils::KmToMs(5.f);
    float currentMoveSpeed = 0.f;
    float currentTurnSpeed = 0.f;
    float jumpPower        = 25.f;
    float turnSpeed        = 160.f;

    int strength = 10;
    int agility  = 10;
    int vitality = 10;
    int energy   = 10;
};
