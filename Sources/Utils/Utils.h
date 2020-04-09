#pragma once
#include "Types.h"
#include "math.hpp"
#include "Glm.h"
#include <vector>
#include <list>
#include <sstream>
#include <string>
#include <unordered_map>

namespace Utils
{
template <typename T>
std::string ToStr(const T& t)
{
    std::ostringstream os;
    os << t;
    return os.str();
}

template<class T>
class ObjectGrid
{
public:
    enum ObjectType
    {
        STATIC,
        DYNAMIC
    };
    ObjectGrid(vec2i size)
        : size(size)
    {
        staticObjects.resize(size.x * size.y);
    }
    void AddObject(T*, ObjectType type)
    {
        //if(type == ObjectType::STATIC)
            //staticObjects.
    }

private:
    std::list<T*> dynamicObjects;
    std::vector<T*> staticObjects;

    vec2i size;
    vec2i offset;
};

struct FileInfo
{
    std::string filename;
    std::string path;
    std::string extension;
};

std::vector<std::string> SplitString(char* s, int size, char split_char);
std::vector<std::string> SplitString(const std::string& s, char split_char);
std::unordered_map<std::string, uint32> SplitStringWithId(const std::string& s, char split_char);
std::string ReadFileBinary(const std::string& file_name);
std::string ReadFile(const std::string& file_name);
std::list<std::string> ReadFileLines(const std::string& file_name);
void WrtieToFile(const std::string& filename, const std::string& content);
std::string ReplaceSlash(const std::string& str);
FileInfo GetFileInfo(const std::string& fullpath);
std::string GetFilename(const std::string& fullpath);
std::string GetFilePath(const std::string& fullpath);
std::string GetFileExtension(const std::string& file_name);
std::string GetFilenameWithExtension(const std::string& fullpath);
std::string GetPathAndFilenameWithoutExtension(const std::string& path);
int NextP2(int a);
bool CheckFileExist(const std::string& file);
bool CheckExtension(const std::string& file, const std::string& ext);
// catch exception and log
float StringToFloat(const std::string& str);
int StringToInt(const std::string& str);
bool StringToBool(const std::string& str);
std::string BoolToString(bool);

template<class T>
bool ValidateRange(const T& c, uint32 id)
{
    return (id >= 0 and id < c.size());
}
} // namespace Utils
