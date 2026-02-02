#pragma once
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

namespace Utils
{
namespace fs = std::filesystem;

struct TextureSet
{
    fs::path baseColor;
    fs::path roughness;
    fs::path normal;
    fs::path metallic;
    fs::path ambientOcclusion;
    fs::path displacement;
    fs::path opacity;

    friend std::ostream& operator<<(std::ostream& os, const TextureSet& ts)
    {
        os << "TextureSet[\n"
           << "  BaseColor: " << (ts.baseColor.empty() ? "NONE" : ts.baseColor.filename().string()) << "\n"
           << "  Normal:    " << (ts.normal.empty() ? "NONE" : ts.normal.filename().string()) << "\n"
           << "  Roughness: " << (ts.roughness.empty() ? "NONE" : ts.roughness.filename().string()) << "\n"
           << "  Opacity:   " << (ts.opacity.empty() ? "NONE" : ts.opacity.filename().string()) << "\n"
           << "  Disp:      " << (ts.displacement.empty() ? "NONE" : ts.displacement.filename().string()) << "\n"
           << "]";
        return os;
    }
};

TextureSet findTexturesInDirectory(const fs::path&);
}  // namespace Utils
