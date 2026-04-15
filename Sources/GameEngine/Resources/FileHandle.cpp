#include "FileHandle.h"

#include <Logger/Log.h>
#include <Utils/TreeNodeReadFunctions.h>
#include <Utils/TreeNodeWriteFunctions.h>
#include <Variant.h>

namespace GameEngine
{
FileHandle::FileHandle(const std::string &path)
{
    *this = path;
}
FileHandle::FileHandle(const File &file)
{
    *this = file;
}
FileHandle::FileHandle(const MemoryFile &memoryFile)
{
    *this = memoryFile;
}
std::string FileHandle::getPath() const
{
    return std::visit(visitor{[](const File &file) { return file.GetDataRelativePath().generic_string(); },
                              [](const MemoryFile &mem) { return "mem://" + mem.name; }},
                      *this);
}
std::string FileHandle::getBaseName() const
{
    return std::visit(
        visitor{[](const File &file) { return file.GetBaseName(); }, [](const MemoryFile &mem) { return "mem://" + mem.name; }},
        *this);
}
std::ostream &operator<<(std::ostream &os, const GameEngine::FileHandle &fileHandle)
{
    std::visit([&os](auto &&arg) { os << arg; }, fileHandle);
    return os;
}
FileHandle &FileHandle::operator=(const std::string &path)
{
    if (path.starts_with("mem://"))
    {
        this->emplace<MemoryFile>(path.substr(6));
    }
    else
    {
        this->emplace<File>(path);
    }
    return *this;
}
FileHandle &FileHandle::operator=(const File &file)
{
    variant::operator=(file);
    return *this;
}
FileHandle &FileHandle::operator=(const MemoryFile &memFile)
{
    variant::operator=(memFile);
    return *this;
}
FileHandle &FileHandle::operator=(const std::filesystem::path &path)
{
    variant::operator=(File(path.generic_string()));
    return *this;
}
bool FileHandle::operator==(const File &rhs) const
{
    if (const auto *lhsFile = std::get_if<File>(this))
    {
        return *lhsFile == rhs;
    }
    return false;
}
bool FileHandle::operator==(const MemoryFile &rhs) const
{
    if (const auto *lhsMem = std::get_if<MemoryFile>(this))
    {
        return *lhsMem == rhs;
    }
    return false;
}
bool FileHandle::operator==(const FileHandle &rhs) const
{
    return static_cast<const variant &>(*this) == static_cast<const variant &>(rhs);
}
}  // namespace GameEngine

void Read(const TreeNode &input, GameEngine::FileHandle &file)
{
    std::string fileStr;
    ::Read(input, fileStr);
    LOG_DEBUG << fileStr;
    if (fileStr.starts_with("mem://"))
    {
        file = fileStr;
    }
    else
    {
        file = GameEngine::File(fileStr);
    }
}
void write(TreeNode &node, const GameEngine::FileHandle &file)
{
    write(node, file.getPath());
}
