#pragma once

#include <d3d/rendering/mesh/StaticMesh.h>
#include <d3d/rendering/material/Material.h>

#include <d3d/scene/RenderScene.h>

// Uncomment line below to flip texture coordinates across Y
#define D3D_OBJ_FLIP_TEXCOORD_Y

namespace d3d {
	struct StaticMeshIndexSet {
		staticMeshIndexType _vi, _ti, _ni;

		// Custom hash stuff - if this index set already exists, must have a vertex for it
		size_t operator()(const StaticMeshIndexSet &set) const {
			return static_cast<size_t>(set._vi ^ set._ti ^ set._ni);
		}

		bool operator==(const StaticMeshIndexSet &other) const {
			return _vi == other._vi && _ti == other._ti && _ni == other._ni;
		}
	};

	class StaticModel {
	public:
		struct StaticMeshAndMaterialIndex {
			std::shared_ptr<StaticMesh> _mesh;
			size_t _materialIndex;

			StaticMeshAndMaterialIndex() {}
			StaticMeshAndMaterialIndex(const std::shared_ptr<StaticMesh> &mesh, size_t materialIndex)
				: _mesh(mesh), _materialIndex(materialIndex)
			{}
		};

		std::vector<StaticMeshAndMaterialIndex> _meshes;
		std::vector<Material> _materials;

		void render(RenderScene* pScene);
		void render(class SceneObjectStaticModelBatcher* pBatcher, const Matrix4x4f &transform);

		void genMipMaps();

		// Loaders
		bool loadFromOBJ(const std::string &fileName, AssetManager &textureManager, AABB3D &aabb, bool useBuffers, bool clearArrays, RenderScene* pRenderScene = nullptr);
	};
}