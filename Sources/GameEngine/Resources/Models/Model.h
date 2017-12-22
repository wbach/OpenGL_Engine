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
	virtual const std::vector<mat4>* GetBonesTransforms(uint32 mesh_id) { return nullptr; }
    virtual void AddCurrentFramePtr(uint32* ptr) { currentFrames.push_back(ptr); }
	virtual void SetCurrentFrame(uint32& current_frame) {}
	virtual void SetTime(const float& time) {}

	virtual void OpenGLLoadingPass() override;

	CMesh* AddMesh(const SMaterial& material, const std::vector<float>& positions, const std::vector<float>& text_coords = std::vector<float>(), const std::vector<float>& normals = std::vector<float>(), const std::vector<float>& tangents = std::vector<float>(),
		const std::vector<uint16>& indices = std::vector<uint16>(), const std::vector<SVertexBoneData>& bones = std::vector<SVertexBoneData>());
	CMesh* AddMesh(CMesh& mesh);
    const std::list<CMesh>& GetMeshes() const {return meshes;	}
	void SetMaterial(const SMaterial& material, uint32 mesh = 0);
	vec3 GetNormalizedScaleVector(float x, float y, float z) const;
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
    std::list<uint32*> currentFrames;

    uint32 id = 0;
    static uint32 s_id;
};
