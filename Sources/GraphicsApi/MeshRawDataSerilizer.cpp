#include "MeshRawDataSerilizer.h"

#include <bitsery/adapter/stream.h>
#include <bitsery/bitsery.h>
#include <bitsery/traits/string.h>
#include <bitsery/traits/vector.h>

#include <filesystem>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <optional>

#include "MeshRawDataSerilizeHelpers.h"

namespace GraphicsApi
{
bool SaveMeshRawDataBinary(const MeshRawData& mesh, const std::filesystem::path& path)
{
    std::ofstream file(path, std::ios::binary);
    if (!file.is_open())
        return false;

    bitsery::Serializer<bitsery::OutputStreamAdapter> ser{file};
    ser.object(const_cast<MeshRawData&>(mesh));
    ser.adapter().flush();

    return true;
}

inline std::optional<MeshRawData> LoadMeshRawDataBinary(const std::filesystem::path& path)
{
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open())
        return std::nullopt;

    MeshRawData mesh;
    bitsery::Deserializer<bitsery::InputStreamAdapter> des{file};
    des.object(mesh);

    if (des.adapter().error() != bitsery::ReaderError::NoError)
        return std::nullopt;

    return mesh;
}
}  // namespace GraphicsApi
