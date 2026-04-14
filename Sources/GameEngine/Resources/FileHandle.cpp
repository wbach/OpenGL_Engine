#include "FileHandle.h"

#include <Utils/TreeNodeReadFunctions.h>
#include <Utils/TreeNodeWriteFunctions.h>
#include <Variant.h>

namespace GameEngine
{
std::string getPath(const FileHandle &fileHandle)
{
    return std::visit(visitor{[](const File &file) { return file.GetDataRelativePath().generic_string(); },
                              [](const MemoryFile &mem) { return "mem://" + mem.name; }},
                      fileHandle);
}
std::string getBaseName(const FileHandle &fileHandle)
{
    return std::visit(visitor{[](const File &file) { return file.GetBaseName(); },
                              [](const MemoryFile &mem) { return "mem://" + mem.name; }},
                      fileHandle);
}
}  // namespace GameEngine

std::ostream &operator<<(std::ostream &os, const GameEngine::FileHandle &fileHandle)
{
    std::visit([&os](auto &&arg) { os << arg; }, fileHandle);
    return os;
}

void Read(const TreeNode &input, GameEngine::FileHandle &file)
{
    std::string fileStr;
    ::Read(input, fileStr);

    if (fileStr.starts_with("mem://"))
    {
        file = GameEngine::MemoryFile{.name = fileStr.substr(6)};
    }
    else
    {
        file = GameEngine::File(fileStr);
    }
}

void write(TreeNode &node, const GameEngine::FileHandle &file)
{
    write(node, GameEngine::getPath(file));
}