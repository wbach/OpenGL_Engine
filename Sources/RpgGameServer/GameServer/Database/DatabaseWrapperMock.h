#pragma once
#include "IDatabaseWrapper.h"

namespace GameServer
{
namespace Database
{
class DatabaseWrapperMock : public IDatabaseWrapper
{
public:
    DatabaseWrapperMock();
    virtual UserCharacterVec GetCharacterByUser(uint32 id) override;
    virtual wb::optional<common::Hero::CharacterData> GetCharacterData(uint32 id) override;
    virtual wb::optional<common::Hero::Stats> GetCharacterStats(uint32 stats) override;
    virtual wb::optional<DatabaseTransformInfo> GetTransformInfo(uint32 characterId) override;
};
}  // namespace Database
}  // namespace GameServer
