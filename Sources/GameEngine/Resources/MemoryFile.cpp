#include "MemoryFile.h"

namespace GameEngine
{
MemoryFile::MemoryFile(const std::string& name)
    : name{name}
{
}
bool MemoryFile::empty() const
{
    return name.empty();
}

std::ostream& operator<<(std::ostream& os, const MemoryFile& f)
{
    return os << "MemoryFile{" << f.name << "}";
}
}  // namespace GameEngine
