#pragma once
#include <ostream>
#include <string>

namespace GameEngine
{
struct MemoryFile
{
    MemoryFile() = default;
    MemoryFile(const std::string& name)
        : name{name}
    {
    }
    std::string name;

    auto operator<=>(const MemoryFile&) const = default;
};

inline std::ostream& operator<<(std::ostream& os, const MemoryFile& f)
{
    return os << "MemoryFile{" << f.name << "}";
}
}  // namespace GameEngine
