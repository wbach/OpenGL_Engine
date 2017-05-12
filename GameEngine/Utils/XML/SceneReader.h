#pragma once
#include <string>

class CScene;

namespace SceneReader
{
	void ReadScene(CScene* scene, const std::string& filename);
}