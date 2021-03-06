#pragma once
#include <unordered_map>
#include "Hero/BaseHero.h"

namespace GameServer
{
typedef std::unordered_map<uint32, Hero::BaseHeroPtr> CharactersMap;

class Map
{
   public:
    Hero::BaseHeroPtr GetCharacter(uint32 id);
    void AddCharacter(uint32 characterId, Hero::BaseHeroPtr);
    void RemoveCharacter(uint32 id);
    void UpdateAllControllers(float dt);
    const CharactersMap& GetAllCharacters() const;

   private:
    CharactersMap characters_;
};
}  // namespace GameServer
