#pragma once
#include "GameEngine/Renderers/Postproccesing/PostprocessingRenderer.h"

namespace GameEngine
{
class DefferedLighting : public PostprocessingRenderer
{
public:
	virtual void Init() override;
	virtual void Prepare() override;
	virtual void Render() override;
	virtual void ReloadShaders() override;
};
} // GameEngine
