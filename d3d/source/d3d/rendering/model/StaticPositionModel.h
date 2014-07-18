#pragma once

#include <d3d/rendering/mesh/StaticPositionMesh.h>
#include <d3d/rendering/material/Material.h>

#include <d3d/scene/RenderScene.h>

namespace d3d {
	class StaticPositionModel {
	public:
		std::vector<std::shared_ptr<StaticPositionMesh>> _meshes;

		void render();

		// Loaders
		bool loadFromOBJ(const std::string &fileName, AABB3D &aabb, bool useBuffers, bool clearArrays);
	};
}