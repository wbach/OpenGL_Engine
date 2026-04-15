#pragma once
#include <ostream>
#include <string>

namespace GameEngine
{
struct MemoryFile
{
    MemoryFile() = default;
    MemoryFile(const std::string&);

    std::string name;

    bool empty() const;
    auto operator<=>(const MemoryFile&) const = default;

    friend std::ostream& operator<<(std::ostream&, const MemoryFile&);
};
}  // namespace GameEngine
