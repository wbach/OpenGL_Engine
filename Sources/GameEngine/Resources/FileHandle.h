#pragma once
#include "File.h"
#include "MemoryFile.h"

namespace GameEngine
{
using FileHandle = std::variant<MemoryFile, File>;
std::string getPath(const FileHandle&);
std::string getBaseName(const FileHandle&);

inline bool operator==(const File& lhs, const FileHandle& rhs)
{
    if (const auto* rhsFile = std::get_if<File>(&rhs))
    {
        return lhs == *rhsFile;
    }
    return false;
}

inline bool operator==(const MemoryFile& lhs, const FileHandle& rhs)
{
    if (const auto* rhsMem = std::get_if<MemoryFile>(&rhs))
    {
        return lhs == *rhsMem;
    }
    return false;
}

inline bool operator==(const FileHandle& lhs, const File& rhs)
{
    return rhs == lhs;
}
inline bool operator==(const FileHandle& lhs, const MemoryFile& rhs)
{
    return rhs == lhs;
}

}  // namespace GameEngine

class TreeNode;
ENGINE_API void Read(const TreeNode&, GameEngine::FileHandle&);
ENGINE_API void write(TreeNode&, const GameEngine::FileHandle&);

std::ostream& operator<<(std::ostream&, const GameEngine::FileHandle&);