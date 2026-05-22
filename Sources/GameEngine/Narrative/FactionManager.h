#pragma once
#include <Types.h>
#include <Utils.h>

#include <unordered_map>

namespace GameEngine
{
enum class Attitude
{
    Friendly,
    Neutral,
    Hostile
};

class FactionManager
{
public:
    FactionManager();

    Attitude getAttitude(uint32_t, uint32_t) const;

private:
    void setupRelation(std::string_view, std::string_view, Attitude);

private:
    std::unordered_map<uint32_t, std::unordered_map<uint32_t, Attitude>> matrix_;
};
}  // namespace GameEngine
