#pragma once
#include "Database/IDatabaseWrapper.h"
#include "Map.h"

namespace GameServer
{
typedef std::vector<Map> Maps;

class Manager
{
   public:
    Manager(Database::IDatabaseWrapperPtr databaseWrapper)
        : databaseWrapper_(databaseWrapper)
    {
        // tmp adding 10 maps
        maps_.resize(10);
    }
    Hero::BaseHeroPtr GetHero(uint32 id);
    void AddHero(uint32 id);
    void RemoveHero(uint32 id);
    void UpdateAllControllers(float dt);
    const CharactersMap& GetAllCharactersInMap(uint32 mapId) const;

   private:
    Maps maps_;
    Database::IDatabaseWrapperPtr databaseWrapper_;
    CharactersMap emptyCharactersMap_;
};
}  // namespace GameServer
