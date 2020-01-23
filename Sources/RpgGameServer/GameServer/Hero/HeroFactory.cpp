#include "HeroFactory.hpp"
#include "Classes/Knight.h"

namespace GameServer
{
namespace HeroFactory
{
Hero::BaseHeroPtr CreateKnight()
{
    return std::make_shared<Hero::Knight>();
}
}  // namespace HeroFactory
}  // namespace GameServer
