#include "TerrainConfiguration.h"
#include <Logger/Log.h>
#include <fstream>
#include "Utils.h"

namespace GameEngine
{
const size_t LOD_SIZE = 8;

void SaveTerrainConfigurationToFile(const TerrainConfiguration& config, const std::string& filename)
{
    std::ofstream file;
    file.open(filename, std::ios::out);

    if (not file.is_open())
    {
        ERROR_LOG("Can not open file to writing :" + filename);
        return;
    }

    file << "Scale " << config.GetScale().x << " " << config.GetScale().y << " " << config.GetScale().z << "\n";

    file << "# Tesselation terrain parameters\n";

    const auto& diplacmentStrength = config.GetPerTerrainBuffer().displacementStrength.value;
    file << "DisplacmentStrength " << diplacmentStrength.x << " " << diplacmentStrength.y << " " << diplacmentStrength.z
         << " " << diplacmentStrength.w << "\n";

    for (size_t i = 0; i < LOD_SIZE; ++i)
    {
        file << "SetLod " << std::to_string(i) << " " << config.GetLodRange(i) << "\n";
    }

    if (config.GetPartsCount())
    {
        file << "PartsCount " << *config.GetPartsCount() << "\n";
    }

    file.close();
}

TerrainConfiguration TerrainConfiguration::ReadFromFile(const std::string& filename)
{
    TerrainConfiguration config;

    std::fstream file(filename);

    if (not file.is_open())
    {
        DEBUG_LOG("Terrain config file not found, creating default : " + filename);
        SaveTerrainConfigurationToFile(config, filename);
        return config;
    }

    std::string line;
    int lineNumber = 0;
    while (std::getline(file, line))
    {
        auto params = Utils::SplitString(line, ' ');

        if (params.empty() or params.front().front() == '#')
            continue;

        if (params.front() == "Scale")
        {
            if (params.size() < 4)
            {
                ERROR_LOG("Wrong scale format in line : " + std::to_string(lineNumber));
                continue;
            }

            try
            {
                config.perTerrainBuffer.scale = vec3(stof(params[1]), stof(params[2]), stof(params[3]));
            }
            catch (...)
            {
                ERROR_LOG("Read scale error in line : " + std::to_string(lineNumber));
                continue;
            }
        }
        else if (params.front() == "SetLod")
        {
            if (params.size() < 3)
            {
                ERROR_LOG("Wrong lod format in line : " + std::to_string(lineNumber));
                continue;
            }

            try
            {
                config.SetLod(stoi(params[1]), stoi(params[2]));
            }
            catch (...)
            {
                ERROR_LOG("Read lod error error in line : " + std::to_string(lineNumber));
                continue;
            }
        }
        else if (params.front() == "DisplacmentStrength")
        {
            if (params.size() < 5)
            {
                ERROR_LOG("Wrong DisplacmentStrength format in line : " + std::to_string(lineNumber));
                continue;
            }

            try
            {
                config.perTerrainBuffer.displacementStrength =
                    vec4(stof(params[1]), stof(params[2]), stof(params[3]), stof(params[4]));
            }
            catch (...)
            {
                ERROR_LOG("Read displacmentStrength error error in line : " + std::to_string(lineNumber));
                continue;
            }
        }
        else if (params.front() == "PartsCount")
        {
            if (params.size() < 2)
            {
                ERROR_LOG("Wrong PartsCount format in line : " + std::to_string(lineNumber));
                continue;
            }

            try
            {
                auto partsCount = std::stoi(params[1]);
                if (partsCount > 1)
                {
                    DEBUG_LOG("Terrain parts enabled. " + std::to_string(partsCount) + "x" +
                              std::to_string(partsCount));
                    config.partsCount_ = partsCount;
                }
                else
                {
                    DEBUG_LOG("Terrain parts disabled.");
                }
            }
            catch (...)
            {
                ERROR_LOG("Read PartsCount error in line : " + std::to_string(lineNumber));
                continue;
            }
        }
        ++lineNumber;
    }
    file.close();
    return config;
}

TerrainConfiguration::TerrainConfiguration()
    : perTerrainBuffer{vec4(.3f), vec4i(0), vec4i(0), vec3(6000.f, 800.f, 6000.f)}
    , terrainRootNodesCount_{8.f}
{
    SetLods();
}

TerrainConfiguration::TerrainConfiguration(const vec3& scale)
    : perTerrainBuffer{vec4(.3f), vec4i(0), vec4i(0), scale}
{
    SetLods();
}

void TerrainConfiguration::SetLods()
{
    lodRanges_.resize(LOD_SIZE);
    SetLod(0, 1750);
    SetLod(1, 874);
    SetLod(2, 386);
    SetLod(3, 192);
    SetLod(4, 100);
    SetLod(5, 50);
    SetLod(6, 0);
    SetLod(7, 0);
}

std::optional<uint32> TerrainConfiguration::GetPartsCount() const
{
    return partsCount_;
}

// namespace GameEngine
int32 TerrainConfiguration::updateMorphingArea(uint32 lod)
{
    return static_cast<int32>((perTerrainBuffer.scale.value.x / terrainRootNodesCount_) /
                              static_cast<float>(pow(2, lod)));
}

void TerrainConfiguration::SetLod(uint32 index, uint32 value)
{
    if (index > LOD_SIZE)
    {
        ERROR_LOG("Try set lod out of range! max(" + std::to_string(LOD_SIZE) + ")");
        return;
    }

    lodRanges_[index] = value;

    switch (index)
    {
        case 0:
            perTerrainBuffer.morpharea1_4.value.x = value - updateMorphingArea(index + 1);
            break;
        case 1:
            perTerrainBuffer.morpharea1_4.value.y = value - updateMorphingArea(index + 1);
            break;
        case 2:
            perTerrainBuffer.morpharea1_4.value.z = value - updateMorphingArea(index + 1);
            break;
        case 3:
            perTerrainBuffer.morpharea1_4.value.w = value - updateMorphingArea(index + 1);
            break;
        case 4:
            perTerrainBuffer.morpharea5_8.value.x = value - updateMorphingArea(index + 1);
            break;
        case 5:
            perTerrainBuffer.morpharea5_8.value.y = value - updateMorphingArea(index + 1);
            break;
        case 6:
            perTerrainBuffer.morpharea5_8.value.z = value - updateMorphingArea(index + 1);
            break;
        case 7:
            perTerrainBuffer.morpharea5_8.value.w = value - updateMorphingArea(index + 1);
            break;
        default:
            break;
    }
}

}  // namespace GameEngine
