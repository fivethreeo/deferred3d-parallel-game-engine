#include <d3d/rendering/lighting/SceneObjectLighting.h>

#include <d3d/rendering/lighting/SceneObjectPointLight.h>
#include <d3d/rendering/lighting/SceneObjectPointLightShadowed.h>
#include <d3d/rendering/lighting/SceneObjectSpotLight.h>
#include <d3d/rendering/lighting/SceneObjectDirectionalLight.h>
#include <d3d/rendering/lighting/SceneObjectDirectionalLightShadowed.h>

using namespace d3d;

void SceneObjectLighting::create(const std::shared_ptr<Shader> &ambientLightShader,
	const std::shared_ptr<Shader> &pointLightShader,
	const std::shared_ptr<Shader> &pointLightShadowedShader,
	const std::shared_ptr<Shader> &spotLightShader,
	const std::shared_ptr<Shader> &directionalLightShader,
	const std::shared_ptr<Shader> &directionalLightShadowedShader,
	const std::shared_ptr<Shader> &emissiveRenderShader,
	const std::shared_ptr<Shader> &depthRenderShader,
	const std::shared_ptr<StaticPositionModel> &sphereModel,
	const std::shared_ptr<StaticPositionModel> &coneModel,
	const std::shared_ptr<Texture2D> &noiseMap)
{
	_ambientLightShader = ambientLightShader;
	_pointLightShader = pointLightShader;
	_pointLightShadowedShader = pointLightShadowedShader;
	_spotLightShader = spotLightShader;
	_directionalLightShader = directionalLightShader;
	_directionalLightShadowedShader = directionalLightShadowedShader;
	_emissiveRenderShader = emissiveRenderShader;
	_depthRenderShader = depthRenderShader;
	_noiseMap = noiseMap;

	_sphereModel = sphereModel;
	_coneModel = coneModel;

	Vec2f gBufferSizeInv(1.0f / static_cast<float>(getRenderScene()->_gBuffer.getWidth()), 1.0f / static_cast<float>(getRenderScene()->_gBuffer.getHeight()));

	_ambientLightShader->bind();

	_ambientLightShader->setShaderTexture("d3dScene", getRenderScene()->_gBuffer.getTextureID(GBuffer::_diffuseAndSpecular), GL_TEXTURE_2D);

	_ambientLightShader->setUniformv2f("d3dGBufferSizeInv", gBufferSizeInv);

	_pointLightShader->bind();

	_pointLightShader->setShaderTexture("d3dGBufferPosition", getRenderScene()->_gBuffer.getTextureID(GBuffer::_positionAndEmissive), GL_TEXTURE_2D);
	_pointLightShader->setShaderTexture("d3dGBufferNormal", getRenderScene()->_gBuffer.getTextureID(GBuffer::_normalAndShininess), GL_TEXTURE_2D);
	_pointLightShader->setShaderTexture("d3dGBufferColor", getRenderScene()->_gBuffer.getTextureID(GBuffer::_diffuseAndSpecular), GL_TEXTURE_2D);

	_pointLightShader->setUniformv2f("d3dGBufferSizeInv", gBufferSizeInv);

	const GLchar* pointLightNames[4] = {
		"d3dPointLightPosition",
		"d3dPointLightColor",
		"d3dPointLightRange",
		"d3dPointLightRangeInv"
	};

	_pointLightLightUBOShaderInterface.reset(new UBOShaderInterface());
	_pointLightLightUBOShaderInterface->create("d3dPointLight", _pointLightShader.get(), pointLightNames, 4);

	_pointLightShadowedShader->bind();

	_pointLightShadowedShader->setShaderTexture("d3dGBufferPosition", getRenderScene()->_gBuffer.getTextureID(GBuffer::_positionAndEmissive), GL_TEXTURE_2D);
	_pointLightShadowedShader->setShaderTexture("d3dGBufferNormal", getRenderScene()->_gBuffer.getTextureID(GBuffer::_normalAndShininess), GL_TEXTURE_2D);
	_pointLightShadowedShader->setShaderTexture("d3dGBufferColor", getRenderScene()->_gBuffer.getTextureID(GBuffer::_diffuseAndSpecular), GL_TEXTURE_2D);

	_pointLightShadowedShader->setShaderTexture("d3dNoiseMap", _noiseMap->getTextureID(), GL_TEXTURE_2D);

	_pointLightShadowedShader->setUniformv2f("d3dGBufferSizeInv", gBufferSizeInv);

	const GLchar* pointLightShadowedNames[7] = {
		"d3dPointLightPosition",
		"d3dPointLightColor",
		"d3dPointLightRange",
		"d3dPointLightRangeInv",
		"d3dToLightSpace",
		"d3dProj22",
		"d3dProj23"
	};

	_pointLightShadowedLightUBOShaderInterface.reset(new UBOShaderInterface());
	_pointLightShadowedLightUBOShaderInterface->create("d3dPointLightShadowed", _pointLightShadowedShader.get(), pointLightShadowedNames, 7);

	_spotLightShader->bind();

	_spotLightShader->setShaderTexture("d3dGBufferPosition", getRenderScene()->_gBuffer.getTextureID(GBuffer::_positionAndEmissive), GL_TEXTURE_2D);
	_spotLightShader->setShaderTexture("d3dGBufferNormal", getRenderScene()->_gBuffer.getTextureID(GBuffer::_normalAndShininess), GL_TEXTURE_2D);
	_spotLightShader->setShaderTexture("d3dGBufferColor", getRenderScene()->_gBuffer.getTextureID(GBuffer::_diffuseAndSpecular), GL_TEXTURE_2D);

	_spotLightShader->setUniformv2f("d3dGBufferSizeInv", gBufferSizeInv);

	const GLchar* spotLightNames[8] = {
		"d3dSpotLightPosition",
		"d3dSpotLightColor",
		"d3dSpotLightRange",
		"d3dSpotLightRangeInv",
		"d3dSpotLightDirection",
		"d3dSpotLightSpreadAngleCos",
		"d3dSpotLightSpreadAngleCosFlipInv",
		"d3dSpotLightExponent"
	};

	_spotLightLightUBOShaderInterface.reset(new UBOShaderInterface());
	_spotLightLightUBOShaderInterface->create("d3dSpotLight", _spotLightShader.get(), spotLightNames, 8);

	_directionalLightShader->bind();

	_directionalLightShader->setShaderTexture("d3dGBufferPosition", getRenderScene()->_gBuffer.getTextureID(GBuffer::_positionAndEmissive), GL_TEXTURE_2D);
	_directionalLightShader->setShaderTexture("d3dGBufferNormal", getRenderScene()->_gBuffer.getTextureID(GBuffer::_normalAndShininess), GL_TEXTURE_2D);
	_directionalLightShader->setShaderTexture("d3dGBufferColor", getRenderScene()->_gBuffer.getTextureID(GBuffer::_diffuseAndSpecular), GL_TEXTURE_2D);

	_directionalLightShader->setUniformv2f("d3dGBufferSizeInv", gBufferSizeInv);

	const GLchar* directionalLightNames[2] = {
		"d3dDirectionalLightColor",
		"d3dDirectionalLightDirection"
	};

	_directionalLightLightUBOShaderInterface.reset(new UBOShaderInterface());
	_directionalLightLightUBOShaderInterface->create("d3dDirectionalLight", _directionalLightShader.get(), directionalLightNames, 2);

	_directionalLightShadowedShader->bind();

	_directionalLightShadowedShader->setShaderTexture("d3dGBufferPosition", getRenderScene()->_gBuffer.getTextureID(GBuffer::_positionAndEmissive), GL_TEXTURE_2D);
	_directionalLightShadowedShader->setShaderTexture("d3dGBufferNormal", getRenderScene()->_gBuffer.getTextureID(GBuffer::_normalAndShininess), GL_TEXTURE_2D);
	_directionalLightShadowedShader->setShaderTexture("d3dGBufferColor", getRenderScene()->_gBuffer.getTextureID(GBuffer::_diffuseAndSpecular), GL_TEXTURE_2D);

	_directionalLightShadowedShader->setShaderTexture("d3dNoiseMap", _noiseMap->getTextureID(), GL_TEXTURE_2D);

	_directionalLightShadowedShader->setUniformv2f("d3dGBufferSizeInv", gBufferSizeInv);

	const GLchar* directionalLightShadowedNames[5] = {
		"d3dDirectionalLightColor",
		"d3dDirectionalLightDirection",
		"d3dNumCascades",
		"d3dSplitDistances",
		"d3dLightBiasViewProjections"
	};

	_directionalLightShadowedLightUBOShaderInterface.reset(new UBOShaderInterface());
	_directionalLightShadowedLightUBOShaderInterface->create("d3dDirectionalLightShadowed", _directionalLightShadowedShader.get(), directionalLightShadowedNames, 5);

	_emissiveRenderShader->bind();

	_emissiveRenderShader->setShaderTexture("d3dGBufferPosition", getRenderScene()->_gBuffer.getTextureID(GBuffer::_positionAndEmissive), GL_TEXTURE_2D);
	_emissiveRenderShader->setShaderTexture("d3dGBufferColor", getRenderScene()->_gBuffer.getTextureID(GBuffer::_diffuseAndSpecular), GL_TEXTURE_2D);
	
	_emissiveRenderShader->setUniformv2f("d3dGBufferSizeInv", gBufferSizeInv);

	_depthRenderShader->bind();
}

void SceneObjectLighting::add(SceneObjectPointLight &light) {
	_pointLights.push_back(light);
}

void SceneObjectLighting::add(SceneObjectSpotLight &light) {
	_spotLights.push_back(light);
}

void SceneObjectLighting::add(SceneObjectDirectionalLight &light) {
	_directionalLights.push_back(light);
}

void SceneObjectLighting::add(SceneObjectPointLightShadowed &light) {
	_shadowedPointLights.push_back(light);
}

/*
void SceneObjectLighting::add(SceneObjectSpotLightShadowed &light) {

}*/

void SceneObjectLighting::add(SceneObjectDirectionalLightShadowed &light) {
	_shadowedDirectionalLights.push_back(light);
}

void SceneObjectLighting::postRender() {
	getRenderScene()->_shaderSwitchesEnabled = false;

	glDisable(GL_DEPTH_TEST);

	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);

	_emissiveRenderShader->bind();
	_emissiveRenderShader->bindShaderTextures();

	getRenderScene()->renderNormalizedQuad();

	// Render ambient light
	if (_ambientLight != Vec3f(0.0f, 0.0f, 0.0f)) {
		_ambientLightShader->bind();
		_ambientLightShader->bindShaderTextures();

		_ambientLightShader->setUniformv3f("d3dColor", _ambientLight);

		getRenderScene()->renderNormalizedQuad();
	}

	// --------------------------------------- Render all directional lights ---------------------------------------

	_directionalLightShader->bind();
	_directionalLightShader->bindShaderTextures();

	for (std::list<SceneObjectRef>::iterator it = _directionalLights.begin(); it != _directionalLights.end(); it++) {
		SceneObjectDirectionalLight* pDirectionalLight = static_cast<SceneObjectDirectionalLight*>((*it).get());

		pDirectionalLight->updateUniformBuffer();

		_directionalLightLightUBOShaderInterface->bindBufferToSetIndex(*pDirectionalLight->_uniformBuffer);

		getRenderScene()->renderNormalizedQuad();

		D3D_GL_ERROR_CHECK();
	}

	_directionalLightShadowedShader->bind();

	for (std::list<SceneObjectRef>::iterator it = _shadowedDirectionalLights.begin(); it != _shadowedDirectionalLights.end(); it++) {
		SceneObjectDirectionalLightShadowed* pDirectionalLightShadowed = static_cast<SceneObjectDirectionalLightShadowed*>((*it).get());

		pDirectionalLightShadowed->updateUniformBuffer();

		_directionalLightShadowedLightUBOShaderInterface->bindBufferToSetIndex(*pDirectionalLightShadowed->_uniformBuffer);

		pDirectionalLightShadowed->setCascadeShadowMaps(_directionalLightShadowedShader.get());

		_directionalLightShadowedShader->bindShaderTextures();

		Shader::validate(_directionalLightShadowedShader->getProgramID());
		getRenderScene()->renderNormalizedQuad();

		D3D_GL_ERROR_CHECK();
	}

	glEnable(GL_DEPTH_TEST);

	// ------------------------------------------- Render all point lights -------------------------------------------

	_pointLightShader->bind();
	_pointLightShader->bindShaderTextures();

	if (_needsAttenuationUpdate)
		_pointLightShader->setUniformv3f("d3dAttenuation", _attenuation);

	_sphereModel->_meshes[0]->setAttributes();

	for (std::list<SceneObjectRef>::iterator it = _pointLights.begin(); it != _pointLights.end(); it++) {
		SceneObjectPointLight* pPointLight = static_cast<SceneObjectPointLight*>((*it).get());

		pPointLight->updateUniformBuffer();

		_pointLightLightUBOShaderInterface->bindBufferToSetIndex(*pPointLight->_uniformBuffer);

		pPointLight->setTransform(getRenderScene());

		// If camera is inside light, do not perform depth test (would cull it away improperly)
		if ((pPointLight->getPosition() - getRenderScene()->_renderCamera._position).magnitude() < pPointLight->getRange() * _pointLightRangeScalar) {
			glCullFace(GL_FRONT);

			glDepthFunc(GL_GREATER);

			_sphereModel->_meshes[0]->renderFromAttributes();

			glDepthFunc(GL_LESS);

			glCullFace(GL_BACK);
		}
		else
			_sphereModel->render();
	}

	// ------------------------------------------- Render all shadowed point lights -------------------------------------------

	_pointLightShadowedShader->bind();
	_pointLightShadowedShader->bindShaderTextures();

	if (_needsAttenuationUpdate)
		_pointLightShadowedShader->setUniformv3f("d3dAttenuation", _attenuation);

	_sphereModel->_meshes[0]->setAttributes();

	for (std::list<SceneObjectRef>::iterator it = _shadowedPointLights.begin(); it != _shadowedPointLights.end(); it++) {
		SceneObjectPointLightShadowed* pPointLightShadowed = static_cast<SceneObjectPointLightShadowed*>((*it).get());

		pPointLightShadowed->updateUniformBuffer();

		_pointLightShadowedLightUBOShaderInterface->bindBufferToSetIndex(*pPointLightShadowed->_uniformBuffer);

		pPointLightShadowed->setTransform(getRenderScene());

		pPointLightShadowed->setShadowMap(_pointLightShadowedShader.get());

		_pointLightShadowedShader->bindShaderTextures();

		// If camera is inside light, do not perform depth test (would cull it away improperly)
		if ((pPointLightShadowed->getPosition() - getRenderScene()->_renderCamera._position).magnitude() < pPointLightShadowed->getRange() * _pointLightRangeScalar) {
			glCullFace(GL_FRONT);

			glDepthFunc(GL_GREATER);

			_sphereModel->_meshes[0]->renderFromAttributes();

			glDepthFunc(GL_LESS);

			glCullFace(GL_BACK);
		}
		else
			_sphereModel->render();
	}

	// ------------------------------------------- Render all spot lights -------------------------------------------

	_spotLightShader->bind();
	_spotLightShader->bindShaderTextures();

	if (_needsAttenuationUpdate)
		_spotLightShader->setUniformv3f("d3dAttenuation", _attenuation);

	_coneModel->_meshes[0]->setAttributes();

	for (std::list<SceneObjectRef>::iterator it = _spotLights.begin(); it != _spotLights.end(); it++) {
		SceneObjectSpotLight* pSpotLight = static_cast<SceneObjectSpotLight*>((*it).get());

		pSpotLight->updateUniformBuffer();

		_spotLightLightUBOShaderInterface->bindBufferToSetIndex(*pSpotLight->_uniformBuffer);

		pSpotLight->setTransform(getRenderScene());

		// If camera is inside light, do not perform depth test (would cull it away improperly)
		if ((pSpotLight->getPosition() - getRenderScene()->_renderCamera._position).magnitude() < pSpotLight->getRange() * _spotLightRangeScalar) {
			glCullFace(GL_FRONT);

			glDepthFunc(GL_GREATER);

			_coneModel->_meshes[0]->renderFromAttributes();

			glDepthFunc(GL_LESS);

			glCullFace(GL_BACK);
		}
		else
			_coneModel->render();
	}

	// Clear all lists so that they may be repopulated next frame
	_pointLights.clear();
	_spotLights.clear();
	_directionalLights.clear();
	_shadowedPointLights.clear();
	//_shadowedSpotLights.clear();
	_shadowedDirectionalLights.clear();

	glDisable(GL_BLEND);

	_needsAttenuationUpdate = false;

	getRenderScene()->_shaderSwitchesEnabled = true;
}

void SceneObjectLighting::setAttenuation(const Vec3f &attenuation) {
	_attenuation = attenuation;

	_needsAttenuationUpdate = true;
}

Matrix4x4f d3d::getBiasMatrix() {
	Matrix4x4f mat;

	mat._elements[0] = 0.5f; mat._elements[4] = 0.0f; mat._elements[8] = 0.0f; mat._elements[12] = 0.5f;
	mat._elements[1] = 0.0f; mat._elements[5] = 0.5f; mat._elements[9] = 0.0f; mat._elements[13] = 0.5f;
	mat._elements[2] = 0.0f; mat._elements[6] = 0.0f; mat._elements[10] = 0.5f; mat._elements[14] = 0.5f;
	mat._elements[3] = 0.0f; mat._elements[7] = 0.0f; mat._elements[11] = 0.0f; mat._elements[15] = 1.0f;

	return mat;
}