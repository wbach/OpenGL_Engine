#pragma once
#include <string>
#include <unordered_map>

namespace Utils
{
	typedef std::unordered_map<std::string, std::string> Attributes;

	struct XmlNode
	{
		std::string name_;
		std::string value_;
		Attributes attributes_;
		std::unordered_map<std::string, XmlNode> children_;
		XmlNode* parent = nullptr;
	};
} // Utils
