#include "SingleTon.h"
#include "Configuration.h"
#include "Terrain.h"
#include <iostream>
#include <string>

void argumentToValue(int& value, int argcv, int argc, char** argv)
{
	if (argc > argcv)
	{
		std::string value1 = argv[argcv];
		value = stoi(value1);
	}
}

int main(int argc, char** argv)
{

	auto confInstance = SingleTon<Config>::Get();

	argumentToValue(confInstance.threadsCount, 1, argc, argv);
	argumentToValue(confInstance.width, 2, argc, argv);
	argumentToValue(confInstance.height, 3, argc, argv);
	argumentToValue(confInstance.depth, 4, argc, argv);
	argumentToValue(confInstance.scale, 5, argc, argv);
	argumentToValue(confInstance.offsetX, 6, argc, argv);
	argumentToValue(confInstance.offsetY, 7, argc, argv);


	std::cout << "***********************" << std::endl;
	std::cout << "Noise lib test example." << std::endl;
	std::cout << "***********************" << std::endl;

	//NormalizationTest(); return 0;g

	Terrain terrain(confInstance.width, confInstance.height, confInstance.depth);
	terrain.Generate(confInstance.scale);
	terrain.PrintMinMaxValues();
	terrain.SaveToFile("heightmap.bmp");
	terrain.SaveToRawFile("heightmap.raw");
	return 0;
}
