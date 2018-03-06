#pragma once
#include "Types.h"
#include "Translate.h"
#include "Rotate.h"
#include "Scale.h"
#include "Matrix.h"
#include "InstanceController.h"
#include "optional.hpp"
#include <unordered_map>
#include <memory>

namespace GameEngine
{
	namespace Collada
	{
		struct Node
		{
			std::string id_;
			std::string sid_;
			std::string name_;
			std::string type_;
			wb::optional<Scale> scale_;
			wb::optional<Matrix> matrix_;
			wb::optional<Rotate> rotate_;
			wb::optional<Translate> translate_;
			wb::optional<InstanceController> instanceController_;
			std::unordered_map<std::string, std::shared_ptr<Node>> children_;
		};
	}
}
