#include "XmlMessageConverter.h"
#include <UtilsNetwork/MessageFormat.h>
#include "MessageTypes.h"

namespace common
{

XmlMessageConverter::XmlMessageConverter()
{

}

bool XmlMessageConverter::IsValid(uint8 format, uint8 type) const
{
    return format == Network::ConvertFormat(Network::MessageFormat::Xml) and type >= MESSAGE_TYPE_RANGE.first and type <= MESSAGE_TYPE_RANGE::second;
}

std::unique_ptr<Network::IMessage> XmlMessageConverter::Convert(uint8 type, const std::vector<int8> &message)
{

}

std::vector<int8> XmlMessageConverter::Convert(const IMessage &message)
{

}

}  // namespace common
