#include "EngineUitls.h"
#include "../Resources/Textures/Texture.h"

void Utils::BindTexure(std::weak_ptr<CTexture> texture, uint id)
{
	if (auto t = texture.lock())
	{
		glActiveTexture(GL_TEXTURE0 + id);
		glBindTexture(GL_TEXTURE_CUBE_MAP, t->GetId());
	}
}
