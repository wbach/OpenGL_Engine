#include "Primitives.h"

namespace GameEngine
{
GraphicsApi::MeshRawData GenerateSphere(float radius, unsigned int latSegments, unsigned int lonSegments)
{
    GraphicsApi::MeshRawData mesh;

    for (unsigned int y = 0; y <= latSegments; ++y)
    {
        float v     = float(y) / latSegments;
        float theta = v * glm::pi<float>();

        for (unsigned int x = 0; x <= lonSegments; ++x)
        {
            float u   = float(x) / lonSegments;
            float phi = u * 2.0f * glm::pi<float>();

            float px = radius * sin(theta) * cos(phi);
            float py = radius * cos(theta);
            float pz = radius * sin(theta) * sin(phi);
            mesh.positions_.push_back(px);
            mesh.positions_.push_back(py);
            mesh.positions_.push_back(pz);

            glm::vec3 normal = glm::normalize(glm::vec3(px, py, pz));
            mesh.normals_.push_back(normal.x);
            mesh.normals_.push_back(normal.y);
            mesh.normals_.push_back(normal.z);

            glm::vec3 tangent(-sin(phi), 0.0f, cos(phi));
            glm::vec3 bitangent = glm::cross(normal, tangent);
            mesh.tangents_.push_back(tangent.x);
            mesh.tangents_.push_back(tangent.y);
            mesh.tangents_.push_back(tangent.z);

            mesh.bitangents_.push_back(bitangent.x);
            mesh.bitangents_.push_back(bitangent.y);
            mesh.bitangents_.push_back(bitangent.z);

            mesh.textCoords_.push_back(u);
            mesh.textCoords_.push_back(v);

            mesh.bonesWeights_.push_back(0.0f);
            mesh.bonesWeights_.push_back(0.0f);
            mesh.bonesWeights_.push_back(0.0f);
            mesh.bonesWeights_.push_back(0.0f);
            mesh.joinIds_.push_back(-1);
            mesh.joinIds_.push_back(-1);
            mesh.joinIds_.push_back(-1);
            mesh.joinIds_.push_back(-1);
        }
    }

    for (unsigned int y = 0; y < latSegments; ++y)
    {
        for (unsigned int x = 0; x < lonSegments; ++x)
        {
            unsigned int i0 = y * (lonSegments + 1) + x;
            unsigned int i1 = i0 + 1;
            unsigned int i2 = i0 + (lonSegments + 1);
            unsigned int i3 = i2 + 1;

            mesh.indices_.push_back(i0);
            mesh.indices_.push_back(i1);
            mesh.indices_.push_back(i2);

            mesh.indices_.push_back(i1);
            mesh.indices_.push_back(i3);
            mesh.indices_.push_back(i2);
        }
    }

    return mesh;
}

GraphicsApi::MeshRawData GenerateCube(float size)
{
    GraphicsApi::MeshRawData mesh;
    float half = size * 0.5f;

    glm::vec3 vertices[8] = {{-half, -half, -half}, {half, -half, -half}, {half, half, -half}, {-half, half, -half},
                             {-half, -half, half},  {half, -half, half},  {half, half, half},  {-half, half, half}};

    unsigned int indices[36] = {// tyl (-Z)
                                0, 2, 1, 0, 3, 2,
                                // przod (+Z)
                                4, 5, 6, 4, 6, 7,
                                // lewa (-X)
                                0, 7, 3, 0, 4, 7,
                                // prawa (+X)
                                1, 2, 6, 1, 6, 5,
                                // gora (+Y)
                                3, 7, 6, 3, 6, 2,
                                // dol (-Y)
                                0, 1, 5, 0, 5, 4};

    for (int i = 0; i < 8; ++i)
    {
        mesh.positions_.push_back(vertices[i].x);
        mesh.positions_.push_back(vertices[i].y);
        mesh.positions_.push_back(vertices[i].z);

        glm::vec3 n = glm::normalize(vertices[i]);
        mesh.normals_.push_back(n.x);
        mesh.normals_.push_back(n.y);
        mesh.normals_.push_back(n.z);

        glm::vec3 tangent(1, 0, 0);
        glm::vec3 bitangent(0, 1, 0);
        mesh.tangents_.push_back(tangent.x);
        mesh.tangents_.push_back(tangent.y);
        mesh.tangents_.push_back(tangent.z);

        mesh.bitangents_.push_back(bitangent.x);
        mesh.bitangents_.push_back(bitangent.y);
        mesh.bitangents_.push_back(bitangent.z);

        mesh.textCoords_.push_back((vertices[i].x + half) / size);
        mesh.textCoords_.push_back((vertices[i].y + half) / size);

        for (int k = 0; k < 4; k++)
            mesh.bonesWeights_.push_back(0.0f);
        for (int k = 0; k < 4; k++)
            mesh.joinIds_.push_back(-1);
    }

    mesh.indices_.insert(mesh.indices_.end(), std::begin(indices), std::end(indices));
    return mesh;
}

GraphicsApi::MeshRawData GenerateCylinder(float radius, float height, unsigned int segments)
{
    GraphicsApi::MeshRawData mesh;
    float half = height * 0.5f;

    std::vector<unsigned int> bottomCircleIndices;
    std::vector<unsigned int> topCircleIndices;

    // --- Wierzcholki boczne
    for (unsigned int i = 0; i <= segments; ++i)
    {
        float theta = i * 2.0f * glm::pi<float>() / segments;
        float x     = radius * cos(theta);
        float z     = radius * sin(theta);

        glm::vec3 normal = glm::normalize(glm::vec3(x, 0.0f, z));
        glm::vec3 tangent(-sin(theta), 0.0f, cos(theta));
        glm::vec3 bitangent(0.0f, 1.0f, 0.0f);

        // dolna krawedz
        mesh.positions_.push_back(x);
        mesh.positions_.push_back(-half);
        mesh.positions_.push_back(z);
        mesh.normals_.push_back(normal.x);
        mesh.normals_.push_back(normal.y);
        mesh.normals_.push_back(normal.z);
        mesh.tangents_.push_back(tangent.x);
        mesh.tangents_.push_back(tangent.y);
        mesh.tangents_.push_back(tangent.z);
        mesh.bitangents_.push_back(bitangent.x);
        mesh.bitangents_.push_back(bitangent.y);
        mesh.bitangents_.push_back(bitangent.z);
        mesh.textCoords_.push_back(float(i) / segments);
        mesh.textCoords_.push_back(0.0f);

        // gorna krawedz
        mesh.positions_.push_back(x);
        mesh.positions_.push_back(half);
        mesh.positions_.push_back(z);
        mesh.normals_.push_back(normal.x);
        mesh.normals_.push_back(normal.y);
        mesh.normals_.push_back(normal.z);
        mesh.tangents_.push_back(tangent.x);
        mesh.tangents_.push_back(tangent.y);
        mesh.tangents_.push_back(tangent.z);
        mesh.bitangents_.push_back(bitangent.x);
        mesh.bitangents_.push_back(bitangent.y);
        mesh.bitangents_.push_back(bitangent.z);
        mesh.textCoords_.push_back(float(i) / segments);
        mesh.textCoords_.push_back(1.0f);

        for (int k = 0; k < 4; k++)
            mesh.bonesWeights_.push_back(0.0f);
        for (int k = 0; k < 4; k++)
            mesh.joinIds_.push_back(-1);

        bottomCircleIndices.push_back(i * 2);
        topCircleIndices.push_back(i * 2 + 1);
    }

    // --- Indeksy bocznych scian (CCW patrzac z zewnatrz)
    for (unsigned int i = 0; i < segments; ++i)
    {
        unsigned int i0 = bottomCircleIndices[i];
        unsigned int i1 = topCircleIndices[i];
        unsigned int i2 = bottomCircleIndices[i + 1];
        unsigned int i3 = topCircleIndices[i + 1];

        mesh.indices_.push_back(i0);
        mesh.indices_.push_back(i1);
        mesh.indices_.push_back(i2);

        mesh.indices_.push_back(i1);
        mesh.indices_.push_back(i3);
        mesh.indices_.push_back(i2);
    }

    // --- Dolna podstawa
    unsigned int baseCenterIndex = mesh.positions_.size() / 3;
    mesh.positions_.push_back(0.0f);
    mesh.positions_.push_back(-half);
    mesh.positions_.push_back(0.0f);
    mesh.normals_.push_back(0.0f);
    mesh.normals_.push_back(-1.0f);
    mesh.normals_.push_back(0.0f);
    mesh.tangents_.push_back(1.0f);
    mesh.tangents_.push_back(0.0f);
    mesh.tangents_.push_back(0.0f);
    mesh.bitangents_.push_back(0.0f);
    mesh.bitangents_.push_back(0.0f);
    mesh.bitangents_.push_back(1.0f);
    mesh.textCoords_.push_back(0.5f);
    mesh.textCoords_.push_back(0.5f);
    for (int k = 0; k < 4; k++)
        mesh.bonesWeights_.push_back(0.0f);
    for (int k = 0; k < 4; k++)
        mesh.joinIds_.push_back(-1);

    for (unsigned int i = 0; i <= segments; i++)
    {
        float theta = i * 2.0f * glm::pi<float>() / segments;
        float x     = radius * cos(theta);
        float z     = radius * sin(theta);

        mesh.positions_.push_back(x);
        mesh.positions_.push_back(-half);
        mesh.positions_.push_back(z);

        mesh.normals_.push_back(0.0f);
        mesh.normals_.push_back(-1.0f);
        mesh.normals_.push_back(0.0f);

        glm::vec3 tangent   = glm::normalize(glm::vec3(x, 0.0f, z));
        glm::vec3 bitangent = glm::normalize(glm::cross(glm::vec3(0.0f, -1.0f, 0.0f), tangent));

        mesh.tangents_.push_back(tangent.x);
        mesh.tangents_.push_back(tangent.y);
        mesh.tangents_.push_back(tangent.z);
        mesh.bitangents_.push_back(bitangent.x);
        mesh.bitangents_.push_back(bitangent.y);
        mesh.bitangents_.push_back(bitangent.z);

        mesh.textCoords_.push_back(0.5f + 0.5f * cos(theta));
        mesh.textCoords_.push_back(0.5f + 0.5f * sin(theta));

        for (int k = 0; k < 4; k++)
            mesh.bonesWeights_.push_back(0.0f);
        for (int k = 0; k < 4; k++)
            mesh.joinIds_.push_back(-1);
    }

    for (unsigned int i = 0; i < segments; i++)
    {
        unsigned int i0 = baseCenterIndex;
        unsigned int i1 = baseCenterIndex + i + 1;
        unsigned int i2 = baseCenterIndex + i + 2;
        mesh.indices_.push_back(i0);
        mesh.indices_.push_back(i1);
        mesh.indices_.push_back(i2);
    }

    // --- Gorna podstawa
    unsigned int topCenterIndex = mesh.positions_.size() / 3;
    mesh.positions_.push_back(0.0f);
    mesh.positions_.push_back(half);
    mesh.positions_.push_back(0.0f);
    mesh.normals_.push_back(0.0f);
    mesh.normals_.push_back(1.0f);
    mesh.normals_.push_back(0.0f);
    mesh.tangents_.push_back(1.0f);
    mesh.tangents_.push_back(0.0f);
    mesh.tangents_.push_back(0.0f);
    mesh.bitangents_.push_back(0.0f);
    mesh.bitangents_.push_back(0.0f);
    mesh.bitangents_.push_back(1.0f);
    mesh.textCoords_.push_back(0.5f);
    mesh.textCoords_.push_back(0.5f);
    for (int k = 0; k < 4; k++)
        mesh.bonesWeights_.push_back(0.0f);
    for (int k = 0; k < 4; k++)
        mesh.joinIds_.push_back(-1);

    for (unsigned int i = 0; i <= segments; i++)
    {
        float theta = i * 2.0f * glm::pi<float>() / segments;
        float x     = radius * cos(theta);
        float z     = radius * sin(theta);

        mesh.positions_.push_back(x);
        mesh.positions_.push_back(half);
        mesh.positions_.push_back(z);

        mesh.normals_.push_back(0.0f);
        mesh.normals_.push_back(1.0f);
        mesh.normals_.push_back(0.0f);

        glm::vec3 tangent   = glm::normalize(glm::vec3(x, 0.0f, z));
        glm::vec3 bitangent = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), tangent));

        mesh.tangents_.push_back(tangent.x);
        mesh.tangents_.push_back(tangent.y);
        mesh.tangents_.push_back(tangent.z);
        mesh.bitangents_.push_back(bitangent.x);
        mesh.bitangents_.push_back(bitangent.y);
        mesh.bitangents_.push_back(bitangent.z);

        mesh.textCoords_.push_back(0.5f + 0.5f * cos(theta));
        mesh.textCoords_.push_back(0.5f + 0.5f * sin(theta));

        for (int k = 0; k < 4; k++)
            mesh.bonesWeights_.push_back(0.0f);
        for (int k = 0; k < 4; k++)
            mesh.joinIds_.push_back(-1);
    }

    for (unsigned int i = 0; i < segments; i++)
    {
        unsigned int i0 = topCenterIndex;
        unsigned int i1 = topCenterIndex + i + 2;
        unsigned int i2 = topCenterIndex + i + 1;
        mesh.indices_.push_back(i0);
        mesh.indices_.push_back(i1);
        mesh.indices_.push_back(i2);
    }

    return mesh;
}

GraphicsApi::MeshRawData GenerateCone(float radius, float height, unsigned int segments)
{
    GraphicsApi::MeshRawData mesh;

    // --- Wierzcholek stozka (tip)
    unsigned int tipIndex = 0;
    mesh.positions_.push_back(0.0f);
    mesh.positions_.push_back(height * 0.5f);
    mesh.positions_.push_back(0.0f);

    // tymczasowo normal (przypiszemy pozniej)
    mesh.normals_.push_back(0.0f);
    mesh.normals_.push_back(0.0f);
    mesh.normals_.push_back(0.0f);

    mesh.tangents_.push_back(1.0f);
    mesh.tangents_.push_back(0.0f);
    mesh.tangents_.push_back(0.0f);

    mesh.bitangents_.push_back(0.0f);
    mesh.bitangents_.push_back(0.0f);
    mesh.bitangents_.push_back(1.0f);

    mesh.textCoords_.push_back(0.5f);
    mesh.textCoords_.push_back(1.0f);

    // --- Centrum podstawy
    unsigned int baseCenterIndex = 1;
    mesh.positions_.push_back(0.0f);
    mesh.positions_.push_back(-height * 0.5f);
    mesh.positions_.push_back(0.0f);

    mesh.normals_.push_back(0.0f);
    mesh.normals_.push_back(-1.0f);
    mesh.normals_.push_back(0.0f);

    mesh.tangents_.push_back(1.0f);
    mesh.tangents_.push_back(0.0f);
    mesh.tangents_.push_back(0.0f);

    mesh.bitangents_.push_back(0.0f);
    mesh.bitangents_.push_back(0.0f);
    mesh.bitangents_.push_back(1.0f);

    mesh.textCoords_.push_back(0.5f);
    mesh.textCoords_.push_back(0.0f);

    // --- Punkty na okregu podstawy
    std::vector<glm::vec3> circlePos;
    for (unsigned int i = 0; i <= segments; i++)
    {
        float theta = (float)i / segments * glm::two_pi<float>();
        float x     = radius * cos(theta);
        float z     = radius * sin(theta);

        glm::vec3 pos(x, -height * 0.5f, z);
        circlePos.push_back(pos);

        mesh.positions_.push_back(x);
        mesh.positions_.push_back(-height * 0.5f);
        mesh.positions_.push_back(z);

        // normal dla podstawy
        mesh.normals_.push_back(0.0f);
        mesh.normals_.push_back(-1.0f);
        mesh.normals_.push_back(0.0f);

        // tangent wzdluz okregu
        mesh.tangents_.push_back(-sin(theta));
        mesh.tangents_.push_back(0.0f);
        mesh.tangents_.push_back(cos(theta));

        // bitangent w osi Y
        mesh.bitangents_.push_back(0.0f);
        mesh.bitangents_.push_back(1.0f);
        mesh.bitangents_.push_back(0.0f);

        mesh.textCoords_.push_back((x / radius + 1.0f) * 0.5f);
        mesh.textCoords_.push_back((z / radius + 1.0f) * 0.5f);
    }

    // --- Indeksy podstawy (CCW patrzac od dolu)
    for (unsigned int i = 0; i < segments; i++)
    {
        mesh.indices_.push_back(baseCenterIndex);
        mesh.indices_.push_back(baseCenterIndex + i + 2);
        mesh.indices_.push_back(baseCenterIndex + i + 1);
    }

    // --- Sciany boczne (CCW patrzac z zewnatrz)
    for (unsigned int i = 0; i < segments; i++)
    {
        unsigned int i0 = tipIndex;                 // wierzcholek tip
        unsigned int i1 = baseCenterIndex + i + 1;  // pierwszy punkt na okregu
        unsigned int i2 = baseCenterIndex + i + 2;  // nastepny punkt na okregu

        // --- Dodajemy indeksy (CCW patrzac z zewnatrz)
        mesh.indices_.push_back(i0);
        mesh.indices_.push_back(i2);
        mesh.indices_.push_back(i1);

        // --- Pobranie pozycji wierzcholkow
        glm::vec3 pTip(0.0f, height / 2, 0.0f);
        glm::vec3 p0 = circlePos[i];
        glm::vec3 p1 = circlePos[i + 1];

        // --- Normalne dla scian bocznych
        glm::vec3 n0   = glm::normalize(glm::vec3(p0.x, radius / height, p0.z));
        glm::vec3 n1   = glm::normalize(glm::vec3(p1.x, radius / height, p1.z));
        glm::vec3 nTip = glm::normalize(glm::vec3(0.0f, radius / height, 0.0f));

        // --- Tangenty i bitangenty
        glm::vec3 tangentTip = glm::normalize(glm::vec3(-0.0f, 0.0f, 0.0f));  // mozna interpolowac lub przyblizyc
        glm::vec3 tangent0   = glm::normalize(
              glm::vec3(-sin((float)i / segments * glm::two_pi<float>()), 0.0f, cos((float)i / segments * glm::two_pi<float>())));
        glm::vec3 tangent1 = glm::normalize(glm::vec3(-sin((float)(i + 1) / segments * glm::two_pi<float>()), 0.0f,
                                                      cos((float)(i + 1) / segments * glm::two_pi<float>())));

        glm::vec3 bitangentTip = glm::normalize(glm::cross(nTip, tangentTip));
        glm::vec3 bitangent0   = glm::normalize(glm::cross(n0, tangent0));
        glm::vec3 bitangent1   = glm::normalize(glm::cross(n1, tangent1));

        // --- Przypisanie do tablic
        size_t offset;

        // tip
        offset                       = i0 * 3;
        mesh.normals_[offset + 0]    = nTip.x;
        mesh.normals_[offset + 1]    = nTip.y;
        mesh.normals_[offset + 2]    = nTip.z;
        mesh.tangents_[offset + 0]   = tangentTip.x;
        mesh.tangents_[offset + 1]   = tangentTip.y;
        mesh.tangents_[offset + 2]   = tangentTip.z;
        mesh.bitangents_[offset + 0] = bitangentTip.x;
        mesh.bitangents_[offset + 1] = bitangentTip.y;
        mesh.bitangents_[offset + 2] = bitangentTip.z;

        // wierzcholek i1
        offset                       = i1 * 3;
        mesh.normals_[offset + 0]    = n0.x;
        mesh.normals_[offset + 1]    = n0.y;
        mesh.normals_[offset + 2]    = n0.z;
        mesh.tangents_[offset + 0]   = tangent0.x;
        mesh.tangents_[offset + 1]   = tangent0.y;
        mesh.tangents_[offset + 2]   = tangent0.z;
        mesh.bitangents_[offset + 0] = bitangent0.x;
        mesh.bitangents_[offset + 1] = bitangent0.y;
        mesh.bitangents_[offset + 2] = bitangent0.z;

        // wierzcholek i2
        offset                       = i2 * 3;
        mesh.normals_[offset + 0]    = n1.x;
        mesh.normals_[offset + 1]    = n1.y;
        mesh.normals_[offset + 2]    = n1.z;
        mesh.tangents_[offset + 0]   = tangent1.x;
        mesh.tangents_[offset + 1]   = tangent1.y;
        mesh.tangents_[offset + 2]   = tangent1.z;
        mesh.bitangents_[offset + 0] = bitangent1.x;
        mesh.bitangents_[offset + 1] = bitangent1.y;
        mesh.bitangents_[offset + 2] = bitangent1.z;
    }
    // --- Bones i joinIds
    size_t vertCount = mesh.positions_.size() / 3;
    for (size_t i = 0; i < vertCount; i++)
    {
        for (int k = 0; k < 4; k++)
            mesh.bonesWeights_.push_back(0.0f);
        for (int k = 0; k < 4; k++)
            mesh.joinIds_.push_back(-1);
    }

    return mesh;
}

GraphicsApi::MeshRawData GeneratePlane(float size)
{
    GraphicsApi::MeshRawData mesh;
    float half = size * 0.5f;

    glm::vec3 vertices[4] = {{-half, 0, -half}, {half, 0, -half}, {half, 0, half}, {-half, 0, half}};

    unsigned int indices[6] = {0, 2, 1, 0, 3, 2};

    for (int i = 0; i < 4; i++)
    {
        mesh.positions_.push_back(vertices[i].x);
        mesh.positions_.push_back(vertices[i].y);
        mesh.positions_.push_back(vertices[i].z);

        mesh.normals_.push_back(0);
        mesh.normals_.push_back(1);
        mesh.normals_.push_back(0);

        mesh.tangents_.push_back(1);
        mesh.tangents_.push_back(0);
        mesh.tangents_.push_back(0);

        mesh.bitangents_.push_back(0);
        mesh.bitangents_.push_back(0);
        mesh.bitangents_.push_back(1);

        mesh.textCoords_.push_back((vertices[i].x + half) / size);
        mesh.textCoords_.push_back((vertices[i].z + half) / size);

        for (int k = 0; k < 4; k++)
            mesh.bonesWeights_.push_back(0.0f);
        for (int k = 0; k < 4; k++)
            mesh.joinIds_.push_back(-1);
    }

    mesh.indices_.insert(mesh.indices_.end(), std::begin(indices), std::end(indices));
    return mesh;
}

GraphicsApi::MeshRawData GenerateTorus(float radius, float tubeRadius, unsigned int radialSegs, unsigned int tubularSegs)
{
    GraphicsApi::MeshRawData mesh;

    // Tworzymy wierzcholki i normalne
    for (unsigned int i = 0; i <= radialSegs; ++i)
    {
        float u = i * 2.0f * glm::pi<float>() / radialSegs;
        float cosU = cos(u), sinU = sin(u);

        for (unsigned int j = 0; j <= tubularSegs; ++j)
        {
            float v = j * 2.0f * glm::pi<float>() / tubularSegs;
            float cosV = cos(v), sinV = sin(v);

            glm::vec3 pos((radius + tubeRadius * cosV) * cosU, tubeRadius * sinV, (radius + tubeRadius * cosV) * sinU);
            glm::vec3 center(radius * cosU, 0.0f, radius * sinU);
            glm::vec3 normal = glm::normalize(pos - center);

            mesh.positions_.push_back(pos.x);
            mesh.positions_.push_back(pos.y);
            mesh.positions_.push_back(pos.z);

            mesh.normals_.push_back(normal.x);
            mesh.normals_.push_back(normal.y);
            mesh.normals_.push_back(normal.z);

            glm::vec3 tangent = glm::normalize(glm::vec3(-sinU, 0.0f, cosU));
            glm::vec3 bitangent = glm::normalize(glm::cross(normal, tangent));

            mesh.tangents_.push_back(tangent.x);
            mesh.tangents_.push_back(tangent.y);
            mesh.tangents_.push_back(tangent.z);

            mesh.bitangents_.push_back(bitangent.x);
            mesh.bitangents_.push_back(bitangent.y);
            mesh.bitangents_.push_back(bitangent.z);

            mesh.textCoords_.push_back(float(i) / radialSegs);
            mesh.textCoords_.push_back(float(j) / tubularSegs);

            for (int b = 0; b < 4; ++b) mesh.bonesWeights_.push_back(0.0f);
            for (int b = 0; b < 4; ++b) mesh.joinIds_.push_back(-1);
        }
    }

    // Tworzymy indeksy (CCW) - ostroznie przy ostatnim segmencie
    for (unsigned int i = 0; i < radialSegs; ++i)
    {
        unsigned int nextI = (i + 1) % radialSegs; // ostatni segment laczy sie z pierwszym
        for (unsigned int j = 0; j < tubularSegs; ++j)
        {
            unsigned int a = i * (tubularSegs + 1) + j;
            unsigned int b = nextI * (tubularSegs + 1) + j;
            unsigned int c = b + 1;
            unsigned int d = a + 1;

            // Dwa trojkaty quada w kolejnosci CCW patrzac z zewnatrz
            mesh.indices_.push_back(a);
            mesh.indices_.push_back(c);
            mesh.indices_.push_back(b);

            mesh.indices_.push_back(a);
            mesh.indices_.push_back(d);
            mesh.indices_.push_back(c);
        }
    }

    return mesh;
}


GraphicsApi::MeshRawData GeneratePyramid(float size, float height)
{
    GraphicsApi::MeshRawData mesh;
    float half = size * 0.5f;

    // --- PODSTAWA ---
    glm::vec3 baseVertices[4] = {
        {-half, 0, -half},  // 0
        {half, 0, -half},   // 1
        {half, 0, half},    // 2
        {-half, 0, half}    // 3
    };

    // CCW patrzac od dolu
    unsigned int baseIndices[6] = {0, 1, 2, 2, 3, 0};

    for (int i = 0; i < 4; i++)
    {
        glm::vec3 pos = baseVertices[i];
        mesh.positions_.push_back(pos.x);
        mesh.positions_.push_back(pos.y);
        mesh.positions_.push_back(pos.z);

        // normala podstawy
        mesh.normals_.push_back(0.0f);
        mesh.normals_.push_back(-1.0f);
        mesh.normals_.push_back(0.0f);

        mesh.tangents_.push_back(1.0f);
        mesh.tangents_.push_back(0.0f);
        mesh.tangents_.push_back(0.0f);
        mesh.bitangents_.push_back(0.0f);
        mesh.bitangents_.push_back(0.0f);
        mesh.bitangents_.push_back(1.0f);

        mesh.textCoords_.push_back((pos.x + half) / size);
        mesh.textCoords_.push_back((pos.z + half) / size);

        for (int k = 0; k < 4; k++)
            mesh.bonesWeights_.push_back(0.0f);
        for (int k = 0; k < 4; k++)
            mesh.joinIds_.push_back(-1);
    }

    for (int i = 0; i < 6; i++)
        mesh.indices_.push_back(baseIndices[i]);

    // --- SCIANY BOCZNE ---
    glm::vec3 apex(0, height, 0);
    unsigned int sideStartIndex = mesh.positions_.size() / 3;

    for (int i = 0; i < 4; i++)
    {
        glm::vec3 v0 = baseVertices[i];
        glm::vec3 v2 = baseVertices[(i + 1) % 4];
        glm::vec3 v1 = apex;  // CCW: dolny lewy, wierzcholek, dolny prawy

        glm::vec3 normal    = glm::normalize(glm::cross(v1 - v0, v2 - v0));
        glm::vec3 tangent   = glm::normalize(v2 - v0);
        glm::vec3 bitangent = glm::normalize(glm::cross(normal, tangent));

        glm::vec3 verts[3] = {v0, v1, v2};
        for (int j = 0; j < 3; j++)
        {
            glm::vec3 pos = verts[j];
            mesh.positions_.push_back(pos.x);
            mesh.positions_.push_back(pos.y);
            mesh.positions_.push_back(pos.z);

            mesh.normals_.push_back(normal.x);
            mesh.normals_.push_back(normal.y);
            mesh.normals_.push_back(normal.z);

            mesh.tangents_.push_back(tangent.x);
            mesh.tangents_.push_back(tangent.y);
            mesh.tangents_.push_back(tangent.z);

            mesh.bitangents_.push_back(bitangent.x);
            mesh.bitangents_.push_back(bitangent.y);
            mesh.bitangents_.push_back(bitangent.z);

            mesh.textCoords_.push_back((pos.x + half) / size);
            mesh.textCoords_.push_back(pos.y / height);

            for (int k = 0; k < 4; k++)
                mesh.bonesWeights_.push_back(0.0f);
            for (int k = 0; k < 4; k++)
                mesh.joinIds_.push_back(-1);
        }

        mesh.indices_.push_back(sideStartIndex + i * 3 + 0);
        mesh.indices_.push_back(sideStartIndex + i * 3 + 1);
        mesh.indices_.push_back(sideStartIndex + i * 3 + 2);
    }

    return mesh;
}

GraphicsApi::MeshRawData GenerateIcoSphere(float radius)
{
    return GenerateSphere(radius, 3, 3);
}

GraphicsApi::MeshRawData GenerateTriangle(float size)
{
    GraphicsApi::MeshRawData mesh;
    float half = size * 0.5f;

    // Wierzcholki trojkata (XY plane)
    glm::vec3 vertices[3] = {{-half, -half, 0.0f}, {half, -half, 0.0f}, {0.0f, half, 0.0f}};

    // Indeksy
    unsigned int indices[3] = {0, 1, 2};

    for (int i = 0; i < 3; ++i)
    {
        mesh.positions_.push_back(vertices[i].x);
        mesh.positions_.push_back(vertices[i].y);
        mesh.positions_.push_back(vertices[i].z);

        mesh.normals_.push_back(0.0f);
        mesh.normals_.push_back(0.0f);
        mesh.normals_.push_back(1.0f);

        mesh.tangents_.push_back(1.0f);
        mesh.tangents_.push_back(0.0f);
        mesh.tangents_.push_back(0.0f);
        mesh.bitangents_.push_back(0.0f);
        mesh.bitangents_.push_back(1.0f);
        mesh.bitangents_.push_back(0.0f);

        mesh.textCoords_.push_back((vertices[i].x + half) / size);
        mesh.textCoords_.push_back((vertices[i].y + half) / size);

        for (int k = 0; k < 4; k++)
            mesh.bonesWeights_.push_back(0.0f);
        for (int k = 0; k < 4; k++)
            mesh.joinIds_.push_back(-1);
    }

    mesh.indices_.insert(mesh.indices_.end(), std::begin(indices), std::end(indices));
    return mesh;
}
}  // namespace GameEngine