#pragma once

#include <d3d/scene/SceneObject.h>
#include <d3d/rendering/mesh/StaticMesh.h>

#include <d3d/rendering/voxel/TerrainGrass.h>

namespace d3d {
	class TerrainGrassBatcher : public SceneObject {
	private:
		std::list<SceneObjectRef> _grassObjects;

		std::shared_ptr<Shader> _grassRenderShader;

		std::shared_ptr<Shader> _depthRenderShader;

		std::shared_ptr<Texture2D> _grassTileSetDiffuse;
		std::shared_ptr<Texture2D> _grassTileSetNormal;
		std::shared_ptr<Texture2D> _noiseMap;

		float _waveTimer;

	public:
		float _completelyVisibleDistance;
		float _completelyInvisibleDistance;

		float _waveRate;

		bool _renderShadows;

		TerrainGrassBatcher();

		void create(const std::shared_ptr<Texture2D> &grassTileSetDiffuse, const std::shared_ptr<Texture2D> &grassTileSetNormal, const std::shared_ptr<Texture2D> &noiseMap, const std::shared_ptr<Shader> &grassRenderShader, const std::shared_ptr<Shader> &depthRenderShader);

		// Inherited from SceneObject
		void update(float dt);
		void batchRender();

		SceneObject* copyFactory() {
			return new TerrainGrassBatcher(*this);
		}

		friend TerrainGrass;
	};
}