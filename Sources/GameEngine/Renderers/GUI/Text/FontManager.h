#pragma once
#include <Types.h>

#include <memory>
#include <optional>
#include <unordered_map>
#include "Surface.h"
#include "RenderedText.h"

namespace GameEngine
{
class FontManager
{
public:
    FontManager(const vec2ui& windowSize);
    ~FontManager();

    std::optional<uint32> OpenFont(const std::string& filename, uint32 size);
    std::optional<Surface> RenderFont(uint32 id, const std::string& text, const vec4& color, uint32 outline);
    void DeleteSurface(uint32 surfaceId);

private:
    struct Pimpl;
    std::unique_ptr<Pimpl> impl_;
    std::unordered_map<std::string, uint32> fontNameToIdMap_;
    std::vector<RenderedText> rendererdTexts_;
    const vec2ui& windowSize_;
};
}  // namespace GameEngine