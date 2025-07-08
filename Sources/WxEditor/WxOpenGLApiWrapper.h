#pragma once
#include <OpenGLApi/OpenGLApi.h>

namespace WxEditor
{
class WxOpenGLApiWrapper : public OpenGLApi::OpenGLApi
{
public:
    WxOpenGLApiWrapper();
    void Init() override;
    void CreateContext() override;
};
}  // namespace WxEditor
