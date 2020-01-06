#pragma once
#include <Types.h>

namespace GameEngine
{
class IEditorInterface
{
public:
    virtual ~IEditorInterface() = default;
    virtual void AddObject(const std::string&) = 0;
};
}  // namespace GameEngine
