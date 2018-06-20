#pragma once
#include "XmlNode.h"

namespace Utils
{
	class XmlWriter
	{
	public:
		void Write(const std::string& filename, XmlNode& root);
	};
} // Utils
