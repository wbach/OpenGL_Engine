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

            // pozycje
            float px = radius * sin(theta) * cos(phi);
            float py = radius * cos(theta);
            float pz = radius * sin(theta) * sin(phi);
            mesh.positions_.push_back(px);
            mesh.positions_.push_back(py);
            mesh.positions_.push_back(pz);

            // normalne
            glm::vec3 normal = glm::normalize(glm::vec3(px, py, pz));
            mesh.normals_.push_back(normal.x);
            mesh.normals_.push_back(normal.y);
            mesh.normals_.push_back(normal.z);

            // tangenty i bitangenty (przybliżone)
            glm::vec3 tangent(-sin(phi), 0.0f, cos(phi));
            glm::vec3 bitangent = glm::cross(normal, tangent);
            mesh.tangents_.push_back(tangent.x);
            mesh.tangents_.push_back(tangent.y);
            mesh.tangents_.push_back(tangent.z);

            mesh.bitangents_.push_back(bitangent.x);
            mesh.bitangents_.push_back(bitangent.y);
            mesh.bitangents_.push_back(bitangent.z);

            // współrzędne tekstury
            mesh.textCoords_.push_back(u);
            mesh.textCoords_.push_back(v);

            // wagi kości i ID (pusta kula)
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

    // indeksy
    for (unsigned int y = 0; y < latSegments; ++y)
    {
        for (unsigned int x = 0; x < lonSegments; ++x)
        {
            unsigned int i0 = y * (lonSegments + 1) + x;
            unsigned int i1 = i0 + 1;
            unsigned int i2 = i0 + (lonSegments + 1);
            unsigned int i3 = i2 + 1;

            mesh.indices_.push_back(i0);
            mesh.indices_.push_back(i2);
            mesh.indices_.push_back(i1);

            mesh.indices_.push_back(i1);
            mesh.indices_.push_back(i2);
            mesh.indices_.push_back(i3);
        }
    }

    return mesh;
}

GraphicsApi::MeshRawData GenerateCube(float size)
{
    GraphicsApi::MeshRawData mesh;
    float half = size * 0.5f;

    // Wierzchołki
    glm::vec3 vertices[8] = {
        {-half, -half, -half}, {half, -half, -half}, {half, half, -half}, {-half, half, -half},
        {-half, -half, half},  {half, -half, half},  {half, half, half},  {-half, half, half},
    };

    // Indeksy (2 trójkąty na każdą ścianę)
    unsigned int indices[36] = {
        0, 1, 2, 2, 3, 0,  // tył
        4, 5, 6, 6, 7, 4,  // przód
        0, 4, 7, 7, 3, 0,  // lewa
        1, 5, 6, 6, 2, 1,  // prawa
        3, 2, 6, 6, 7, 3,  // góra
        0, 1, 5, 5, 4, 0   // dół
    };

    for (int i = 0; i < 8; ++i)
    {
        mesh.positions_.push_back(vertices[i].x);
        mesh.positions_.push_back(vertices[i].y);
        mesh.positions_.push_back(vertices[i].z);

        // normalne przybliżone w kierunku wierzchołka
        glm::vec3 n = glm::normalize(vertices[i]);
        mesh.normals_.push_back(n.x);
        mesh.normals_.push_back(n.y);
        mesh.normals_.push_back(n.z);

        // tangenty i bitangenty
        glm::vec3 tangent(1, 0, 0);
        glm::vec3 bitangent(0, 1, 0);
        mesh.tangents_.push_back(tangent.x);
        mesh.tangents_.push_back(tangent.y);
        mesh.tangents_.push_back(tangent.z);

        mesh.bitangents_.push_back(bitangent.x);
        mesh.bitangents_.push_back(bitangent.y);
        mesh.bitangents_.push_back(bitangent.z);

        // texcoords proste 0/1
        mesh.textCoords_.push_back(vertices[i].x + half);
        mesh.textCoords_.push_back(vertices[i].y + half);

        // puste kości
        mesh.bonesWeights_.push_back(0.0f);
        mesh.bonesWeights_.push_back(0.0f);
        mesh.bonesWeights_.push_back(0.0f);
        mesh.bonesWeights_.push_back(0.0f);
        mesh.joinIds_.push_back(-1);
        mesh.joinIds_.push_back(-1);
        mesh.joinIds_.push_back(-1);
        mesh.joinIds_.push_back(-1);
    }

    mesh.indices_.insert(mesh.indices_.end(), std::begin(indices), std::end(indices));

    return mesh;
}

GraphicsApi::MeshRawData GenerateCylinder(float radius, float height, unsigned int segments)
{
    GraphicsApi::MeshRawData mesh;
    float half = height * 0.5f;

    // wierzchołki boczne
    for (unsigned int i = 0; i <= segments; ++i)
    {
        float theta = i * 2.0f * glm::pi<float>() / segments;
        float x     = radius * cos(theta);
        float z     = radius * sin(theta);

        // dolna krawędź
        mesh.positions_.push_back(x);
        mesh.positions_.push_back(-half);
        mesh.positions_.push_back(z);

        // górna krawędź
        mesh.positions_.push_back(x);
        mesh.positions_.push_back(half);
        mesh.positions_.push_back(z);

        // normalne boczne
        glm::vec3 n = glm::normalize(glm::vec3(x, 0.0f, z));
        mesh.normals_.push_back(n.x);
        mesh.normals_.push_back(n.y);
        mesh.normals_.push_back(n.z);
        mesh.normals_.push_back(n.x);
        mesh.normals_.push_back(n.y);
        mesh.normals_.push_back(n.z);

        // texcoords proste
        mesh.textCoords_.push_back(float(i) / segments);
        mesh.textCoords_.push_back(0.0f);
        mesh.textCoords_.push_back(float(i) / segments);
        mesh.textCoords_.push_back(1.0f);

        // tangenty/bitangenty
        mesh.tangents_.push_back(-sin(theta));
        mesh.tangents_.push_back(0);
        mesh.tangents_.push_back(cos(theta));
        mesh.tangents_.push_back(-sin(theta));
        mesh.tangents_.push_back(0);
        mesh.tangents_.push_back(cos(theta));

        mesh.bitangents_.push_back(0);
        mesh.bitangents_.push_back(1);
        mesh.bitangents_.push_back(0);
        mesh.bitangents_.push_back(0);
        mesh.bitangents_.push_back(1);
        mesh.bitangents_.push_back(0);

        // puste kości
        for (int k = 0; k < 4; k++)
            mesh.bonesWeights_.push_back(0.0f);
        for (int k = 0; k < 4; k++)
            mesh.joinIds_.push_back(-1);
    }

    // indeksy boczne
    for (unsigned int i = 0; i < segments; ++i)
    {
        unsigned int i0 = i * 2;
        unsigned int i1 = i0 + 1;
        unsigned int i2 = i0 + 2;
        unsigned int i3 = i0 + 3;

        mesh.indices_.push_back(i0);
        mesh.indices_.push_back(i2);
        mesh.indices_.push_back(i1);
        mesh.indices_.push_back(i1);
        mesh.indices_.push_back(i2);
        mesh.indices_.push_back(i3);
    }

    return mesh;
}

GraphicsApi::MeshRawData GenerateCone(float radius, float height, unsigned int segments)
{
    GraphicsApi::MeshRawData mesh;
    float half = height * 0.5f;

    glm::vec3 tip(0, half, 0);

    // dolne wierzchołki
    for (unsigned int i = 0; i < segments; i++)
    {
        float theta = i * 2.0f * glm::pi<float>() / segments;
        float x     = radius * cos(theta);
        float z     = radius * sin(theta);

        // pozycja
        mesh.positions_.push_back(x);
        mesh.positions_.push_back(-half);
        mesh.positions_.push_back(z);
        // normalne przybliżone
        glm::vec3 n = glm::normalize(glm::vec3(x, radius / height, z));
        mesh.normals_.push_back(n.x);
        mesh.normals_.push_back(n.y);
        mesh.normals_.push_back(n.z);
        // texcoords
        mesh.textCoords_.push_back((cos(theta) + 1) * 0.5f);
        mesh.textCoords_.push_back((sin(theta) + 1) * 0.5f);
        // tangenty/bitangenty
        mesh.tangents_.push_back(-sin(theta));
        mesh.tangents_.push_back(0);
        mesh.tangents_.push_back(cos(theta));
        mesh.bitangents_.push_back(0);
        mesh.bitangents_.push_back(1);
        mesh.bitangents_.push_back(0);
        for (int k = 0; k < 4; k++)
            mesh.bonesWeights_.push_back(0.0f);
        for (int k = 0; k < 4; k++)
            mesh.joinIds_.push_back(-1);
    }

    // wierzchołek stożka
    mesh.positions_.push_back(tip.x);
    mesh.positions_.push_back(tip.y);
    mesh.positions_.push_back(tip.z);
    mesh.normals_.push_back(0);
    mesh.normals_.push_back(1);
    mesh.normals_.push_back(0);
    mesh.textCoords_.push_back(0.5f);
    mesh.textCoords_.push_back(0.5f);
    mesh.tangents_.push_back(1);
    mesh.tangents_.push_back(0);
    mesh.tangents_.push_back(0);
    mesh.bitangents_.push_back(0);
    mesh.bitangents_.push_back(0);
    mesh.bitangents_.push_back(1);
    for (int k = 0; k < 4; k++)
        mesh.bonesWeights_.push_back(0.0f);
    for (int k = 0; k < 4; k++)
        mesh.joinIds_.push_back(-1);

    unsigned int tipIndex = (unsigned int)mesh.positions_.size() / 3 - 1;

    // indeksy
    for (unsigned int i = 0; i < segments; i++)
    {
        unsigned int next = (i + 1) % segments;
        mesh.indices_.push_back(i);
        mesh.indices_.push_back(next);
        mesh.indices_.push_back(tipIndex);
    }

    return mesh;
}

GraphicsApi::MeshRawData GeneratePlane(float size)
{
    GraphicsApi::MeshRawData mesh;
    float half = size * 0.5f;

    glm::vec3 vertices[4] = {{-half, 0, -half}, {half, 0, -half}, {half, 0, half}, {-half, 0, half}};

    unsigned int indices[6] = {0, 1, 2, 2, 3, 0};

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
    for (unsigned int i = 0; i <= radialSegs; i++)
    {
        float u = i * 2.0f * glm::pi<float>() / radialSegs;
        glm::vec3 center(radius * cos(u), 0, radius * sin(u));
        for (unsigned int j = 0; j <= tubularSegs; j++)
        {
            float v = j * 2.0f * glm::pi<float>() / tubularSegs;
            float x = (radius + tubeRadius * cos(v)) * cos(u);
            float y = tubeRadius * sin(v);
            float z = (radius + tubeRadius * cos(v)) * sin(u);
            mesh.positions_.push_back(x);
            mesh.positions_.push_back(y);
            mesh.positions_.push_back(z);

            // normalne
            glm::vec3 n = glm::normalize(glm::vec3(x - center.x, y, z - center.z));
            mesh.normals_.push_back(n.x);
            mesh.normals_.push_back(n.y);
            mesh.normals_.push_back(n.z);

            // texcoords
            mesh.textCoords_.push_back(float(i) / radialSegs);
            mesh.textCoords_.push_back(float(j) / tubularSegs);
            mesh.tangents_.push_back(-sin(u));
            mesh.tangents_.push_back(0);
            mesh.tangents_.push_back(cos(u));
            mesh.bitangents_.push_back(0);
            mesh.bitangents_.push_back(1);
            mesh.bitangents_.push_back(0);
            for (int k = 0; k < 4; k++)
                mesh.bonesWeights_.push_back(0.0f);
            for (int k = 0; k < 4; k++)
                mesh.joinIds_.push_back(-1);
        }
    }

    for (unsigned int i = 0; i < radialSegs; i++)
    {
        for (unsigned int j = 0; j < tubularSegs; j++)
        {
            unsigned int a = i * (tubularSegs + 1) + j;
            unsigned int b = (i + 1) * (tubularSegs + 1) + j;
            unsigned int c = b + 1;
            unsigned int d = a + 1;
            mesh.indices_.push_back(a);
            mesh.indices_.push_back(b);
            mesh.indices_.push_back(d);
            mesh.indices_.push_back(d);
            mesh.indices_.push_back(b);
            mesh.indices_.push_back(c);
        }
    }

    return mesh;
}

GraphicsApi::MeshRawData GeneratePyramid(float size, float height)
{
    GraphicsApi::MeshRawData mesh;
    float half = size * 0.5f;

    glm::vec3 vertices[5] = {
        {-half, 0, -half},
        {half, 0, -half},
        {half, 0, half},
        {-half, 0, half},  // podstawa
        {0, height, 0}     // wierzchołek
    };

    unsigned int indices[18] = {
        0, 1, 2, 2, 3, 0,                   // podstawa
        0, 1, 4, 1, 2, 4, 2, 3, 4, 3, 0, 4  // ściany
    };

    for (int i = 0; i < 5; i++)
    {
        mesh.positions_.push_back(vertices[i].x);
        mesh.positions_.push_back(vertices[i].y);
        mesh.positions_.push_back(vertices[i].z);
        glm::vec3 n = glm::normalize(vertices[i]);  // proste normalne
        mesh.normals_.push_back(n.x);
        mesh.normals_.push_back(n.y);
        mesh.normals_.push_back(n.z);
        mesh.tangents_.push_back(1);
        mesh.tangents_.push_back(0);
        mesh.tangents_.push_back(0);
        mesh.bitangents_.push_back(0);
        mesh.bitangents_.push_back(1);
        mesh.bitangents_.push_back(0);
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

GraphicsApi::MeshRawData GenerateIcoSphere(float radius)
{
    return GenerateSphere(radius, 3, 3);
}

GraphicsApi::MeshRawData GenerateTriangle(float size)
{
    GraphicsApi::MeshRawData mesh;
    float half = size * 0.5f;

    // Wierzchołki trójkąta (XY plane)
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