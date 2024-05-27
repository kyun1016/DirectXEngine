#include "GeometryGenerator.h"

namespace kyun {

using namespace DirectX::SimpleMath;

MeshData GeometryGenerator::MakeSquare(const float scale, const Vector2 texScale)
{
    std::vector<Vector3> positions;
    std::vector<Vector3> colors;
    std::vector<Vector3> normals;
    std::vector<Vector2> texcoords; // ÅØ½ºÃç ÁÂÇ¥

    // ¾Õ¸é
    positions.push_back(Vector3(-1.0f, 1.0f, 0.0f));
    positions.push_back(Vector3(1.0f, 1.0f, 0.0f));
    positions.push_back(Vector3(1.0f, -1.0f, 0.0f));
    positions.push_back(Vector3(-1.0f, -1.0f, 0.0f));
    colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
    colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
    colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
    colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
    texcoords.push_back(Vector2(0.0f, 0.0f));
    texcoords.push_back(Vector2(1.0f, 0.0f));
    texcoords.push_back(Vector2(1.0f, 1.0f));
    texcoords.push_back(Vector2(0.0f, 1.0f));

    MeshData meshData;

    for (size_t i = 0; i < positions.size(); ++i) {
        Vertex v;
        v.position = positions[i] * scale;
        v.color = colors[i];
        v.normalModel = normals[i];
        v.texcoord = texcoords[i] * texScale;
        v.tangentModel = Vector3(1.0f, 0.0f, 0.0f);

        meshData.vertices.push_back(v);
    }
    meshData.indices = {
        0, 1, 2, 0, 2, 3, // ¾Õ¸é
    };

    return meshData;
}

MeshData GeometryGenerator::MakeBox(const float& scale)
{
    std::vector<Vector3> positions;
    std::vector<Vector3> colors;
    std::vector<Vector3> normals;
    std::vector<Vector2> texcoords; // ÅØ½ºÃç ÁÂÇ¥

    // À­¸é
    positions.push_back(Vector3(-1.0f, 1.0f, 0.0f));
    positions.push_back(Vector3(1.0f, 1.0f, 0.0f));
    positions.push_back(Vector3(1.0f, -1.0f, 0.0f));
    positions.push_back(Vector3(-1.0f, -1.0f, 0.0f));
    colors.push_back(Vector3(1.0f, 0.0f, 0.0f));
    colors.push_back(Vector3(1.0f, 0.0f, 0.0f));
    colors.push_back(Vector3(1.0f, 0.0f, 0.0f));
    colors.push_back(Vector3(1.0f, 0.0f, 0.0f));
    normals.push_back(Vector3(0.0f, 1.0f, 0.0f));
    normals.push_back(Vector3(0.0f, 1.0f, 0.0f));
    normals.push_back(Vector3(0.0f, 1.0f, 0.0f));
    normals.push_back(Vector3(0.0f, 1.0f, 0.0f));
    texcoords.push_back(Vector2(0.0f, 0.0f));
    texcoords.push_back(Vector2(1.0f, 0.0f));
    texcoords.push_back(Vector2(1.0f, 1.0f));
    texcoords.push_back(Vector2(0.0f, 1.0f));

    // ¾Æ·§¸é

    // ¾Õ¸é

    // µÞ¸é

    // ¿ÞÂÊ

    // ¿À¸¥ÂÊ

    return MeshData();
}
} // namespace kyun

