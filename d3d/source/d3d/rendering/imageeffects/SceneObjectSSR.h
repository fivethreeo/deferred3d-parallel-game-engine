#pragma once

#include <d3d/scene/RenderScene.h>
#include <d3d/rendering/imageeffects/SceneObjectEffectBuffer.h>
#include <d3d/rendering/texture/TextureCube.h>

namespace d3d {
	class SceneObjectSSR : public SceneObject {
	private:
		d3d::SceneObjectRef _effectBuffer;

		std::shared_ptr<Shader> _blurShaderHorizontalEdgeAware;
		std::shared_ptr<Shader> _blurShaderVerticalEdgeAware;

		std::shared_ptr<Shader> _ssrShader;

		std::shared_ptr<Shader> _renderImageShader;

		std::shared_ptr<TextureCube> _cubeMap;

		std::shared_ptr<Texture2D> _noiseMap;

	public:
		size_t _numBlurPasses;
		float _blurRadius;

		float _reflectionRadius;

		SceneObjectSSR()
			: _numBlurPasses(1), _blurRadius(0.001327f)
		{}

		void create(const std::shared_ptr<Shader> &blurShaderHorizontalEdgeAware,
			const std::shared_ptr<Shader> &blurShaderVerticalEdgeAware,
			const std::shared_ptr<Shader> &ssrShader,
			const std::shared_ptr<Shader> &renderImageShader,
			const std::shared_ptr<TextureCube> &cubeMap,
			const std::shared_ptr<Texture2D> &noiseMap);

		// Inherited from SceneObject
		void onAdd();
		void postRender();

		SceneObjectSSR* copyFactory() {
			return new SceneObjectSSR(*this);
		}
	};
}