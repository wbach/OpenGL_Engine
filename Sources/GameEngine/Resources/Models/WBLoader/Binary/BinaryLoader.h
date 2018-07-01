#pragma once
#include "../AbstractLoader.h"
#include <list>

class CTextureLoader;

namespace WBLoader
{
	class BinaryLoader : public AbstractLoader
	{
	public:
		BinaryLoader(CTextureLoader& textureLodaer);
		virtual void ParseFile(const std::string& filename) override;
		virtual bool CheckExtension(const std::string& filename) override;
		
	};
}