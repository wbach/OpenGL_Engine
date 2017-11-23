#include "Terrain.h"
#include <iostream>
#include "Image.h"
#include "Configuration.h"
#include "SingleTon.h"

#define CONF SingleTon<Config>::Get()

Terrain::Terrain(int width, int height, int depth)
	: width(width)
	, height(height)
	, depth(depth)
{
	MinMaxReset();
}

void Terrain::Generate(int scale)
{
	floatData.resize(width*height);
	std::cout << "CalcualteFloatData..." << std::endl;
	CalcualteFloatData(scale);
	std::cout << "Normalize..." << std::endl;
	Normalize();
	std::cout << "CreateBitmapData..." << std::endl;
	CreateBitmapData();
}

void Terrain::SaveToFile(const std::string &file_name) const
{
	std::cout << "SaveToFile: " << file_name << std::endl;
	if (data.empty())
	{
		std::cout << "Bmp data is empty." << std::endl;
		return;
	}
	bmp_generator(file_name, width, height, &data[0]);
}

void Terrain::SaveToRawFile(const std::string & file_name) const
{
	writeRawData(file_name, data);
}

void Terrain::PrintMinMaxValues() const
{
	std::cout << "Min: " << minHeight << " Max: " << maxHeight << std::endl;
}

float Terrain::GetRandomHeight(int x, int y, int scale)
{
	float xx = static_cast<float>(x) / width * scale;
	float yy = static_cast<float>(y) / height * scale;

	auto value = perlin.GetValue(xx + CONF.offsetX, 50, yy + CONF.offsetY);
	SaveMinMaxValues(value);
	return value;
}

void Terrain::SaveMinMaxValues(float value)
{
	if (value > maxHeight)
		maxHeight = value;

	if (value < minHeight)
		minHeight = value;
}

void Terrain::Normalize()
{
	for (auto& v : floatData)
		v -= minHeight;

	MinMaxReset();

	for (auto& v : floatData)
		SaveMinMaxValues(v);

	for (auto& v : floatData)
		v /= maxHeight;

	MinMaxReset();

	for (auto& v : floatData)
		SaveMinMaxValues(v);
}

void Terrain::CreateBitmapData()
{
	for (const auto& v : floatData)
	{
		auto value = static_cast<unsigned char>(v * 255.f);
		data.push_back(value);
		data.push_back(value);
		data.push_back(value);
	}
}

void Terrain::CalcualteFloatData(int scale)
{
	std::vector<std::thread> threads;
	threads.resize(CONF.threadsCount);
	int part = height / CONF.threadsCount;

	int x = 0;
	for (auto& thread : threads)
	{
		thread = std::thread(&Terrain::CalcualteFloatDataPart, this, 0, x*part, width, (x + 1)*part, scale);
		x++;
	}

	for (auto& thread : threads)
	{
		thread.join();
	}
}

void Terrain::CalcualteFloatDataPart(int x1, int y1, int x2, int y2, int scale)
{
	if (x1 < 0)
		x1 = 0;
	if (y1 < 0)
		y1 = 0;
	if (x2 > width)
		x2 = width;
	if (y2 > height)
		y2 = height;

	for (int y = y1; y < y2; y++)
		for (int x = x1; x < x2; x++)
			insertToFloatData(x, y, GetRandomHeight(x, y, scale));
}

void Terrain::MinMaxReset()
{
	minHeight = std::numeric_limits<float>::max();
	maxHeight = std::numeric_limits<float>::min();
}

void Terrain::insertToFloatData(int x, int y, float f)
{
	std::lock_guard<std::mutex> guard(insertMutex);
	floatData[x + y*width] = f;
}
