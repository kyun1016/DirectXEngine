#pragma once

#include <directxtk/SimpleMath.h>
#include "MeshData.h"

namespace kyun {

using namespace DirectX::SimpleMath;

class GeometryGenerator
{
public:
	static MeshData MakeSquare(const float scale = 1.0f, const Vector2 texScale = Vector2(1.0f));
	static MeshData MakeBox(const float& scale = 1.0f);
	
};
}


