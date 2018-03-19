#pragma once

namespace GameEngine
{
	enum class TextureType
	{
		FLOAT_BUFFER_2D,
		DEPTH_BUFFER_2D,
		FLOAT_TEXTURE_2D,
		FLOAT_TEXTURE_1C,
		SHADOW_MAP,
		U8_RGBA
	};

	enum class TextureFilter
	{
		LINEAR,
		NEAREST
	};

	enum class TextureMipmap
	{
		NONE,
		LINEAR
	};
}
