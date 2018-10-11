#include "EngineMeasurement.h"
#include <fstream>

void GameEngine::EngineMeasurement::Add(const std::string & name, uint64 value)
{
	measurements_[name].Add(value);
}

void GameEngine::EngineMeasurement::WriteToFile(const std::string & fileName)
{
	if (timer_.GetTimeMiliseconds() > 10000)
	{
		std::ofstream file(fileName, std::ios_base::out);
		for (const auto& m : measurements_)
		{
			file << m.first << ": " << m.second.GetLastValue() <<  "ms (" << m.second.GetMeanValue() << "ms)\n";
		}
		file.close();
		measurements_.clear();
		timer_ = Utils::Timer();
	}
}

void GameEngine::Measurement::Add(uint64 value)
{
	++count_;
	lastValue_ = value;
	meanValue_ = ((count_ - 1.f) * static_cast<float>(value)) / static_cast<float>(count_);
}
