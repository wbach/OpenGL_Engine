#pragma once
#include <OpenGLApi/OpenGLApi.h>

namespace WxEditor
{
enum WxKeyEventType
{
    WX_KEY_UP,
    WX_KEY_DOWN
};
class WxOpenGLApiWrapper : public OpenGLApi::OpenGLApi
{
public:
    WxOpenGLApiWrapper();
    void CreateContext() override;
};
}  // namespace WxEditor
