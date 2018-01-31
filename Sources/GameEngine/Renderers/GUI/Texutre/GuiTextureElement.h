#pragma once
#include "Types.h"
#include <unordered_map>
#include "GLM/GLMUtils.h"

class CTexture;

namespace GameEngine
{
	namespace Renderer
	{
		namespace Gui
		{
			class GuiTextureElement 
			{
			public:
				CTexture* texture = nullptr;
				void SetPosition(const vec2& pos)
				{
					position_ = pos;
					UpdateTransformMatrix();
				}
				void SetRotation(float r)
				{
					rotation_ = r;
					UpdateTransformMatrix();
				}
				void SetScale(const vec2& scale)
				{
					scale_ = scale;
					UpdateTransformMatrix();
				}
				const mat4& GetMatrix() const
				{
					return transformMatrix_;
				}
				const vec2 GetPosition() const
				{
					return position_;
				}
				const vec2 GetScale() const
				{
					return scale_;
				}
				float GetRotation()
				{
					return rotation_;
				}
			private:
				void UpdateTransformMatrix() 
				{
					transformMatrix_ = Utils::CreateTransformationMatrix(position_, scale_, rotation_);
				}

			private:
				mat4 transformMatrix_ = mat4(1.f);
				vec2 position_;
				vec2 scale_;
				float rotation_;
			};
		} // Gui
	} // Renderers
} // GameEngine