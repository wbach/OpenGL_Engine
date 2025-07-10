#pragma once
#include <OpenGLApi/OpenGLApi.h>

namespace WxEditor
{
class WxOpenGLApiWrapper : public OpenGLApi::OpenGLApi
{
public:
    WxOpenGLApiWrapper(const vec2i);
    void CreateContext() override;
};
}  // namespace WxEditor
