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
    virtual void Update(float) {};
	virtual const std::vector<glm::mat4>* GetBonesTransforms(uint mesh_id) { return nullptr; }
    virtual void AddCurrentFramePtr(uint* ptr) { currentFrames.push_back(ptr); }
	virtual void SetCurrentFrame(uint& current_frame) {}
	virtual void SetTime(const float& time) {}

	void OpenGLLoadingPass();
	CMesh* AddMesh(const SMaterial& material, const std::vector<float>& positions, const std::vector<float>& text_coords = std::vector<float>(), const std::vector<float>& normals = std::vector<float>(), const std::vector<float>& tangents = std::vector<float>(),
		const std::vector<unsigned short>& indices = std::vector<unsigned short>(), const std::vector<SVertexBoneData>& bones = std::vector<SVertexBoneData>());
	CMesh* AddMesh(CMesh& mesh);
    const std::list<CMesh>& GetMeshes() const {return meshes;	}
	void SetMaterial(const SMaterial& material, uint mesh = 0);
	glm::vec3 GetNormalizedScaleVector(float x, float y, float z) const;
	void	CalculateBoudnigBox();

    bool operator==(const CModel &q) { return filename.compare(q.filename) == 0; }
    bool operator==(const std::string &file) { return filename.compare(file) == 0; }
    const std::string& GetFileName() {return filename;}

protected:
    std::list<CMesh> meshes;
    std::string filename;
	//For all model
    BoundingBox boundingBox;

	//One model in memory for all entities, so we need have time from outside to update (each entity can have diffrent animation progress)
    std::list<uint*> currentFrames;

    uint id = 0;
    static uint s_id;
};
