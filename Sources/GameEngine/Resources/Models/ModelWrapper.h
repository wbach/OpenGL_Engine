#pragma once
#include  "Model.h"
#include <unordered_map>

namespace GameEngine
{	
	enum LevelOfDetail
	{
		L1 = 0,
		L2,
		L3
	};

	class ModelWrapper
	{
	public:
		ModelWrapper() {}
		void Add(ModelRawPtr model, LevelOfDetail lvl);
		ModelRawPtr Get(LevelOfDetail lvl);
		vec3 GetScale();

	private:
		vec3 normalizedScale_;
		std::unordered_map<LevelOfDetail, ModelRawPtr> models_;
	};
} // GameEngine