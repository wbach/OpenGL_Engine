#pragma once
#include "GameEngine/Resources/File.h"

namespace GameEngine
{
class Model;

std::vector<std::string> GetAvaiableFormats();
bool ExportModel(const Model&, const File&);
}  // namespace GameEngine
