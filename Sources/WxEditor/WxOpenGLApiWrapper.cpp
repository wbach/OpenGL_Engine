#include "WxOpenGLApiWrapper.h"

#include <Logger/Log.h>

#include <memory>
#include <vector>

#include "WxWindowApi.h"

namespace WxEditor
{
WxOpenGLApiWrapper::WxOpenGLApiWrapper(const vec2i renderSize)
    : OpenGLApi::OpenGLApi(std::make_unique<WxWindowApi>(renderSize))
{
    SetBackgroundColor(Color(0.18f, 0.27f, 0.47f));
}
void WxOpenGLApiWrapper::CreateContext()
{
    DEBUG_LOG("Skip createContext, should already be created");
}
}  // namespace WxEditor
