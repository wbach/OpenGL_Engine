#pragma once
#include "XmlNode.h"

namespace Utils
{
class XmlReader
{
public:
    bool Read(const std::string& filename);
    XmlNode* Get(const std::string& name, XmlNode* node = nullptr);

private:
    std::unique_ptr<XmlNode> root_;
};
}  // namespace Utils
