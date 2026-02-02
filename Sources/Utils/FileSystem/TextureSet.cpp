#include "TextureSet.h"

namespace Utils
{
TextureSet findTexturesInDirectory(const fs::path& directory)
{
    TextureSet set;
    if (!fs::exists(directory) || !fs::is_directory(directory))
        return set;

    // Definicja par: Pole w strukturze <-> Słowa kluczowe
    std::vector<std::pair<fs::path*, std::vector<std::string>>> mappings = {
        {&set.baseColor, {"color", "albedo", "diffuse", "basecolor"}},
        {&set.roughness, {"roughness", "rough", "gloss"}},
        {&set.normal, {"normalgl", "normaldx", "normal"}},
        {&set.metallic, {"metallic", "metalness", "metal"}},
        {&set.ambientOcclusion, {"ao", "ambientocclusion", "occlusion"}},
        {&set.displacement, {"displacement", "height", "disp"}},
        {&set.opacity, {"opacity", "alpha", "mask"}}};

    for (const auto& entry : fs::directory_iterator(directory))
    {
        if (!entry.is_regular_file())
            continue;

        std::string filename = entry.path().stem().string();
        std::transform(filename.begin(), filename.end(), filename.begin(), ::tolower);

        for (auto& [field, keywords] : mappings)
        {
            for (const auto& key : keywords)
            {
                if (filename.find(key) != std::string::npos)
                {
                    // Prosty priorytet: jeśli pole jest puste LUB znaleźliśmy
                    // bardziej precyzyjne dopasowanie (np. "normalgl" zamiast "normal")
                    if (field->empty() || key.length() > 5)
                    {  // uproszczona logika priorytetu
                        *field = entry.path();
                    }
                    break;
                }
            }
        }
    }
    return set;
}
}  // namespace Utils
