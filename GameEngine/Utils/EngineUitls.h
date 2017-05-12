#pragma once
#include <memory>
#include "Types.h"

class CTexture;

namespace Utils
{
	void BindTexure(std::weak_ptr<CTexture> texture, uint id);
}
