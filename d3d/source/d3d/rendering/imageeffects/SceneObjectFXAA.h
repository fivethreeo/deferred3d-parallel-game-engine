#pragma once

#include <d3d/scene/RenderScene.h>
#include <d3d/rendering/imageeffects/SceneObjectEffectBuffer.h>

namespace d3d {
	class SceneObjectFXAA : public SceneObject {
	private:
		std::shared_ptr<Shader> _fxaaShader;
		std::shared_ptr<Shader> _lumaShader;

		std::shared_ptr<TextureRT> _lumaBuffer;

	public:
		void create(const std::shared_ptr<Shader> &fxaaShader, const std::shared_ptr<Shader> &lumaShader);

		// Inherited from SceneObject
		void postRender();

		SceneObjectFXAA* copyFactory() {
			return new SceneObjectFXAA(*this);
		}
	};
}