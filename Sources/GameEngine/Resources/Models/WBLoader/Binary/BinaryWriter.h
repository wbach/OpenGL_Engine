#pragma once
#include <string>
#include <memory>

class CModel;

namespace WBLoader
{
	void CreateBinFile(const std::unique_ptr<CModel>& model, const std::string& filename);
	std::string CreateBinPath(const std::string& filename);
}
