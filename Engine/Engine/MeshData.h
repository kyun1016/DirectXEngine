#pragma once

#include <vector>
#include <string>

#include "Vertex.h"

namespace kyun {
struct MeshData {
	std::vector<Vertex> vertices;
	std::vector<SkinnedVertex> skinnedVertices;
	std::vector<uint32_t> indices;
	std::string albedoTextureFilename;
	std::string emissiveTextureFilename;
	std::string normalTextureFilename;
	std::string heightTextureFilename;
	std::string aoTextureFilename;
	std::string metallicTextureFilename;
	std::string roughnessTextureFilename;
	std::string opacityTextureFilename;
};
} // namespace kyun