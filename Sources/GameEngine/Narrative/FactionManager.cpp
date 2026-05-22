#include "FactionManager.h"

namespace GameEngine
{
FactionManager::FactionManager()
{
    setupRelation("Player", "OldCampMembers", Attitude::Neutral);

    setupRelation("Scierwojad", "Player", Attitude::Hostile);
    setupRelation("Scierwojad", "OldCampMembers", Attitude::Hostile);
}

Attitude FactionManager::getAttitude(uint32_t factionA, uint32_t factionB) const
{
    if (factionA == factionB)
    {
        return Attitude::Friendly;
    }

    auto itA = matrix_.find(factionA);
    if (itA != matrix_.end())
    {
        auto itB = itA->second.find(factionB);
        if (itB != itA->second.end())
        {
            return itB->second;
        }
    }
    return Attitude::Neutral;
}
void FactionManager::setupRelation(std::string_view factionA, std::string_view factionB, Attitude attitude)
{
    // Relacja dwustronna
    uint32_t idA = Utils::HashString(factionA);
    uint32_t idB = Utils::HashString(factionB);

    matrix_[idA][idB] = attitude;
    matrix_[idB][idA] = attitude;
}
}  // namespace GameEngine
