#include "XMLMessageConverter.h"
#include <Utils/XML/XmlReader.h>
#include <Utils/XML/XmlWriter.h>
#include <UtilsNetwork/MessageFormat.h>
//#include "MessageTypes.h"

namespace GameEngine
{
XmlMessageConverter::XmlMessageConverter()
{
}

bool XmlMessageConverter::IsValid(Network::IMessageFormat format, Network::IMessageType type) const
{
    return format == Network::ConvertFormat(Network::MessageFormat::Xml) ;//and type >= COMMON_MESSAGE_TYPE_RANGE_LOW and type <= COMMON_MESSAGE_TYPE_RANGE_HIGH;
}
}
