#include "WxOpenGLApiWrapper.h"

#include <Logger/Log.h>

#include <memory>
#include <vector>

namespace WxEditor
{
WxOpenGLApiWrapper::WxOpenGLApiWrapper(std::unique_ptr<GraphicsApi::IWindowApi> api)
    : OpenGLApi::OpenGLApi(std::move(api))
{
    SetBackgroundColor(Color(0.18f, 0.27f, 0.47f));
}
void WxOpenGLApiWrapper::CreateContext()
{
    DEBUG_LOG("Skip createContext, should already be created");
}
}  // namespace WxEditor
