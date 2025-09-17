#pragma once
#include <vector>

#include "../AbstractLoader.h"

namespace GameEngine
{
class ITextureLoader;

namespace WBLoader
{
class WaveFrontObjLoader : public AbstractLoader
{
public:
    WaveFrontObjLoader(ITextureLoader& textureLodaer);
    bool ParseFile(const File&) override;
    bool CheckExtension(const File&) override;

private:
    void GetFileData(const File& file_name);
    void ProcessFileData();
    void ReadMaterialFile(const std::string& file);
    void ProcesVertex(WBLoader::Object* object, const std::string& value);
    void ProcesNormal(WBLoader::Object* object, const std::string& value);
    void ProcesUVs(WBLoader::Object* object, const std::string& value);
    void ProcesFace(WBLoader::Object* object, WBLoader::Mesh* mesh, const std::string& value);
    void ProcesObject(WBLoader::Object*& object, const std::string& value);
    void ProcesMaterial(WBLoader::Object* object, WBLoader::Mesh*& mesh, const std::string& value);
    void Clear();

private:
    std::vector<std::string> fileData;
    std::string path;
    std::string filename;

    std::list<Material> materials;
    std::vector<vec3> vertex;
    std::vector<vec2> text_coords;
    std::vector<vec3> normals;
};
}  // namespace WBLoader
}  // namespace GameEngine
