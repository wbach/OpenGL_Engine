#pragma once
#include <list>
#include <memory>
#include "../OpenGLObject.h"
#include "GameEngine/Animations/AnimationClip.h"
#include "GameEngine/Animations/Joint.h"
#include "Mesh.h"

typedef std::unordered_map<std::string, GameEngine::Animation::AnimationClip> AnimationClipsMap;

class CModel : public COpenGLObject
{
public:
    CModel();
    CModel(float scaleFactor);
    CModel(const CModel&) = delete;

    virtual ~CModel();
    virtual void InitModel(const std::string& file_name);
    virtual void OpenGLLoadingPass() override;

    CMesh* AddMesh(CMesh& mesh);
    CMesh* AddMesh(GameEngine::IGraphicsApiPtr api);
    inline float GetScaleFactor();
    inline const std::string& GetFileName() const;
    inline const std::vector<CMesh>& GetMeshes() const;
    const std::vector<mat4*>& GetBoneTransforms();

    inline bool operator==(const CModel& q) const;
    inline bool operator==(const std::string& file) const;

public:
    GameEngine::Animation::Joint skeleton_;
    AnimationClipsMap animationClips_;

protected:
    void AddJoints(GameEngine::Animation::Joint& joint);

protected:
    std::string filename;
    std::vector<CMesh> meshes;
    std::vector<mat4*> boneTransforms;
    float scaleFactor_;
};

typedef CModel* ModelRawPtr;
typedef std::shared_ptr<CModel> ModelPtr;

const std::vector<CMesh>& CModel::GetMeshes() const
{
    return meshes;
}
bool CModel::operator==(const CModel& q) const
{
    return filename.compare(q.filename) == 0;
}
bool CModel::operator==(const std::string& file) const
{
    return filename.compare(file) == 0;
}
const std::string& CModel::GetFileName() const
{
    return filename;
}
float CModel::GetScaleFactor()
{
    return scaleFactor_;
}
