#include "MainScene.h"
#include "../GameEngine/Engine/Engine.h"
#include "Logger/Log.h"
#include <fstream>
#include "FreeImage.h"

void SaveTextureToFile(const std::string& file_name, const std::vector<int>& data, int width, int height)
{
	//FIBITMAP* Image = FreeImage_ConvertFromRawBits(data, width, height, 4, 32, FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK, TRUE);
	
	FIBITMAP* bitmap = FreeImage_Allocate(width, height, 32, 8, 8, 8);

	width = FreeImage_GetWidth(bitmap);

	height = FreeImage_GetHeight(bitmap);

	int index = 0;
	for (int y = height - 1; y >= 0; y--)
	{
		BYTE *bits = FreeImage_GetScanLine(bitmap, height - 1 - y);

		for (int x = 0; x < width; x++)
		{
			/*bits[0] = data[(y*width + x) * 4 + 2];
			bits[1] = data[(y*width + x) * 4 + 1];
			bits[2] = data[(y*width + x) * 4 + 0];
			bits[3] = data[(y*width + x) * 4 + 3];*/
			auto value = static_cast<char>(data[index++]) * 10;
			bits[0] = value;
			bits[1] = value;
			bits[2] = value;
			bits[3] = 255;
			bits += 4;
		}
	}

	FreeImage_FlipVertical(bitmap);
	FreeImage_Save(FIF_PNG, bitmap, file_name.c_str(), PNG_DEFAULT);
	FreeImage_Unload(bitmap);
}

std::vector<int> ReadFile(const std::string& file)
{
	std::vector<int> out;

	auto data = Utils::ReadFile(file);

	int max_h = -10;

	for (const auto& c : data)
	{
		auto value = static_cast<int>(c);
		out.push_back(value);

		if (max_h < value)
			max_h = value;

		//std::cout << " " <<  value << " ";
		
		//<< std::endl;
	}
	std::cout << "Max value: " << max_h << "\nSize: " << out.size();
	return out;
}

void TestGameStart()
{
    CEngine engine;
	engine.Init();
    engine.scene = std::make_unique<MainScene>(engine);
	engine.PreperaScene();
	engine.GameLoop();
}

int main(int argc, char* argv[])
{	
	//MemoryTest();
//	auto data = ReadFile("D:\\Projects\\OpenGL_Engine\\TestGame\\terrain8bit.raw");
//	SaveTextureToFile("D:\\Projects\\OpenGL_Engine\\TestGame\\heightmap.png", data, 513, 513);
	CLogger::Instance().EnableLogs();
	TestGameStart();
	return 0;
}
