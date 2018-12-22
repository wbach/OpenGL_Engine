#pragma once
#include "GameEngine/Api/IGraphicsApi.h"
#include "Types.h"

namespace GameEngine
{
class IShaderProgram
{
public:
    virtual ~IShaderProgram()
    {
    }

    virtual void Init()                        = 0;
    virtual void Reload()                      = 0;
    virtual void SetFiles(const ShadersFiles&) = 0;

    virtual bool IsReady() const       = 0;
    virtual bool IsReadyToLoad() const = 0;

    virtual void Start() const = 0;
    virtual void Stop() const  = 0;

    virtual void Load(uint32 varIndex, bool value) const                      = 0;
    virtual void Load(uint32 varIndex, int value) const                       = 0;
    virtual void Load(uint32 varIndex, uint32 value) const                    = 0;
    virtual void Load(uint32 varIndex, float value) const                     = 0;
    virtual void Load(uint32 varIndex, const vec2ui& value) const             = 0;
    virtual void Load(uint32 varIndex, const vec2& value) const               = 0;
    virtual void Load(uint32 varIndex, const vec3& value) const               = 0;
    virtual void Load(uint32 varIndex, const vec4& value) const               = 0;
    virtual void Load(uint32 varIndex, const mat3& value) const               = 0;
    virtual void Load(uint32 varIndex, const mat4& value) const               = 0;
    virtual void Load(uint32 varIndex, const std::vector<float>& value) const = 0;
    virtual void Load(uint32 varIndex, const std::vector<vec3>& value) const  = 0;

    virtual uint32 GetUniformLocation(const std::string& uniformName) const = 0;
};
}  // namespace GameEngine
