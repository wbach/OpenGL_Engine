#pragma once
#include <OpenGLApi/OpenGLApi.h>

namespace WxEditor
{
class WxOpenGLApiWrapper : public OpenGLApi::OpenGLApi
{
public:
    WxOpenGLApiWrapper(std::unique_ptr<GraphicsApi::IWindowApi>);
    void CreateContext() override;
};
}  // namespace WxEditor
