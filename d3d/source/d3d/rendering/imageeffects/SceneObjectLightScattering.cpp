#include <d3d/rendering/imageeffects/SceneObjectLightScattering.h>

using namespace d3d;

void SceneObjectLightScattering::create(const std::shared_ptr<Shader> &blurShaderHorizontal,
	const std::shared_ptr<Shader> &blurShaderVertical,
	const std::shared_ptr<Shader> &lightScatteringShader,
	const std::shared_ptr<Shader> &renderImageShader)
{
	_renderMask = 0xffff;

	_blurShaderHorizontal = blurShaderHorizontal;
	_blurShaderVertical = blurShaderVertical;
	_lightScatteringShader = lightScatteringShader;
	_renderImageShader = renderImageShader;

	_lightScatteringShader->bind();

	_lightScatteringShader->setShaderTexture("d3dGBufferPosition", getRenderScene()->_gBuffer.getTextureID(GBuffer::_positionAndEmissive), GL_TEXTURE_2D);
}

void SceneObjectLightScattering::onAdd() {
	_effectBuffer = getScene()->getNamed("ebuf");

	assert(_effectBuffer.isAlive());
}

void SceneObjectLightScattering::postRender() {
	SceneObjectEffectBuffer* pEffectBuffer = static_cast<SceneObjectEffectBuffer*>(_effectBuffer.get());

	Vec2f sizeInv(1.0f / static_cast<float>(pEffectBuffer->_ping->getWidth()), 1.0f / static_cast<float>(pEffectBuffer->_ping->getHeight()));

	glDisable(GL_DEPTH_TEST);

	_lightScatteringShader->bind();

	_lightScatteringShader->setUniformv2f("d3dSizeInv", sizeInv);

	_lightScatteringShader->setUniformv3f("d3dColor", _lightSourceColor * std::max(0.0f, (getScene()->_logicCamera._rotation * Vec3f(0.0f, 0.0f, -1.0f)).dot((_lightSourcePosition - getScene()->_logicCamera._position).normalized())));

	_lightScatteringShader->setUniformv4f("d3dLightProjectedPosition", getScene()->_logicCamera.getProjectionViewMatrix() * Vec4f(_lightSourcePosition.x, _lightSourcePosition.y, _lightSourcePosition.z, 1.0f));
	_lightScatteringShader->setUniformv3f("d3dLightViewPosition", getScene()->_logicCamera.getViewMatrix() * _lightSourcePosition);

	_lightScatteringShader->bindShaderTextures();

	pEffectBuffer->_ping->bind();
	pEffectBuffer->_ping->setViewport();

	getRenderScene()->renderNormalizedQuad();

	for (size_t i = 0; i < _numBlurPasses; i++) {
		_blurShaderHorizontal->bind();

		_blurShaderHorizontal->setShaderTexture("d3dScene", pEffectBuffer->_ping->getTextureID(), GL_TEXTURE_2D);
		_blurShaderHorizontal->setUniformf("d3dBlurSize", _blurRadius);
		_blurShaderHorizontal->setUniformv2f("d3dSizeInv", sizeInv);

		_blurShaderHorizontal->bindShaderTextures();

		pEffectBuffer->_pong->bind();
		pEffectBuffer->_pong->setViewport();

		getRenderScene()->renderNormalizedQuad();

		_blurShaderVertical->bind();

		_blurShaderVertical->setShaderTexture("d3dScene", pEffectBuffer->_pong->getTextureID(), GL_TEXTURE_2D);
		_blurShaderVertical->setUniformf("d3dBlurSize", _blurRadius);
		_blurShaderVertical->setUniformv2f("d3dSizeInv", sizeInv);

		_blurShaderVertical->bindShaderTextures();

		pEffectBuffer->_ping->bind();
		pEffectBuffer->_ping->setViewport();

		getRenderScene()->renderNormalizedQuad();
	}

	Vec2f gBufferSizeInv(1.0f / static_cast<float>(getRenderScene()->_gBuffer.getWidth()), 1.0f / static_cast<float>(getRenderScene()->_gBuffer.getHeight()));

	// Blend with effect buffer
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);

	_renderImageShader->bind();

	_renderImageShader->setShaderTexture("d3dScene", pEffectBuffer->_ping->getTextureID(), GL_TEXTURE_2D);

	_renderImageShader->setUniformv2f("d3dGBufferSizeInv", gBufferSizeInv);

	_renderImageShader->bindShaderTextures();

	getRenderScene()->_gBuffer.bind();
	getRenderScene()->_gBuffer.setDrawEffect();
	getRenderScene()->_gBuffer.setReadEffect();

	getRenderScene()->_gBuffer.setViewport();

	getRenderScene()->renderNormalizedQuad();

	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
}