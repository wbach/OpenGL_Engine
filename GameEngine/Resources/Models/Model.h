#pragma once
#include "Mesh.h"
#include <list>
#include "../OpenGLObject.h"

class CModel : public COpenGLObject
{
public:
	CModel();

	virtual void InitModel(const std::string&  file_name);
    CModel(const CModel& ) = delete;

	virtual ~CModel();
	virtual void Update(float dt) {};	
	virtual const std::vector<glm::mat4>* GetBonesTransforms(uint mesh_id) { return nullptr; }
	virtual void AddCurrentFramePtr(uint* ptr) { m_CurrentFrames.push_back(ptr); }
	virtual void SetCurrentFrame(uint& current_frame) {}
	virtual void SetTime(const float& time) {}

	void OpenGLLoadingPass();
	CMesh* AddMesh(std::vector<float>& positions, std::vector<float>& text_coords, std::vector<float>& normals, std::vector<float>& tangents,
		std::vector<unsigned short>& indices, SMaterial& material, std::vector<SVertexBoneData>& bones);
	const std::list<CMesh>& GetMeshes() const {return m_Meshes;	}
	void SetMaterial(const SMaterial& material, uint mesh = 0);
	glm::vec3 GetNormalizedScaleVector(float x, float y, float z) const;
	void	CalculateBoudnigBox();

	bool operator==(const CModel &q) { return m_Filename.compare(q.m_Filename) == 0; }
	bool operator==(const std::string &file) { return m_Filename.compare(file) == 0; }
	const std::string& GetFileName() {return m_Filename;}

protected:
	std::list<CMesh> m_Meshes;
	std::string m_Filename;
	//For all model
	BoundingBox m_BoundingBox;

	//One model in memory for all entities, so we need have time from outside to update (each entity can have diffrent animation progress)
	std::list<uint*> m_CurrentFrames;

    uint m_Id = 0;
    static uint s_Id;
};
