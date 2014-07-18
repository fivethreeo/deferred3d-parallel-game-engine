#pragma once

#include <d3d/scene/RenderScene.h>
#include <d3d/rendering/imageeffects/SceneObjectEffectBuffer.h>

namespace d3d {
	class SceneObjectSSAO : public SceneObject {
	private:
		d3d::SceneObjectRef _effectBuffer;

		std::shared_ptr<Shader> _blurShaderHorizontal;
		std::shared_ptr<Shader> _blurShaderVertical;

		std::shared_ptr<Shader> _ssaoShader;

		std::shared_ptr<Shader> _renderImageShader;

		std::shared_ptr<Texture2D> _noiseMap;

	public:
		size_t _numBlurPasses;
		float _blurRadius;

		float _ssaoRadius;
		float _ssaoStrength;

		SceneObjectSSAO()
			: _numBlurPasses(2), _blurRadius(0.00493f), _ssaoRadius(0.2f), _ssaoStrength(3.0f)
		{}

		void create(const std::shared_ptr<Shader> &blurShaderHorizontal,
			const std::shared_ptr<Shader> &blurShaderVertical,
			const std::shared_ptr<Shader> &ssaoShader,
			const std::shared_ptr<Shader> &renderImageShader,
			const std::shared_ptr<Texture2D> &noiseMap);

		// Inherited from SceneObject
		void onAdd();
		void postRender();

		SceneObjectSSAO* copyFactory() {
			return new SceneObjectSSAO(*this);
		}
	};
}