#pragma once
#include <string>
#include <memory>

class CModel;
class CTextureLoader;

namespace WBLoader
{
	std::unique_ptr<CModel> ReadBinFile(const std::string& filename, CTextureLoader& textureLoader);
} // WBLoader
