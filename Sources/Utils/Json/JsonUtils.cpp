#include "JsonUtils.h"

#include "JsonReader.h"

namespace Utils
{
bool isJsonFileOfType(const std::string& file, const std::string& type)
{
    JsonReader reader;
    if (not reader.Read(file))
        return false;

    return reader.Get("Material") != nullptr;
}
}  // namespace Utils