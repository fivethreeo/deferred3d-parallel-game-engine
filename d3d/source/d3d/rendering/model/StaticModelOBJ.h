#pragma once

#include <d3d/assetmanager/Asset.h>
#include <d3d/rendering/model/StaticModel.h>

namespace d3d {
	class StaticModelOBJ : public Asset {
	private:
		AABB3D _aabb;

	public:
		AssetManager _textureManager;

		StaticModel _model;

		// Inherited from Asset
		bool createAsset(const std::string &name);

		void render(RenderScene* pScene) {
			_model.render(pScene);
		}

		void render(SceneObjectStaticModelBatcher* pBatcher, const Matrix4x4f &transform) {
			_model.render(pBatcher, transform);
		}

		const AABB3D &getAABB() const {
			return _aabb;
		}

		// Asset factory
		static Asset* assetFactory() {
			return new StaticModelOBJ();
		}
	};
}