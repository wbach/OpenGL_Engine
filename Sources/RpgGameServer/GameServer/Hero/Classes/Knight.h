#pragma once
#include "../BaseHero.h"

namespace GameServer
{
namespace Hero
{
class Knight : public BaseHero
{
   public:
    Knight();
    Knight(const std::string& name);

   protected:
};
}  // namespace Hero
}  // namespace GameServer
