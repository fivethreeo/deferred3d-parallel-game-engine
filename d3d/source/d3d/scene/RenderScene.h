#pragma once

#include <d3d/scene/Scene.h>

#include <d3d/rendering/bufferobjects/TextureRT.h>
#include <d3d/rendering/GBuffer.h>
#include <d3d/rendering/shader/Shader.h>
#include <d3d/rendering/shader/UBOShaderInterface.h>
#include <d3d/rendering/texture/Texture2D.h>
#include <d3d/rendering/mesh/StaticPositionMesh.h>

namespace d3d {
	class RenderScene : public Scene {
	public:
		enum GBufferRenderShaderType {
			_standard = 0, _normal, _heightNormal
		};

	private:
		sf::RenderWindow* _pWindow;

		VBO _sceneUniformBuffer;

		std::array<std::shared_ptr<Shader>, 3> _gBufferRenderShaders;
		std::array<UBOShaderInterface, 3> _gBufferRenderShaderMaterialUBOInterfaces;

		std::shared_ptr<Texture2D> _whiteTexture;

		StaticPositionMesh _normalizedQuad;

		Matrix4x4f _viewModelMatrix;
		Matrix4x4f _projectionViewModelMatrix;
		Matrix3x3f _normalMatrix;

	public:
		GBuffer _gBuffer;

		Vec4f _clearColor;

		Camera _renderCamera;

		bool _shaderSwitchesEnabled;

		bool _renderingShadows;

		RenderScene();

		void createRenderScene(size_t numWorkers, const AABB3D &rootRegion, sf::RenderWindow* pWindow,
			const std::shared_ptr<Shader> &gBufferRender,
			const std::shared_ptr<Shader> &gBufferRenderNormal,
			const std::shared_ptr<Shader> &gBufferRenderHeightNormal,
			const std::shared_ptr<Texture2D> &whiteTexture,
			ComputeSystem::DeviceType computeDeviceType);

		// Renders scene to the currently bound buffer
		void renderShadow();

		// Render scene to a render target
		void render(TextureRT &target);
		void renderToMainFramebuffer();

		void frame(float dt);

		void useShader(Shader* pShader);
		void useShader(GBufferRenderShaderType type);
		void useShader(class Material &material);

		void updateShaderUniforms();

		void setTransform(const Matrix4x4f &transform);

		static void createMaterialInterface(UBOShaderInterface &materialInterface, Shader* pShader);

		void renderNormalizedQuad() {
			_normalizedQuad.render();
		}

		sf::RenderWindow* getRenderWindow() const {
			return _pWindow;
		}

		const std::shared_ptr<Texture2D> &getWhiteTexture() const {
			return _whiteTexture;
		}

		UBOShaderInterface &getMaterialUBOShaderInterface(GBufferRenderShaderType type) {
			return _gBufferRenderShaderMaterialUBOInterfaces[type];
		}

		VBO &getSceneUniformBuffer() {
			return _sceneUniformBuffer;
		}

		const Matrix4x4f &getCurrentViewModelMatrix() const {
			return _viewModelMatrix;
		}

		const Matrix4x4f &getCurrentProjectionViewModelMatrix() const {
			return _projectionViewModelMatrix;
		}

		const Matrix3x3f &getCurrentNormalMatrix() const {
			return _normalMatrix;
		}
	};
}