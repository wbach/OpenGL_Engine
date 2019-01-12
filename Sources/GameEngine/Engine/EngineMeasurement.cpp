#include "EngineMeasurement.h"
#include <fstream>

void GameEngine::EngineMeasurement::Add(const std::string& name, uint64_t value)
{
    measurements_[name].Add(value);
}

void GameEngine::EngineMeasurement::WriteToFile(const std::string& fileName)
{
    if (timer_.GetTimeNanoseconds() > 10000000000)
    {
        std::ofstream file(fileName, std::ios_base::out);
        for (const auto& m : measurements_)
        {
            file << m.first << ": " << m.second.GetLastValue() << "ns (" << m.second.GetMeanValue() << "ns)\n";
        }
        file.close();
        measurements_.clear();
        timer_ = Utils::Timer();
    }
}

void GameEngine::Measurement::Add(uint64_t value)
{
    ++count_;
    lastValue_ = value;
    meanValue_ = ((count_ - 1.f) * static_cast<float>(value)) / static_cast<float>(count_);
}
