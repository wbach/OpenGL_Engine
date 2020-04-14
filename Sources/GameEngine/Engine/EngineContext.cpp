#include "EngineContext.h"

namespace GameEngine
{
EngineContextData::~EngineContextData()
{
    threadSync_.Stop();
}
}  // namespace GameEngine
