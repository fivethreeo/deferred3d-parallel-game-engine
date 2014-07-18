#pragma once

#include <d3d/scene/RenderScene.h>
#include <d3d/rendering/model/StaticModel.h>

namespace d3d {
	class SceneObjectStaticModelBatcher : public SceneObject {
	private:
		std::unordered_map<StaticModel*, std::list<Matrix4x4f>> _modelTransforms;

	public:
		SceneObjectStaticModelBatcher() {
			_renderMask = 0xffff;
		}

		// Inherited from SceneObject
		void batchRender();

		SceneObjectStaticModelBatcher* copyFactory() {
			return new SceneObjectStaticModelBatcher(*this);
		}

		friend class StaticModel;
	};
}