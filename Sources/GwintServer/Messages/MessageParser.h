#pragma once
#include "Message.h"
#include <rapidxml-1.13/rapidxml.hpp>

namespace 
{
	static std::string& ParseString(rapidxml::xml_node<>* node)
	{
		return std::string(node->value());
	}

	static Body ParseBody(rapidxml::xml_node<>* node)
	{
		Body body;

		return body;
	}

	static Header ParseHeader(rapidxml::xml_node<>* node)
	{
		Header header;
		for (auto snode = node->first_node(); snode; snode = snode->next_sibling())
		{
			std::string name = snode->name();
			if (name == "from")
				header.from = ParseString(snode);
		}
		return header;
	}
	static Message ParseMessage(rapidxml::xml_node<>* node)
	{
		Message message;
		for (auto snode = node->first_node(); snode; snode = snode->next_sibling())
		{
			std::string name = snode->name();
			if (name == "header")
				message.m_Header = ParseHeader(snode);
			else if (name == "Body")
				message.m_Body = ParseBody(snode);
		}

		return message;
	}
	static Message Parse(const std::string& str)
	{
		rapidxml::xml_document<> document;
		try
		{
			document.parse<0>(const_cast<char*>(str.c_str()));
		}
		catch (rapidxml::parse_error p)
		{
			p.what();
			return;
		}

		auto root = document.first_node();
		if (root == nullptr)
			return;
		

		return ParseMessage(root);
	}
}