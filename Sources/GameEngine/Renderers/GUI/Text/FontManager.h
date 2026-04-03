#pragma once
#include <Types.h>
#include <Utils/IdPool.h>

#include <optional>
#include <unordered_map>

#include "IFontManager.h"

namespace GameEngine
{
class ENGINE_API FontManager : public IFontManager
{
public:
    FontManager();
    ~FontManager() override;

    std::optional<IdType> openFont(FontStyle, FontSize, FontOutline, const std::optional<File>&) override;
    std::optional<TextureData> renderText(const std::string&, IdType, const std::optional<TextWrapWidth>&) override;
    void closeFont(IdType) override;

private:
    std::string getFontName(const std::optional<File>&, FontStyle, uint32, uint32) const;

private:
    struct Pimpl;
    Utils::IdPool idPool;
    std::unique_ptr<Pimpl> impl_;
    std::unordered_map<std::string, uint32> fontNameToIdMap_;
    bool isInit_;
};
}  // namespace GameEngine
