#pragma once
#include "File.h"
#include "MemoryFile.h"

namespace GameEngine
{
class FileHandle : public std::variant<MemoryFile, File>
{
public:
    using variant::variant;

    FileHandle() = default;
    FileHandle(const std::string&);
    FileHandle(const File&);
    FileHandle(const MemoryFile&);

    FileHandle& operator=(const std::string&);
    FileHandle& operator=(const std::filesystem::path&);
    FileHandle& operator=(const File&);
    FileHandle& operator=(const MemoryFile&);

    bool operator==(const File&) const;
    bool operator==(const MemoryFile&) const;
    bool operator==(const FileHandle&) const;

    std::string getPath() const;
    std::string getBaseName() const;

    friend std::ostream& operator<<(std::ostream&, const GameEngine::FileHandle&);
};
}  // namespace GameEngine

class TreeNode;
ENGINE_API void Read(const TreeNode&, GameEngine::FileHandle&);
ENGINE_API void write(TreeNode&, const GameEngine::FileHandle&);
