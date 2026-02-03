#pragma once

#include <wx/wx.h>

#include "WxEditor/EngineRelated/GLCanvas.h"

struct TreeGenerationParams;

namespace WxEditor
{
void GenerateTree(wxFrame*, GLCanvas*);
void GenerateTree(wxFrame*, GLCanvas*, const TreeGenerationParams&, const vec3&, GameEngine::GameObject* = nullptr);
}  // namespace WxEditor
