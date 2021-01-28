#include "XmlConverterUtils.h"

namespace Network
{
IMessageData CreatePayload(TreeNode& root)
{
    auto s = Utils::Xml::Write(root);
    std::vector<int8> v;
    s = s.substr(0, s.find_last_of('>') + 1);
    std::copy(s.begin(), s.end(), std::back_inserter(v));
    return v;
}
}