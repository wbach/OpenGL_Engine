#pragma once

#include <wx/wx.h>
#include <optional>

#include "WxEditor/EngineRelated/GLCanvas.h"

namespace WxEditor
{
void GenerateForest(wxFrame*, GLCanvas*, const vec2&, const vec2&, float, const std::optional<int>& = std::nullopt);
}
