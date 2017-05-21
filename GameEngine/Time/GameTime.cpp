#include "GameTime.h"

void CGameTime::Update(const float & delta_time)
{
    deltaGameTime = delta_time / 1000;
    currentTime += deltaGameTime;

    if (currentTime > 1)
        currentTime -= 1;
    if (currentTime < 0)
        currentTime += 1;

    deltaTime = delta_time;
}
