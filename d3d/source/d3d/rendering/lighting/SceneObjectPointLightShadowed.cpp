#include <d3d/rendering/lighting/SceneObjectPointLightShadowed.h>

using namespace d3d;

SceneObjectPointLightShadowed::SceneObjectPointLightShadowed()
: _position(0.0f, 0.0f, 0.0f),
_color(1.0f, 1.0f, 1.0f),
_range(1.0f),
_shadowMapZNear(0.01f),
_needsUniformBufferUpdate(true),
_enabled(true)
{
	_renderMask = 0xffff;

	_aabb._lowerBound = Vec3f(-1.0f, -1.0f, -1.0f);
	_aabb._lowerBound = Vec3f(1.0f, 1.0f, 1.0f);

	_aabb.calculateHalfDims();
	_aabb.calculateCenter();

	_updateFaces.assign(true);

	_faceObjects.reset(new std::array<std::vector<AABB3D>, 6>());
}

void SceneObjectPointLightShadowed::create(SceneObjectLighting* pLighting, unsigned int size) {
	_lighting = pLighting;

	_uniformBuffer.reset(new VBO());
	_uniformBuffer->create();

	pLighting->_pointLightShadowedLightUBOShaderInterface->setUpBuffer(*_uniformBuffer);

	_cubeMap.reset(new CubeMapDepthRT());
	_cubeMap->create(size, CubeMapDepthRT::_16);

	updateUniformBuffer();
}

void SceneObjectPointLightShadowed::setPosition(const Vec3f &position) {
	_position = position;

	_needsUniformBufferUpdate = true;

	_aabb.setCenter(position);

	_updateFaces.assign(true);

	updateAABB();
}

void SceneObjectPointLightShadowed::setColor(const Vec3f &color) {
	_color = color;

	_needsUniformBufferUpdate = true;
}

void SceneObjectPointLightShadowed::setRange(float range) {
	_range = range;

	_needsUniformBufferUpdate = true;

	_aabb.setHalfDims(Vec3f(_range, _range, _range));

	_updateFaces.assign(true);

	updateAABB();
}

void SceneObjectPointLightShadowed::updateUniformBuffer() {
	_uniformBuffer->bind(GL_UNIFORM_BUFFER);

	SceneObjectLighting* pLighting = static_cast<SceneObjectLighting*>(_lighting.get());

	pLighting->_pointLightShadowedLightUBOShaderInterface->setUniformv3f("d3dPointLightPosition", getRenderScene()->_renderCamera.getViewMatrix() * _position);
	pLighting->_pointLightShadowedLightUBOShaderInterface->setUniformmat4("d3dToLightSpace", Matrix4x4f::translateMatrix(-_position) * getRenderScene()->_renderCamera.getViewInverseMatrix());

	if (_needsUniformBufferUpdate) {
		pLighting->_pointLightShadowedLightUBOShaderInterface->setUniformv3f("d3dPointLightColor", _color);
		pLighting->_pointLightShadowedLightUBOShaderInterface->setUniformf("d3dPointLightRange", _range);
		pLighting->_pointLightShadowedLightUBOShaderInterface->setUniformf("d3dPointLightRangeInv", 1.0f / _range);

		float zNear = _shadowMapZNear;
		float zFar = _range;
		float nearMinusFar = zNear - zFar;
		float proj22 = (zNear + zFar) / nearMinusFar;
		float proj23 = (2.0f * zNear * zFar) / nearMinusFar;

		pLighting->_pointLightShadowedLightUBOShaderInterface->setUniformf("d3dProj22", proj22);
		pLighting->_pointLightShadowedLightUBOShaderInterface->setUniformf("d3dProj23", proj23);

		_needsUniformBufferUpdate = false;
	}
}

void SceneObjectPointLightShadowed::preRender() {
	if (_enabled) {
		// Check which faces need updates
		std::array<std::vector<AABB3D>, 6> newFaceObjects;

		bool needsUpdate = false;

		for (unsigned char i = 0; i < 6; i++) {
			Camera c = _cubeMap->getCamera(i, _position, _shadowMapZNear, _range);

			std::vector<SceneObjectRef> visible;

			getRenderScene()->findVisible(c, visible);

			newFaceObjects[i].resize(visible.size());

			for (size_t j = 0; j < visible.size(); j++)
				newFaceObjects[i][j] = visible[j]->getAABB();

			if (newFaceObjects[i].size() != (*_faceObjects)[i].size()) {
				_updateFaces[i] = true;

				needsUpdate = true;
			}
			else {
				for (size_t j = 0; j < visible.size(); j++) {
					// Check for equality with previous
					if (newFaceObjects[i][j] != (*_faceObjects)[i][j]) {
						_updateFaces[i] = true;

						needsUpdate = true;

						break;
					}
				}
			}
		}

		(*_faceObjects) = newFaceObjects;

		// ---------------------------- Render to Shadow Map ----------------------------

		if (needsUpdate) {
			SceneObjectLighting* pLighting = static_cast<SceneObjectLighting*>(_lighting.get());

			getRenderScene()->useShader(pLighting->_depthRenderShader.get());

			getRenderScene()->_shaderSwitchesEnabled = false;
			getRenderScene()->_renderingShadows = true;

			_cubeMap->bind();
			_cubeMap->setViewport();

			glClearDepth(1.0f);

			for (size_t i = 0; i < 6; i++)
			if (_updateFaces[i]) {
				_cubeMap->renderFace(getRenderScene(), _position, i, _shadowMapZNear, _range, _range);

				_updateFaces[i] = false;
			}

			Shader::unbind();
			CubeMapDepthRT::unbind();

			getRenderScene()->_shaderSwitchesEnabled = true;
			getRenderScene()->_renderingShadows = false;

			D3D_GL_ERROR_CHECK();
		}
	}
}

void SceneObjectPointLightShadowed::deferredRender() {
	if (_enabled && !getRenderScene()->_renderingShadows && getRenderScene()->_shaderSwitchesEnabled)
		static_cast<SceneObjectLighting*>(_lighting.get())->_shadowedPointLights.push_back(*this);
}

void SceneObjectPointLightShadowed::setShadowMap(Shader* pShader) {
	pShader->setShaderTexture("d3dShadowMap", _cubeMap->getDepthTextureID(), GL_TEXTURE_CUBE_MAP);
}