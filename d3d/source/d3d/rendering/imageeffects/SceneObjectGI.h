#pragma once

#include <d3d/scene/RenderScene.h>
#include <d3d/rendering/imageeffects/SceneObjectEffectBuffer.h>

#include <d3d/bvh/BVHTree.h>

#include <d3d/rendering/mesh/StaticMesh.h>
#include <d3d/rendering/model/StaticModel.h>

#include <d3d/rendering/lighting/SceneObjectPointLight.h>
#include <d3d/rendering/lighting/SceneObjectSpotLight.h>
#include <d3d/rendering/lighting/SceneObjectDirectionalLight.h>
#include <d3d/rendering/lighting/SceneObjectPointLightShadowed.h>
#include <d3d/rendering/lighting/SceneObjectDirectionalLightShadowed.h>

namespace d3d {
	class SceneObjectGI : public SceneObject {
	public:
		struct BufferMaterial {
			Vec3f _diffuseColor;
			float _specularColor;
			float _shininess;
			float _emissiveColor;

			// An index of 0 means unused
			int _diffuseMapIndexPlusOne;
			int _specularMapIndexPlusOne;
			int _shininessMapIndexPlusOne;
			int _emissiveMapIndexPlusOne;

			// Special
			int _normalMapIndexPlusOne;
		};

		struct Vertex {
			Vec3f _position;
			Vec3f _normal;
			Vec2f _texCoord;
		};

		struct BufferTriangle {
			Vertex _v0;
			Vertex _v1;
			Vertex _v2;

			int _materialIndex;
		};

		struct BufferTreeNode {
			Vec3f _minBounds;
			Vec3f _maxBounds;

			int _parentIndexPlusOne;

			int _childIndexPlusOne0;
			int _childIndexPlusOne1;

			int _trianglesStartIndex;
			int _numTriangles;
		};

		struct BufferLight {
			int _type;
			Vec3f _position;
			Vec3f _color;
			Vec3f _direction;
			float _range;
			float _rangeInv;
			float _lightSpreadAngleCos;
			float _lightSpreadAngleCosFlipInv;
			float _lightExponent;
		};

		struct BufferDataDesc {
			std::vector<int> _indices;
			std::vector<int> _offsets;
			int _blockIndex;
			int _blockSize;
			int _alignOffset;
		};

	private:
		d3d::SceneObjectRef _effectBuffer;

		std::shared_ptr<Shader> _blurShaderHorizontalEdgeAware;
		std::shared_ptr<Shader> _blurShaderVerticalEdgeAware;

		std::shared_ptr<Shader> _giProgram;

		std::shared_ptr<Shader> _renderImageShader;
		std::shared_ptr<Shader> _renderImageMultShader;

		std::shared_ptr<Texture2D> _noiseMap;

		GLuint _targetRender;
		unsigned int _targetWidth, _targetHeight;

		std::shared_ptr<BVHTree> _tree;

		std::shared_ptr<VBO> _treeBuffer;
		std::shared_ptr<VBO> _triangleBuffer;
		std::shared_ptr<VBO> _materialBuffer;
		std::shared_ptr<VBO> _lightBuffer;
		GLuint _materialTextures;

		// For compilation
		std::shared_ptr<std::vector<Material>> _materials;

		unsigned int _numLights;

		std::shared_ptr<TextureRT> _imageRescaleRT;

		static const int _treeNumNames = 7;
		static const int _triangleNumNames = 10;
		static const int _materialNumNames = 9;
		static const int _lightNumNames = 9;

		BufferDataDesc _treeDataDesc;
		BufferDataDesc _triangleDataDesc;
		BufferDataDesc _materialDataDesc;
		BufferDataDesc _lightDataDesc;

		std::list<SceneObjectRef> _pointLights;
		std::list<SceneObjectRef> _spotLights;
		std::list<SceneObjectRef> _directionalLights;
		std::list<SceneObjectRef> _shadowedPointLights;
		//std::list<SceneObjectRef> _shadowedSpotLights;
		std::list<SceneObjectRef> _shadowedDirectionalLights;

		static int toBufferRec(const BVHNode* pNode, std::vector<BufferTriangle> &triangles, std::vector<BufferTreeNode> &buffer, int parentIndex);

	public:
		size_t _numBlurPasses;
		float _blurRadius;

		static const int _workGroupSize = 16;

		SceneObjectGI()
			: _numBlurPasses(8), _blurRadius(0.003642f), _targetRender(0), _numLights(0)
		{}

		void create(const std::shared_ptr<Shader> &blurShaderHorizontalEdgeAware,
			const std::shared_ptr<Shader> &blurShaderVerticalEdgeAware,
			const std::shared_ptr<Shader> &renderImageShader,
			const std::shared_ptr<Shader> &renderImageMultShader,
			const std::shared_ptr<Shader> &giProgram,
			const std::shared_ptr<Texture2D> &noiseMap,
			unsigned int imageWidth, unsigned int imageHeight,
			float sampleScalar);

		void clearGeometry(const AABB3D &root);
		void clearMaterials();
		void clearLights();

		void compileGeometry();
		void compileMaterials();
		void compileLights();

		void addMesh(const StaticMesh &mesh, d3d::Matrix4x4f &transform, const Matrix3x3f &normalMatrix, int materialIndex);
		void addMaterial(const Material &material);

		void addLight(const std::shared_ptr<SceneObjectPointLight> &light);
		void addLight(const std::shared_ptr<SceneObjectSpotLight> &light);
		void addLight(const std::shared_ptr<SceneObjectDirectionalLight> &light);
		void addLight(const std::shared_ptr<SceneObjectPointLightShadowed> &light);
		void addLight(const std::shared_ptr<SceneObjectDirectionalLightShadowed> &light);

		void addModel(const StaticModel &model, d3d::Matrix4x4f &transform);

		// Inherited from SceneObject
		void onAdd();
		void postRender();
		void onDestroy();

		SceneObjectGI* copyFactory() {
			return new SceneObjectGI(*this);
		}
	};
}