#pragma once
#include <Types.h>
#include <UtilsNetwork/IMessage.h>

namespace common
{
class CharacterInfo
{
   public:
    CharacterInfo()
        : CharacterInfo("")
    {
    }

    CharacterInfo(const std::string& name)
        : CharacterInfo(0, name, 0, 0)
    {
    }

    CharacterInfo(uint8 id, const std::string& name, uint8 lvl, uint8 classId)
        : id_(id)
        , lvl_(lvl)
        , classId_(classId)
    {
        memset(name_, 0, Network::arraySize);
        Network::CopyToArray(name_, name);
    }

    std::string GetName() const
    {
        return name_;
    }

   public:
    uint8 id_;
    uint8 lvl_;
    uint8 classId_;

   private:
    char name_[Network::arraySize];
};
}  // namespace Network
