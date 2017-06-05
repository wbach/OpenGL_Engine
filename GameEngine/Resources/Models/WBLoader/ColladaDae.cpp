#include "ColladaDae.h"
#include <rapidxml.hpp>

namespace WBLoader
{
	ColladaDae::ColladaDae(CTextureLoader & textureLodaer)
		: textureLodaer(textureLodaer)
	{
	}
	void ColladaDae::ParseFile(const std::string & filename)
	{
	}
	std::list<CMesh> ColladaDae::CreateFinalMesh()
	{
		return std::list<CMesh>();
	}
	bool ColladaDae::CheckExtension(const std::string & filename)
	{
		auto ext = Utils::GetFileExtension(filename);
		return ext == "dae" || ext == "DAE" || ext == "Dae";
	}
}