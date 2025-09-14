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
        LOG_ERROR << "Can not open file to writing :" << filename;
        return;
    }

    file << "Scale " << config.GetScale().x << " " << config.GetScale().y << " " << config.GetScale().z << "\n";

    file << "# Tesselation terrain parameters\n";

    const auto& diplacmentStrength = config.GetPerTerrainBuffer().displacementStrength.value;
    file << "DisplacmentStrength " << diplacmentStrength.x << " " << diplacmentStrength.y << " " << diplacmentStrength.z << " "
         << diplacmentStrength.w << "\n";

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

TerrainConfiguration TerrainConfiguration::ReadFromFile(const File& configFile)
{
    LOG_DEBUG << "filename : " << configFile.GetDataRelativePath();
    TerrainConfiguration config;

    std::fstream file(configFile.GetAbsolutePath());

    if (not file.is_open())
    {
        LOG_DEBUG << "Terrain config file not found, creating default : " << configFile.GetFilename();
        SaveTerrainConfigurationToFile(config, configFile.GetAbsolutePath().string());
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
                LOG_ERROR << "Wrong scale format in line : " << lineNumber;
                continue;
            }

            try
            {
                config.scale_ = vec3(stof(params[1]), stof(params[2]), stof(params[3]));
            }
            catch (...)
            {
                LOG_ERROR << "Read scale error in line : " << lineNumber;
                continue;
            }
        }
        else if (params.front() == "SetLod")
        {
            if (params.size() < 3)
            {
                LOG_ERROR << "Wrong lod format in line : " << lineNumber;
                continue;
            }

            try
            {
                config.SetLod(stoi(params[1]), stoi(params[2]));
            }
            catch (...)
            {
                LOG_ERROR << "Read lod error error in line : " << lineNumber;
                continue;
            }
        }
        else if (params.front() == "DisplacmentStrength")
        {
            if (params.size() < 5)
            {
                LOG_ERROR << "Wrong DisplacmentStrength format in line : " << lineNumber;
                continue;
            }

            try
            {
                config.perTerrainBuffer.displacementStrength =
                    vec4(stof(params[1]), stof(params[2]), stof(params[3]), stof(params[4]));
            }
            catch (...)
            {
                LOG_ERROR << "Read displacmentStrength error error in line : " << lineNumber;
                continue;
            }
        }
        else if (params.front() == "PartsCount")
        {
            if (params.size() < 2)
            {
                LOG_ERROR << "Wrong PartsCount format in line : " << lineNumber;
                continue;
            }

            try
            {
                auto partsCount = std::stoi(params[1]);
                if (partsCount > 1)
                {
                    LOG_DEBUG << "Terrain parts enabled. " << partsCount << "x" << partsCount;
                    config.partsCount_ = partsCount;
                }
                else
                {
                    LOG_DEBUG << "Terrain parts disabled.";
                }
            }
            catch (...)
            {
                LOG_ERROR << "Read PartsCount error in line : " << lineNumber;
                continue;
            }
        }
        ++lineNumber;
    }
    config.perTerrainBuffer.scaleAndYOffset.value = vec4(config.scale_, 0.f);
    file.close();
    return config;
}

TerrainConfiguration::TerrainConfiguration()
    : scale_(513, 20, 513)
    , perTerrainBuffer{{vec4{.3f}}, {vec4i{0}}, {vec4i{0}}, {vec4{scale_, 0.f}}}
    , terrainRootNodesCount_{8.f}
{
    SetLods();
}

TerrainConfiguration::TerrainConfiguration(const vec3& scale)
    : scale_(scale)
    , perTerrainBuffer{{vec4(.3f)}, {vec4i(0)}, {vec4i(0)}, {vec4(scale, 0.f)}}
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

void TerrainConfiguration::SetTerrainYOffset(float offset)
{
    perTerrainBuffer.scaleAndYOffset.value.w = offset;
}

// namespace GameEngine
int32 TerrainConfiguration::updateMorphingArea(uint32 lod)
{
    return static_cast<int32>((scale_.x / terrainRootNodesCount_) / static_cast<float>(pow(2, lod)));
}

void TerrainConfiguration::SetLod(uint32 index, uint32 value)
{
    if (index > LOD_SIZE)
    {
        LOG_ERROR << "Try set lod out of range! max(" << LOD_SIZE <<  ")";
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
