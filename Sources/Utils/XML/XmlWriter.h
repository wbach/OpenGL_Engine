#pragma once
#include "XmlNode.h"

namespace Utils
{
namespace Xml
{
void Write(const std::string& filename, XmlNode& root);
std::string Write(XmlNode& root);
}
}  // namespace Utils
