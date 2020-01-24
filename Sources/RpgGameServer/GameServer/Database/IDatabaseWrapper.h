#pragma once
#include <memory>
#include <unordered_map>
#include <vector>
#include "../../../Common/Hero/Stats.h"
#include "../Hero/BaseHero.h"
#include "DatabaseTransformInfo.h"
#include "Types.h"
#include "optional.hpp"

namespace GameServer
{
namespace Database
{
typedef std::vector<uint32> UserCharacterVec;
typedef std::unordered_map<uint32, Hero::BaseHeroPtr> CharactersMap;

class IDatabaseWrapper
{
public:
    virtual ~IDatabaseWrapper() = default;

    virtual UserCharacterVec GetCharacterByUser(uint32 id)                           = 0;
    virtual wb::optional<common::Hero::CharacterData> GetCharacterData(uint32 id)    = 0;
    virtual wb::optional<common::Hero::Stats> GetCharacterStats(uint32 stats)        = 0;
    virtual wb::optional<DatabaseTransformInfo> GetTransformInfo(uint32 characterId) = 0;

protected:
    CharactersMap allCharacters_;
};

typedef std::shared_ptr<IDatabaseWrapper> IDatabaseWrapperPtr;
}  // namespace Database
}  // namespace GameServer
