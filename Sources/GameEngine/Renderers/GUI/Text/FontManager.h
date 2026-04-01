#pragma once
#include <Types.h>
#include <Utils/IdPool.h>

#include <unordered_map>

#include "IFontManager.h"

namespace GameEngine
{
class ENGINE_API FontManager : public IFontManager
{
public:
    FontManager();
    ~FontManager() override;

    std::optional<uint32> openFont(const File&, FontStyle, uint32, uint32) override;
    std::optional<TextureData> renderFont(uint32, const std::string&, uint32) override;
    void closeFont(uint32) override;

private:
    std::string getFontName(const File&, FontStyle, uint32, uint32) const;

private:
    struct Pimpl;
    Utils::IdPool idPool;
    std::unique_ptr<Pimpl> impl_;
    std::unordered_map<std::string, uint32> fontNameToIdMap_;
    bool isInit_;
};
}  // namespace GameEngine
