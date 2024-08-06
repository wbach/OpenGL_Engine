#pragma once
#include "Types.h"
#include "math.hpp"
#include "Glm.h"
#include <vector>
#include <optional>
#include <list>
#include <sstream>
#include <string>
#include <unordered_map>
#include <filesystem>

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
std::string MergeString(const std::vector<std::string>&, const std::string& = "");
std::vector<std::string> SplitString(char* s, int size, char split_char);
std::vector<std::string> SplitString(const std::string& s, char split_char);
std::unordered_map<std::string, uint32> SplitStringWithId(const std::string& s, char split_char);
std::string ReplaceSlash(const std::string& str);
int NextP2(int a);
bool CheckFileExist(const std::string& file);
bool CheckExtension(const std::string& file, const std::string& ext);
// catch exception and log
float StringToFloat(const std::string& str);
int StringToInt(const std::string& str);
bool StringToBool(const std::string& str);
std::string BoolToString(bool);
std::string toLower(const std::string&);
template<class T>
bool ValidateRange(const T& c, uint32 id)
{
    return (id >= 0 and id < c.size());
}
} // namespace Utils
