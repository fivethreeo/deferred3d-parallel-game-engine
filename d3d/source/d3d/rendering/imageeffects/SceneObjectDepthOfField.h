#pragma once

#include <d3d/scene/RenderScene.h>
#include <d3d/rendering/imageeffects/SceneObjectEffectBuffer.h>

namespace d3d {
	class SceneObjectDepthOfField : public SceneObject {
	private:
		d3d::SceneObjectRef _effectBuffer;

		std::shared_ptr<Shader> _depthOfFieldBlurShaderHorizontal;
		std::shared_ptr<Shader> _depthOfFieldBlurShaderVertical;

		std::shared_ptr<Shader> _renderImageShader;

	public:
		size_t _numBlurPasses;
		float _blurRadius;

		float _focalDistance;
		float _focalRange;

		SceneObjectDepthOfField()
			: _focalDistance(8.0f), _focalRange(2.0f),
			_numBlurPasses(3), _blurRadius(0.00115f)
		{}

		void create(const std::shared_ptr<Shader> &depthOfFieldBlurShaderHorizontal,
			const std::shared_ptr<Shader> &depthOfFieldBlurShaderVertical,
			const std::shared_ptr<Shader> &renderImageShader);

		// Inherited from SceneObject
		void onAdd();
		void postRender();

		SceneObjectDepthOfField* copyFactory() {
			return new SceneObjectDepthOfField(*this);
		}
	};
}