#pragma once

#include <d3d/scene/RenderScene.h>

namespace d3d {
	class SceneObjectEffectBuffer : public SceneObject {
	private:
	public:
		std::shared_ptr<d3d::TextureRT> _ping;
		std::shared_ptr<d3d::TextureRT> _pong;

		std::shared_ptr<d3d::TextureRT> _fullPing;
		std::shared_ptr<d3d::TextureRT> _fullPong;

		void create(float downsampleRatio);

		SceneObjectEffectBuffer* copyFactory() {
			return new SceneObjectEffectBuffer(*this);
		}
	};
}