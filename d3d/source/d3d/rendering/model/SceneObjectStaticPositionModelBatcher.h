#pragma once

#include <d3d/scene/RenderScene.h>
#include <d3d/rendering/model/StaticPositionModel.h>

namespace d3d {
	class SceneObjectStaticPositionModelBatcher : public SceneObject {
	private:
		std::unordered_map<StaticPositionModel*, std::list<Matrix4x4f>> _modelTransforms;

	public:
		SceneObjectStaticPositionModelBatcher() {
			_renderMask = 0xffff;
		}

		// Inherited from SceneObject
		void batchRender();

		SceneObjectStaticPositionModelBatcher* copyFactory() {
			return new SceneObjectStaticPositionModelBatcher(*this);
		}

		friend class StaticModel;
	};
}