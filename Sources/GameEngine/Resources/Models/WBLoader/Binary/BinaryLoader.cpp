#include "BinaryLoader.h"
#include "GameEngine/Resources/TextureLoader.h"
#include "BinaryReader.h"

namespace WBLoader
{
	BinaryLoader::BinaryLoader(CTextureLoader& textureLodaer)
		: AbstractLoader(textureLodaer.GetGraphicsApi(), textureLodaer)
	{
	}

	void BinaryLoader::ParseFile(const std::string & filename)
	{
	}

	bool BinaryLoader::CheckExtension(const std::string & filename)
	{
		auto ext = Utils::GetFileExtension(filename);
		return ext == "bin" || ext == "BIN" || ext == "Bin";
	}

} // WBLoader 