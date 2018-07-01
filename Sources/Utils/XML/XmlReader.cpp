#include "XmlReader.h"
#include "rapidxml.hpp"
#include "rapidxml_print.hpp"
#include "Logger/Log.h"
#include "Utils/Utils.h"

namespace Utils
{
	XmlNode empty;

	void ParseNode(rapidxml::xml_node<>* node, XmlNode& n)
	{
		if (node == nullptr)
			return;

		n.name_ = node->name();
		n.value_ = node->value();

		for (auto att_node = node->first_attribute(); att_node; att_node = att_node->next_attribute())
		{
			std::string att_name = att_node->name();
			std::string att_value = att_node->value();

			n.attributes_[att_name] = att_value;
		}

		for (auto snode = node->first_node(); snode; snode = snode->next_sibling())
		{
			if (snode == nullptr)
				return;

			std::string name = snode->name();
			if (name.empty())
				return;
			n.children_[name] = std::make_shared<XmlNode>();
			ParseNode(snode, *n.children_[name].get());
			n.children_[name]->parent = &n;
		}
	}

	bool XmlReader::Read(const std::string & filename)
	{
		rapidxml::xml_document<> document;

		auto str = Utils::ReadFile(filename);
		try
		{
			document.parse<0>(const_cast<char*>(str.c_str()));
		}
		catch (rapidxml::parse_error p)
		{
			std::string out = p.what();
			Error(out);
			return false;
		}
		ParseNode(document.first_node(), root_);
		return true;
	}

	XmlNode* XmlReader::Get(const std::string& name, XmlNode* node)
	{
		if (node == nullptr)
			node = &root_;

		if (node->name_ == name)
			return node;

		if (node->children_.count(name) != 0)
		{
			return node->children_.at(name).get();
		}

		for (auto& ch : node->children_)
		{
			auto n = Get(name, ch.second.get());
			if (n != nullptr)
				return n;
		}

		return &empty;
	}
} // Utils
