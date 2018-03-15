#pragma once
#include "Mesh.h"
#include "../OpenGLObject.h"
#include "GameEngine/Animations/Joint.h"
#include "GameEngine/Animations/AnimationClip.h"
#include <list>
#include <memory>

class CModel : public COpenGLObject
{
public:
	CModel();
    CModel(const CModel& ) = delete;

	virtual ~CModel();
	virtual void InitModel(const std::string&  file_name);
	virtual void OpenGLLoadingPass() override;

	CMesh* AddMesh(CMesh& mesh);
	inline const std::string& GetFileName() const;
	inline const std::vector<CMesh>& GetMeshes() const;
	std::vector<mat4> GetBoneTransforms() const;

	inline bool operator==(const CModel &q) const;
	inline bool operator==(const std::string &file) const;

public:
	GameEngine::Animation::Joint skeleton_;
	std::unordered_map<std::string, GameEngine::Animation::AnimationClip> animationClips_;

protected:
	void AddJoints(GameEngine::Animation::Joint joint, std::vector<mat4>& m) const;

protected:
    std::string filename;
    std::vector<CMesh> meshes;
};

typedef CModel* ModelRawPtr;
typedef std::shared_ptr<CModel> ModelPtr;

const std::vector<CMesh>& CModel::GetMeshes() const
{ 
	return meshes;
}
bool CModel::operator==(const CModel &q) const
{ 
	return filename.compare(q.filename) == 0;
}
bool CModel::operator==(const std::string &file) const
{ 
	return filename.compare(file) == 0;
}
const std::string& CModel::GetFileName() const
{ 
	return filename;
}
