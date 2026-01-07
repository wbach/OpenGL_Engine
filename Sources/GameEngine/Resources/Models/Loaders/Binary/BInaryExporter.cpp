#include "BInaryExporter.h"

#include <GraphicsApi/MeshRawDataSerilizeHelpers.h>
#include <Logger/Log.h>
#include <bitsery/adapter/stream.h>
#include <bitsery/bitsery.h>

#include <fstream>

#include "GameEngine/Resources/Models/Mesh.h"
#include "GameEngine/Resources/Models/Model.h"

namespace GameEngine
{

template <typename S>
void serialize(S& s, GameEngine::Mesh& mesh)
{
    s.object(mesh.GetMeshDataRef());
}

template <typename S>
void serialize(S& s, GameEngine::Model& model)
{
    constexpr size_t MaxMeshes = 1024;
    s.container(model.GetMeshes(), MaxMeshes);
}

void ExportModelBinary(const std::unique_ptr<Model>& model, const std::filesystem::path& path)
{
    std::ofstream file(path, std::ios::binary);
    if (!file)
    {
        LOG_WARN << "Open file error: " << path;
        return;
    }

    bitsery::Serializer<bitsery::OutputStreamAdapter> ser{file};
    ser.object(*model);

    if (!file.good())
    {
        LOG_ERROR << "File write error: " << path;
    }
}
}  // namespace GameEngine
