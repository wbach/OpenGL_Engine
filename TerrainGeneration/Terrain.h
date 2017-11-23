#pragma once
#include "noise/module/perlin.h"
#include <string>
#include <vector>
#include <mutex>

class Terrain
{
public:
	Terrain(int width, int height, int depth);
	void Generate(int scale);
	void SaveToFile(const std::string& file_name) const;
	void SaveToRawFile(const std::string& file_name) const;
	void PrintMinMaxValues() const;
	float GetMin() { return minHeight; }
	float GetMax() { return maxHeight; }
private:
	float GetRandomHeight(int x, int y, int scale);
	void SaveMinMaxValues(float value);
	void Normalize();
	void CreateBitmapData();
	void CalcualteFloatData(int scale);
	void CalcualteFloatDataPart(int x1, int y1, int x2, int y2, int scale);
	void MinMaxReset();
	void insertToFloatData(int x, int y, float f);

private:
	noise::module::Perlin perlin;
	std::vector<unsigned char> data;
	std::vector<float> floatData;
	int width;
	int height;
	int depth;

	float minHeight;
	float maxHeight;

	std::mutex insertMutex;
};