#pragma once
#include "XmlNode.h"

namespace Utils
{
	class XmlReader
	{
	public:
		XmlNode Read(const std::string& filename);
		XmlNode* Get(const std::string& name, XmlNode* node = nullptr);

	private:
		XmlNode root_;
	};
} // Utils
