#include <d3d/rendering/imageeffects/SceneObjectGI.h>

#include <d3d/rendering/lighting/SceneObjectLighting.h>

using namespace d3d;

void SceneObjectGI::create(const std::shared_ptr<Shader> &blurShaderHorizontalEdgeAware,
	const std::shared_ptr<Shader> &blurShaderVerticalEdgeAware,
	const std::shared_ptr<Shader> &renderImageShader,
	const std::shared_ptr<Shader> &renderImageMultShader,
	const std::shared_ptr<Shader> &giProgram,
	const std::shared_ptr<Texture2D> &noiseMap,
	unsigned int imageWidth, unsigned int imageHeight,
	float sampleScalar)
{
	assert(getScene() != nullptr);

	_renderMask = 0xffff;

	_blurShaderHorizontalEdgeAware = blurShaderHorizontalEdgeAware;
	_blurShaderVerticalEdgeAware = blurShaderVerticalEdgeAware;
	_renderImageShader = renderImageShader;
	_renderImageMultShader = renderImageMultShader;
	_giProgram = giProgram;
	_noiseMap = noiseMap;

	_imageRescaleRT.reset(new TextureRT());
	_imageRescaleRT->create(imageWidth, imageHeight, false, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE);

	_targetWidth = static_cast<unsigned int>(static_cast<float>(getRenderScene()->_gBuffer.getWidth()) * sampleScalar);
	_targetHeight = static_cast<unsigned int>(static_cast<float>(getRenderScene()->_gBuffer.getHeight()) * sampleScalar);

	glGenTextures(1, &_targetRender);
	glBindTexture(GL_TEXTURE_2D, _targetRender);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, _targetWidth, _targetHeight, 0, GL_RGBA, GL_HALF_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	_treeBuffer.reset(new VBO());
	_triangleBuffer.reset(new VBO());
	_materialBuffer.reset(new VBO());
	_lightBuffer.reset(new VBO());
	
	glGenTextures(1, &_materialTextures);
	glBindTexture(GL_TEXTURE_2D_ARRAY, _materialTextures);

	_tree.reset(new BVHTree());
	_materials.reset(new std::vector<Material>());

	std::array<const GLchar*, _treeNumNames> treeNames;
	treeNames[0] = "tree[0]._minBounds";
	treeNames[1] = "tree[0]._maxBounds";
	treeNames[2] = "tree[0]._parentIndexPlusOne";
	treeNames[3] = "tree[0]._childIndexPlusOne0";
	treeNames[4] = "tree[0]._childIndexPlusOne1";
	treeNames[5] = "tree[0]._trianglesStartIndex";
	treeNames[6] = "tree[0]._numTriangles";

	std::array<const GLchar*, _triangleNumNames> triangleNames;
	triangleNames[0] = "triangles[0]._v0._position";
	triangleNames[1] = "triangles[0]._v0._normal";
	triangleNames[2] = "triangles[0]._v0._texCoord";
	triangleNames[3] = "triangles[0]._v1._position";
	triangleNames[4] = "triangles[0]._v1._normal";
	triangleNames[5] = "triangles[0]._v1._texCoord";
	triangleNames[6] = "triangles[0]._v2._position";
	triangleNames[7] = "triangles[0]._v2._normal";
	triangleNames[8] = "triangles[0]._v2._texCoord";
	triangleNames[9] = "triangles[0]._materialIndex";

	std::array<const GLchar*, _materialNumNames> materialNames;
	materialNames[0] = "materials[0]._diffuseColor";
	materialNames[1] = "materials[0]._specularColor";
	materialNames[2] = "materials[0]._shininess";
	materialNames[3] = "materials[0]._emissiveColor";
	materialNames[4] = "materials[0]._diffuseMapIndexPlusOne";
	materialNames[5] = "materials[0]._specularMapIndexPlusOne";
	materialNames[6] = "materials[0]._shininessMapIndexPlusOne";
	materialNames[7] = "materials[0]._emissiveMapIndexPlusOne";
	materialNames[8] = "materials[0]._normalMapIndexPlusOne";

	std::array<const GLchar*, _lightNumNames> lightNames;
	lightNames[0] = "lights[0]._type";
	lightNames[1] = "lights[0]._position";
	lightNames[2] = "lights[0]._color";
	lightNames[3] = "lights[0]._direction";
	lightNames[4] = "lights[0]._range";
	lightNames[5] = "lights[0]._rangeInv";
	lightNames[6] = "lights[0]._lightSpreadAngleCos";
	lightNames[7] = "lights[0]._lightSpreadAngleCosFlipInv";
	lightNames[8] = "lights[0]._lightExponent";

	std::array<GLenum, 1> properties1;
	properties1[0] = GL_BUFFER_DATA_SIZE;

	std::array<GLenum, 1> properties2;
	properties2[0] = GL_OFFSET;

	_treeDataDesc._indices.resize(_treeNumNames);
	_triangleDataDesc._indices.resize(_triangleNumNames);
	_materialDataDesc._indices.resize(_materialNumNames);
	_lightDataDesc._indices.resize(_lightNumNames);

	for (int i = 0; i < _treeNumNames; i++)
		_treeDataDesc._indices[i] = glGetProgramResourceIndex(_giProgram->getProgramID(), GL_BUFFER_VARIABLE, treeNames[i]);

	for (int i = 0; i < _triangleNumNames; i++)
		_triangleDataDesc._indices[i] = glGetProgramResourceIndex(_giProgram->getProgramID(), GL_BUFFER_VARIABLE, triangleNames[i]);

	for (int i = 0; i < _materialNumNames; i++)
		_materialDataDesc._indices[i] = glGetProgramResourceIndex(_giProgram->getProgramID(), GL_BUFFER_VARIABLE, materialNames[i]);

	for (int i = 0; i < _lightNumNames; i++)
		_lightDataDesc._indices[i] = glGetProgramResourceIndex(_giProgram->getProgramID(), GL_BUFFER_VARIABLE, lightNames[i]);

	_treeDataDesc._blockIndex = glGetProgramResourceIndex(_giProgram->getProgramID(), GL_SHADER_STORAGE_BLOCK, "TreeBuffer");
	_triangleDataDesc._blockIndex = glGetProgramResourceIndex(_giProgram->getProgramID(), GL_SHADER_STORAGE_BLOCK, "TriangleBuffer");
	_materialDataDesc._blockIndex = glGetProgramResourceIndex(_giProgram->getProgramID(), GL_SHADER_STORAGE_BLOCK, "MaterialBuffer");
	_lightDataDesc._blockIndex = glGetProgramResourceIndex(_giProgram->getProgramID(), GL_SHADER_STORAGE_BLOCK, "LightBuffer");

	_treeDataDesc._offsets.resize(_treeNumNames);
	_triangleDataDesc._offsets.resize(_triangleNumNames);
	_materialDataDesc._offsets.resize(_materialNumNames);
	_lightDataDesc._offsets.resize(_lightNumNames);

	for (int i = 0; i < _treeNumNames; i++)
		glGetProgramResourceiv(_giProgram->getProgramID(), GL_BUFFER_VARIABLE, _treeDataDesc._indices[i], 1, &properties2[0], 1, nullptr, &_treeDataDesc._offsets[i]);

	for (int i = 0; i < _triangleNumNames; i++)
		glGetProgramResourceiv(_giProgram->getProgramID(), GL_BUFFER_VARIABLE, _triangleDataDesc._indices[i], 1, &properties2[0], 1, nullptr, &_triangleDataDesc._offsets[i]);

	for (int i = 0; i < _materialNumNames; i++)
		glGetProgramResourceiv(_giProgram->getProgramID(), GL_BUFFER_VARIABLE, _materialDataDesc._indices[i], 1, &properties2[0], 1, nullptr, &_materialDataDesc._offsets[i]);

	for (int i = 0; i < _lightNumNames; i++)
		glGetProgramResourceiv(_giProgram->getProgramID(), GL_BUFFER_VARIABLE, _lightDataDesc._indices[i], 1, &properties2[0], 1, nullptr, &_lightDataDesc._offsets[i]);

	glGetProgramResourceiv(_giProgram->getProgramID(), GL_SHADER_STORAGE_BLOCK, _treeDataDesc._blockIndex, 1, &properties1[0], 1, nullptr, &_treeDataDesc._blockSize);
	glGetProgramResourceiv(_giProgram->getProgramID(), GL_SHADER_STORAGE_BLOCK, _triangleDataDesc._blockIndex, 1, &properties1[0], 1, nullptr, &_triangleDataDesc._blockSize);
	glGetProgramResourceiv(_giProgram->getProgramID(), GL_SHADER_STORAGE_BLOCK, _materialDataDesc._blockIndex, 1, &properties1[0], 1, nullptr, &_materialDataDesc._blockSize);
	glGetProgramResourceiv(_giProgram->getProgramID(), GL_SHADER_STORAGE_BLOCK, _lightDataDesc._blockIndex, 1, &properties1[0], 1, nullptr, &_lightDataDesc._blockSize);

	_treeDataDesc._alignOffset = (_treeDataDesc._blockSize % 16 == 0 ? _treeDataDesc._blockSize : _treeDataDesc._blockSize - (_treeDataDesc._blockSize % 16) + 16);
	_triangleDataDesc._alignOffset = (_triangleDataDesc._blockSize % 16 == 0 ? _triangleDataDesc._blockSize : _triangleDataDesc._blockSize - (_triangleDataDesc._blockSize % 16) + 16);
	_materialDataDesc._alignOffset = (_materialDataDesc._blockSize % 16 == 0 ? _materialDataDesc._blockSize : _materialDataDesc._blockSize - (_materialDataDesc._blockSize % 16) + 16);
	_lightDataDesc._alignOffset = (_lightDataDesc._blockSize % 16 == 0 ? _lightDataDesc._blockSize : _lightDataDesc._blockSize - (_lightDataDesc._blockSize % 16) + 16);

	_treeBuffer.reset(new VBO());
	_treeBuffer->create();

	_triangleBuffer.reset(new VBO());
	_triangleBuffer->create();

	_materialBuffer.reset(new VBO());
	_materialBuffer->create();

	_lightBuffer.reset(new VBO());
	_lightBuffer->create();

	_giProgram->bind();

	_giProgram->setShaderTexture("d3dGBufferPosition", getRenderScene()->_gBuffer.getTextureID(GBuffer::_positionAndEmissive), GL_TEXTURE_2D);
	_giProgram->setShaderTexture("d3dGBufferNormal", getRenderScene()->_gBuffer.getTextureID(GBuffer::_normalAndShininess), GL_TEXTURE_2D);
	_giProgram->setShaderTexture("d3dGBufferColor", getRenderScene()->_gBuffer.getTextureID(GBuffer::_diffuseAndSpecular), GL_TEXTURE_2D);
	_giProgram->setShaderTexture("d3dMaterialTextures", _materialTextures, GL_TEXTURE_2D);
	_giProgram->setShaderTexture("d3dNoiseTex", noiseMap->getTextureID(), GL_TEXTURE_2D);
	_giProgram->setShaderImage("d3dOutputTexture", _targetRender, 0, false, 0, GL_WRITE_ONLY, GL_RGBA16F);

	_blurShaderHorizontalEdgeAware->bind();

	_blurShaderHorizontalEdgeAware->setShaderTexture("d3dGBufferPosition", getRenderScene()->_gBuffer.getTextureID(GBuffer::_positionAndEmissive), GL_TEXTURE_2D);
	_blurShaderHorizontalEdgeAware->setShaderTexture("d3dGBufferNormal", getRenderScene()->_gBuffer.getTextureID(GBuffer::_normalAndShininess), GL_TEXTURE_2D);

	_blurShaderVerticalEdgeAware->bind();

	_blurShaderVerticalEdgeAware->setShaderTexture("d3dGBufferPosition", getRenderScene()->_gBuffer.getTextureID(GBuffer::_positionAndEmissive), GL_TEXTURE_2D);
	_blurShaderVerticalEdgeAware->setShaderTexture("d3dGBufferNormal", getRenderScene()->_gBuffer.getTextureID(GBuffer::_normalAndShininess), GL_TEXTURE_2D);
}

void SceneObjectGI::onDestroy() {
	if (_targetRender != 0)
		glDeleteTextures(1, &_targetRender);
}

void SceneObjectGI::onAdd() {
	_effectBuffer = getScene()->getNamed("ebuf");

	assert(_effectBuffer.isAlive());
}

void SceneObjectGI::postRender() {
	struct Uint2 {
		unsigned int _x;
		unsigned int _y;

		Uint2()
		{}

		Uint2(unsigned int x, unsigned int y)
			: _x(x), _y(y)
		{}
	};

	glDisable(GL_DEPTH_TEST);

	Matrix3x3f normalMatrixInverse;

	getRenderScene()->_renderCamera.getNormalMatrix().inverse(normalMatrixInverse);

	_giProgram->bind();

	_giProgram->setUniformi("d3dNumLights", _numLights);
	_giProgram->setUniformmat4("d3dViewMatrixInv", getRenderScene()->_renderCamera.getViewInverseMatrix());
	_giProgram->setUniformmat3("d3dNormalMatrixInv", normalMatrixInverse);
	_giProgram->setUniformv2i("d3dTargetSize", _targetWidth, _targetHeight);
	_giProgram->setUniformv2f("d3dTargetSizeInv", Vec2f(1.0f / _targetWidth, 1.0f / _targetHeight));

	_giProgram->bindShaderTextures();
	_giProgram->bindShaderImages();

	// Execute compute shader
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, _treeBuffer->getID());
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, _triangleBuffer->getID());
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, _materialBuffer->getID());
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, _lightBuffer->getID());

	glDispatchCompute(_targetWidth / _workGroupSize + 1, _targetHeight / _workGroupSize + 1, 1);

	//glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);

	glFlush();
	glFinish();

	D3D_GL_ERROR_CHECK();

	SceneObjectEffectBuffer* pEffectBuffer = static_cast<SceneObjectEffectBuffer*>(_effectBuffer.get());

	Vec2f sizeInv(1.0f / static_cast<float>(pEffectBuffer->_fullPing->getWidth()), 1.0f / static_cast<float>(pEffectBuffer->_fullPing->getHeight()));

	_giProgram->unbindShaderImages();

	_blurShaderVerticalEdgeAware->bind();

	_blurShaderVerticalEdgeAware->setShaderTexture("d3dScene", _targetRender, GL_TEXTURE_2D);
	_blurShaderVerticalEdgeAware->setUniformf("d3dBlurSize", _blurRadius);
	_blurShaderVerticalEdgeAware->setUniformv2f("d3dSizeInv", sizeInv);

	_blurShaderVerticalEdgeAware->bindShaderTextures();

	pEffectBuffer->_fullPing->bind();
	pEffectBuffer->_fullPing->setViewport();

	getRenderScene()->renderNormalizedQuad();

	// Blur the buffer
	for (size_t i = 0; i < _numBlurPasses; i++) {
		_blurShaderHorizontalEdgeAware->bind();

		_blurShaderHorizontalEdgeAware->setShaderTexture("d3dScene", pEffectBuffer->_fullPing->getTextureID(), GL_TEXTURE_2D);
		_blurShaderHorizontalEdgeAware->setUniformf("d3dBlurSize", _blurRadius);
		_blurShaderHorizontalEdgeAware->setUniformv2f("d3dSizeInv", sizeInv);

		_blurShaderHorizontalEdgeAware->bindShaderTextures();

		pEffectBuffer->_fullPong->bind();
		pEffectBuffer->_fullPong->setViewport();

		getRenderScene()->renderNormalizedQuad();

		_blurShaderVerticalEdgeAware->bind();

		_blurShaderVerticalEdgeAware->setShaderTexture("d3dScene", pEffectBuffer->_fullPong->getTextureID(), GL_TEXTURE_2D);
		_blurShaderVerticalEdgeAware->setUniformf("d3dBlurSize", _blurRadius);
		_blurShaderVerticalEdgeAware->setUniformv2f("d3dSizeInv", sizeInv);

		_blurShaderVerticalEdgeAware->bindShaderTextures();

		pEffectBuffer->_fullPing->bind();
		pEffectBuffer->_fullPing->setViewport();

		getRenderScene()->renderNormalizedQuad();
	}

	_blurShaderHorizontalEdgeAware->bind();

	_blurShaderHorizontalEdgeAware->setShaderTexture("d3dScene", pEffectBuffer->_fullPing->getTextureID(), GL_TEXTURE_2D);
	_blurShaderHorizontalEdgeAware->setUniformf("d3dBlurSize", _blurRadius);
	_blurShaderHorizontalEdgeAware->setUniformv2f("d3dSizeInv", sizeInv);

	_blurShaderHorizontalEdgeAware->bindShaderTextures();

	pEffectBuffer->_fullPong->bind();
	pEffectBuffer->_fullPong->setViewport();

	getRenderScene()->renderNormalizedQuad();

	Vec2f gBufferSizeInv(1.0f / static_cast<float>(getRenderScene()->_gBuffer.getWidth()), 1.0f / static_cast<float>(getRenderScene()->_gBuffer.getHeight()));

	// Blend with effect buffer
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);

	_renderImageMultShader->bind();

	_renderImageMultShader->setShaderTexture("d3dScene", pEffectBuffer->_fullPong->getTextureID(), GL_TEXTURE_2D);
	_renderImageMultShader->setShaderTexture("d3dColor", getRenderScene()->_gBuffer.getTextureID(GBuffer::_diffuseAndSpecular), GL_TEXTURE_2D);

	_renderImageMultShader->setUniformv2f("d3dGBufferSizeInv", gBufferSizeInv);

	_renderImageMultShader->bindShaderTextures();

	getRenderScene()->_gBuffer.bind();
	getRenderScene()->_gBuffer.setDrawEffect();
	getRenderScene()->_gBuffer.setReadEffect();

	getRenderScene()->_gBuffer.setViewport();

	getRenderScene()->renderNormalizedQuad();

	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
}

int SceneObjectGI::toBufferRec(const BVHNode* pNode, std::vector<BufferTriangle> &triangles, std::vector<BufferTreeNode> &buffer, int parentIndex) {
	buffer.push_back(BufferTreeNode());

	size_t myIndex = buffer.size() - 1;

	buffer[myIndex]._minBounds = pNode->getAABB()._lowerBound;
	buffer[myIndex]._maxBounds = pNode->getAABB()._upperBound;

	buffer[myIndex]._parentIndexPlusOne = parentIndex;

	buffer[myIndex]._trianglesStartIndex = triangles.size();
	buffer[myIndex]._numTriangles = pNode->getTriangles().size();

	// Add node triangles
	for (size_t i = 0; i < pNode->getTriangles().size(); i++) {
		BufferTriangle triangle;

		std::array<Vertex, 3> vertices;

		for (size_t vi = 0; vi < 3; vi++) {
			vertices[vi]._position = pNode->getTriangles()[i]._points[vi];
			vertices[vi]._normal = pNode->getTriangles()[i]._normals[vi];
			vertices[vi]._texCoord = pNode->getTriangles()[i]._uv[vi];
		}

		triangle._v0 = vertices[0];
		triangle._v1 = vertices[1];
		triangle._v2 = vertices[2];

		triangle._materialIndex = pNode->getTriangles()[i]._materialIndex;

		triangles.push_back(triangle);
	}

	size_t myIndexPlusOne = myIndex + 1;

	buffer[myIndex]._childIndexPlusOne0 = 0;
	buffer[myIndex]._childIndexPlusOne1 = 0;

	if (pNode->getLeft() != nullptr)
		buffer[myIndex]._childIndexPlusOne0 = toBufferRec(pNode->getLeft(), triangles, buffer, myIndexPlusOne);

	if (pNode->getRight() != nullptr)
		buffer[myIndex]._childIndexPlusOne1 = toBufferRec(pNode->getRight(), triangles, buffer, myIndexPlusOne);

	return myIndexPlusOne;
}

void SceneObjectGI::clearGeometry(const AABB3D &root) {
	_tree->create(root);
}

void SceneObjectGI::clearMaterials() {
	_materials->clear();
}

void SceneObjectGI::clearLights() {
	_numLights = 0;

	_pointLights.clear();
	_spotLights.clear();
	_directionalLights.clear();
	_shadowedPointLights.clear();
	_shadowedDirectionalLights.clear();
}

void SceneObjectGI::addMesh(const StaticMesh &mesh, d3d::Matrix4x4f &transform, const Matrix3x3f &normalMatrix, int materialIndex) {
	std::vector<StaticMesh::Vertex> transformedVertices(mesh._vertices.size());

	for (size_t i = 0; i < mesh._vertices.size(); i++) {
		transformedVertices[i]._position = transform * mesh._vertices[i]._position;
		transformedVertices[i]._normal = normalMatrix * mesh._vertices[i]._normal;
		transformedVertices[i]._texCoord = mesh._vertices[i]._texCoord;
	}

	for (size_t i = 0; i < mesh._indices.size(); i += 3) {
		FormTriangle triangle;

		for (int ci = 0; ci < 3; ci++) {
			triangle._points[ci] = transformedVertices[mesh._indices[i + ci]]._position;
			triangle._normals[ci] = transformedVertices[mesh._indices[i + ci]]._normal;
			triangle._uv[ci] = transformedVertices[mesh._indices[i + ci]]._texCoord;
		}

		triangle._materialIndex = materialIndex;

		_tree->add(triangle);
	}
}

void SceneObjectGI::addMaterial(const Material &material) {
	_materials->push_back(material);
}

void SceneObjectGI::addModel(const StaticModel &model, d3d::Matrix4x4f &transform) {
	Matrix3x3f upperLeftSubmatrixInverse;

	transform.getUpperLeftMatrix3x3f().inverse(upperLeftSubmatrixInverse);

	Matrix3x3f normalMatrix = upperLeftSubmatrixInverse.transpose();

	for (size_t i = 0; i < model._meshes.size(); i++)
		addMesh(*model._meshes[i]._mesh, transform, normalMatrix, model._meshes[i]._materialIndex + _materials->size());

	for (size_t i = 0; i < model._materials.size(); i++)
		addMaterial(model._materials[i]);
}

void SceneObjectGI::addLight(const std::shared_ptr<SceneObjectPointLight> &light) {
	_pointLights.push_back(*light);
}

void SceneObjectGI::addLight(const std::shared_ptr<SceneObjectSpotLight> &light) {
	_spotLights.push_back(*light);
}

void SceneObjectGI::addLight(const std::shared_ptr<SceneObjectDirectionalLight> &light) {
	_directionalLights.push_back(*light);
}

void SceneObjectGI::addLight(const std::shared_ptr<SceneObjectPointLightShadowed> &light) {
	_shadowedPointLights.push_back(*light);
}

void SceneObjectGI::addLight(const std::shared_ptr<SceneObjectDirectionalLightShadowed> &light) {
	_shadowedDirectionalLights.push_back(*light);
}

void SceneObjectGI::compileGeometry() {
	_tree->compile();

	std::vector<BufferTreeNode> treeNodes;
	std::vector<BufferTriangle> triangles;

	toBufferRec(_tree->getRootNode(), triangles, treeNodes, 0);

	// ------------------------------------- Compile Buffer -------------------------------------

	std::vector<unsigned char> treeData;

	treeData.resize(treeNodes.size() * _treeDataDesc._alignOffset);

	for (size_t i = 0; i < treeNodes.size(); i++) {
		memcpy(&treeData[i * _treeDataDesc._alignOffset + _treeDataDesc._offsets[0]], &treeNodes[i]._minBounds, sizeof(Vec3f));
		memcpy(&treeData[i * _treeDataDesc._alignOffset + _treeDataDesc._offsets[1]], &treeNodes[i]._maxBounds, sizeof(Vec3f));
		memcpy(&treeData[i * _treeDataDesc._alignOffset + _treeDataDesc._offsets[2]], &treeNodes[i]._parentIndexPlusOne, sizeof(int));
		memcpy(&treeData[i * _treeDataDesc._alignOffset + _treeDataDesc._offsets[3]], &treeNodes[i]._childIndexPlusOne0, sizeof(int));
		memcpy(&treeData[i * _treeDataDesc._alignOffset + _treeDataDesc._offsets[4]], &treeNodes[i]._childIndexPlusOne1, sizeof(int));
		memcpy(&treeData[i * _treeDataDesc._alignOffset + _treeDataDesc._offsets[5]], &treeNodes[i]._trianglesStartIndex, sizeof(int));
		memcpy(&treeData[i * _treeDataDesc._alignOffset + _treeDataDesc._offsets[6]], &treeNodes[i]._numTriangles, sizeof(int));
	}

	_treeBuffer->bind(GL_SHADER_STORAGE_BUFFER);

	glBufferData(GL_SHADER_STORAGE_BUFFER, treeData.size(), &treeData[0], GL_STATIC_DRAW);

	std::vector<unsigned char> triangleData;

	triangleData.resize(triangles.size() * _triangleDataDesc._alignOffset);

	for (size_t i = 0; i < triangles.size(); i++) {
		memcpy(&triangleData[i * _triangleDataDesc._alignOffset + _triangleDataDesc._offsets[0]], &triangles[i]._v0._position, sizeof(Vec3f));
		memcpy(&triangleData[i * _triangleDataDesc._alignOffset + _triangleDataDesc._offsets[1]], &triangles[i]._v0._normal, sizeof(Vec3f));
		memcpy(&triangleData[i * _triangleDataDesc._alignOffset + _triangleDataDesc._offsets[2]], &triangles[i]._v0._texCoord, sizeof(Vec2f));
		memcpy(&triangleData[i * _triangleDataDesc._alignOffset + _triangleDataDesc._offsets[3]], &triangles[i]._v1._position, sizeof(Vec3f));
		memcpy(&triangleData[i * _triangleDataDesc._alignOffset + _triangleDataDesc._offsets[4]], &triangles[i]._v1._normal, sizeof(Vec3f));
		memcpy(&triangleData[i * _triangleDataDesc._alignOffset + _triangleDataDesc._offsets[5]], &triangles[i]._v1._texCoord, sizeof(Vec2f));
		memcpy(&triangleData[i * _triangleDataDesc._alignOffset + _triangleDataDesc._offsets[6]], &triangles[i]._v2._position, sizeof(Vec3f));
		memcpy(&triangleData[i * _triangleDataDesc._alignOffset + _triangleDataDesc._offsets[7]], &triangles[i]._v2._normal, sizeof(Vec3f));
		memcpy(&triangleData[i * _triangleDataDesc._alignOffset + _triangleDataDesc._offsets[8]], &triangles[i]._v2._texCoord, sizeof(Vec2f));
		memcpy(&triangleData[i * _triangleDataDesc._alignOffset + _triangleDataDesc._offsets[9]], &triangles[i]._materialIndex, sizeof(int));
	}

	_triangleBuffer->bind(GL_SHADER_STORAGE_BUFFER);

	glBufferData(GL_SHADER_STORAGE_BUFFER, triangleData.size(), &triangleData[0], GL_STATIC_DRAW);
}

void SceneObjectGI::compileMaterials() {
	std::vector<BufferMaterial> materials;

	std::vector<Texture2D*> textures;
	std::unordered_map<Texture2D*, size_t> textureIndexMap;

	for (size_t i = 0; i < _materials->size(); i++) {
		if ((*_materials)[i]._pDiffuseMap != nullptr && textureIndexMap.find((*_materials)[i]._pDiffuseMap) == textureIndexMap.end()) {
			textures.push_back((*_materials)[i]._pDiffuseMap);

			textureIndexMap[(*_materials)[i]._pDiffuseMap] = textures.size() - 1;
		}

		if ((*_materials)[i]._pSpecularMap != nullptr && textureIndexMap.find((*_materials)[i]._pSpecularMap) == textureIndexMap.end()) {
			textures.push_back((*_materials)[i]._pSpecularMap);

			textureIndexMap[(*_materials)[i]._pSpecularMap] = textures.size() - 1;
		}

		if ((*_materials)[i]._pShininessMap != nullptr && textureIndexMap.find((*_materials)[i]._pShininessMap) == textureIndexMap.end()) {
			textures.push_back((*_materials)[i]._pShininessMap);

			textureIndexMap[(*_materials)[i]._pShininessMap] = textures.size() - 1;
		}

		if ((*_materials)[i]._pEmissiveMap != nullptr && textureIndexMap.find((*_materials)[i]._pEmissiveMap) == textureIndexMap.end()) {
			textures.push_back((*_materials)[i]._pEmissiveMap);

			textureIndexMap[(*_materials)[i]._pEmissiveMap] = textures.size() - 1;
		}

		if ((*_materials)[i]._pNormalMap != nullptr && textureIndexMap.find((*_materials)[i]._pNormalMap) == textureIndexMap.end()) {
			textures.push_back((*_materials)[i]._pNormalMap);

			textureIndexMap[(*_materials)[i]._pNormalMap] = textures.size() - 1;
		}
	}

	for (size_t i = 0; i < _materials->size(); i++) {
		BufferMaterial material;

		material._diffuseColor = (*_materials)[i]._diffuseColor;
		material._specularColor = (*_materials)[i]._specularColor;
		material._shininess = (*_materials)[i]._shininess;
		material._emissiveColor = (*_materials)[i]._emissiveColor;

		material._diffuseMapIndexPlusOne = textureIndexMap[(*_materials)[i]._pDiffuseMap] + 1;
		material._specularMapIndexPlusOne = textureIndexMap[(*_materials)[i]._pSpecularMap] + 1;
		material._shininessMapIndexPlusOne = textureIndexMap[(*_materials)[i]._pShininessMap] + 1;
		material._emissiveMapIndexPlusOne = textureIndexMap[(*_materials)[i]._pEmissiveMap] + 1;

		material._normalMapIndexPlusOne = textureIndexMap[(*_materials)[i]._pNormalMap] + 1;

		materials.push_back(material);
	}

	// ------------------------------------- Compile Buffer -------------------------------------

	std::vector<unsigned char> materialData;

	materialData.resize(materials.size() * _materialDataDesc._alignOffset);

	for (size_t i = 0; i < materials.size(); i++) {
		memcpy(&materialData[i * _materialDataDesc._alignOffset + _materialDataDesc._offsets[0]], &materials[i]._diffuseColor, sizeof(Vec3f));
		memcpy(&materialData[i * _materialDataDesc._alignOffset + _materialDataDesc._offsets[1]], &materials[i]._specularColor, sizeof(float));
		memcpy(&materialData[i * _materialDataDesc._alignOffset + _materialDataDesc._offsets[2]], &materials[i]._shininess, sizeof(float));
		memcpy(&materialData[i * _materialDataDesc._alignOffset + _materialDataDesc._offsets[3]], &materials[i]._emissiveColor, sizeof(float));
		memcpy(&materialData[i * _materialDataDesc._alignOffset + _materialDataDesc._offsets[4]], &materials[i]._diffuseMapIndexPlusOne, sizeof(int));
		memcpy(&materialData[i * _materialDataDesc._alignOffset + _materialDataDesc._offsets[5]], &materials[i]._specularMapIndexPlusOne, sizeof(int));
		memcpy(&materialData[i * _materialDataDesc._alignOffset + _materialDataDesc._offsets[6]], &materials[i]._shininessMapIndexPlusOne, sizeof(int));
		memcpy(&materialData[i * _materialDataDesc._alignOffset + _materialDataDesc._offsets[7]], &materials[i]._emissiveMapIndexPlusOne, sizeof(int));
		memcpy(&materialData[i * _materialDataDesc._alignOffset + _materialDataDesc._offsets[8]], &materials[i]._normalMapIndexPlusOne, sizeof(int));
	}

	_materialBuffer->bind(GL_SHADER_STORAGE_BUFFER);

	glBufferData(GL_SHADER_STORAGE_BUFFER, materialData.size(), &materialData[0], GL_STATIC_DRAW);

	/*unsigned int maxWidth = 0;
	unsigned int maxHeight = 0;

	for (size_t i = 0; i < textures.size(); i++) {
	maxWidth = std::max(maxWidth, textures[i]->getWidth());
	maxHeight = std::max(maxHeight, textures[i]->getHeight());
	}*/

	size_t imageSize = _imageRescaleRT->getWidth() * _imageRescaleRT->getHeight();
	size_t allImagesSize = imageSize * textures.size();

	std::vector<sf::Color> imageData(allImagesSize);

	size_t imageDataIndex = 0;

	glDisable(GL_DEPTH_TEST);

	_imageRescaleRT->bind();
	_imageRescaleRT->setViewport();

	_renderImageShader->bind();

	for (size_t i = 0; i < textures.size(); i++) {
		_renderImageShader->setShaderTexture("d3dScene", textures[i]->getTextureID(), GL_TEXTURE_2D);

		_renderImageShader->bindShaderTextures();

		getRenderScene()->renderNormalizedQuad();

		glFlush();

		glReadPixels(0, 0, _imageRescaleRT->getWidth(), _imageRescaleRT->getHeight(), GL_RGBA, GL_UNSIGNED_BYTE, &imageData[imageDataIndex]);

		imageDataIndex += imageSize;
	}

	getRenderScene()->_gBuffer.bind();
	getRenderScene()->_gBuffer.setDrawEffect();
	getRenderScene()->_gBuffer.setReadEffect();
	getRenderScene()->_gBuffer.setViewport();

	glEnable(GL_DEPTH_TEST);

	glBindTexture(GL_TEXTURE_2D_ARRAY, _materialTextures);

	if (!imageData.empty())
		glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA8, _imageRescaleRT->getWidth(), _imageRescaleRT->getHeight(), textures.size(), 0, GL_RGBA, GL_UNSIGNED_BYTE, &imageData[0]);
	else
		glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA8, _imageRescaleRT->getWidth(), _imageRescaleRT->getHeight(), textures.size(), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
}

void SceneObjectGI::compileLights() {
	SceneObjectLighting* pLighting = static_cast<SceneObjectLighting*>(getScene()->getNamed("lighting").get());

	std::vector<BufferLight> lights;

	for (std::list<SceneObjectRef>::const_iterator cit = _pointLights.begin(); cit != _pointLights.end(); cit++) {
		const SceneObjectPointLight* pLight = static_cast<const SceneObjectPointLight*>(cit->get());

		BufferLight bufferLight;

		bufferLight._type = 0;
		bufferLight._position = pLight->getPosition();
		bufferLight._color = pLight->getColor();
		bufferLight._direction = Vec3f(1.0f, 0.0f, 0.0f);
		bufferLight._range = pLight->getRange();
		bufferLight._rangeInv = 1.0f / pLight->getRange();
		bufferLight._lightSpreadAngleCos = 1.0f;
		bufferLight._lightSpreadAngleCosFlipInv = 1.0f;
		bufferLight._lightExponent = 1.0f;

		lights.push_back(bufferLight);
	}

	for (std::list<SceneObjectRef>::const_iterator cit = _spotLights.begin(); cit != _spotLights.end(); cit++) {
		const SceneObjectSpotLight* pLight = static_cast<const SceneObjectSpotLight*>(cit->get());

		BufferLight bufferLight;

		bufferLight._type = 1;
		bufferLight._position = pLight->getPosition();
		bufferLight._color = pLight->getColor();
		bufferLight._direction = pLight->getDirection();
		bufferLight._range = pLight->getRange();
		bufferLight._rangeInv = 1.0f / pLight->getRange();
		bufferLight._lightSpreadAngleCos = pLight->getSpotAngleCos();
		bufferLight._lightSpreadAngleCosFlipInv = pLight->getSpotAngleCosFlipInv();
		bufferLight._lightExponent = pLight->getExponent();

		lights.push_back(bufferLight);
	}

	for (std::list<SceneObjectRef>::const_iterator cit = _directionalLights.begin(); cit != _directionalLights.end(); cit++) {
		const SceneObjectDirectionalLight* pLight = static_cast<const SceneObjectDirectionalLight*>(cit->get());

		BufferLight bufferLight;

		bufferLight._type = 2;
		bufferLight._position = Vec3f(0.0f, 0.0f, 0.0f);
		bufferLight._color = pLight->getColor();
		bufferLight._direction = pLight->getDirection();
		bufferLight._range = 1.0f;
		bufferLight._rangeInv = 1.0f;
		bufferLight._lightSpreadAngleCos = 1.0f;
		bufferLight._lightSpreadAngleCosFlipInv = 1.0f;
		bufferLight._lightExponent = 1.0f;

		lights.push_back(bufferLight);
	}

	for (std::list<SceneObjectRef>::const_iterator cit = _shadowedPointLights.begin(); cit != _shadowedPointLights.end(); cit++) {
		const SceneObjectPointLightShadowed* pLight = static_cast<const SceneObjectPointLightShadowed*>(cit->get());

		BufferLight bufferLight;

		bufferLight._type = 3;
		bufferLight._position = pLight->getPosition();
		bufferLight._color = pLight->getColor();
		bufferLight._direction = Vec3f(1.0f, 0.0f, 0.0f);
		bufferLight._range = pLight->getRange();
		bufferLight._rangeInv = 1.0f / pLight->getRange();
		bufferLight._lightSpreadAngleCos = 1.0f;
		bufferLight._lightSpreadAngleCosFlipInv = 1.0f;
		bufferLight._lightExponent = 1.0f;

		lights.push_back(bufferLight);
	}

	// TODO: ADD SHADOWED SPOT LIGHTS

	for (std::list<SceneObjectRef>::const_iterator cit = _shadowedDirectionalLights.begin(); cit != _shadowedDirectionalLights.end(); cit++) {
		const SceneObjectDirectionalLightShadowed* pLight = static_cast<const SceneObjectDirectionalLightShadowed*>(cit->get());

		BufferLight bufferLight;

		bufferLight._type = 5;
		bufferLight._position = Vec3f(0.0f, 0.0f, 0.0f);
		bufferLight._color = pLight->getColor();
		bufferLight._direction = Vec3f(1.0f, 0.0f, 0.0f);
		bufferLight._range = 1.0f;
		bufferLight._rangeInv = 1.0f;
		bufferLight._lightSpreadAngleCos = 1.0f;
		bufferLight._lightSpreadAngleCosFlipInv = 1.0f;
		bufferLight._lightExponent = 1.0f;

		lights.push_back(bufferLight);
	}

	_numLights = lights.size();

	std::vector<unsigned char> lightData;

	lightData.resize(lights.size() * _lightDataDesc._alignOffset);

	for (size_t i = 0; i < lights.size(); i++) {
		memcpy(&lightData[i * _lightDataDesc._alignOffset + _lightDataDesc._offsets[0]], &lights[i]._type, sizeof(int));
		memcpy(&lightData[i * _lightDataDesc._alignOffset + _lightDataDesc._offsets[1]], &lights[i]._position, sizeof(Vec3f));
		memcpy(&lightData[i * _lightDataDesc._alignOffset + _lightDataDesc._offsets[2]], &lights[i]._color, sizeof(Vec3f));
		memcpy(&lightData[i * _lightDataDesc._alignOffset + _lightDataDesc._offsets[3]], &lights[i]._direction, sizeof(Vec3f));
		memcpy(&lightData[i * _lightDataDesc._alignOffset + _lightDataDesc._offsets[4]], &lights[i]._range, sizeof(float));
		memcpy(&lightData[i * _lightDataDesc._alignOffset + _lightDataDesc._offsets[5]], &lights[i]._rangeInv, sizeof(float));
		memcpy(&lightData[i * _lightDataDesc._alignOffset + _lightDataDesc._offsets[6]], &lights[i]._lightSpreadAngleCos, sizeof(float));
		memcpy(&lightData[i * _lightDataDesc._alignOffset + _lightDataDesc._offsets[7]], &lights[i]._lightSpreadAngleCosFlipInv, sizeof(float));
		memcpy(&lightData[i * _lightDataDesc._alignOffset + _lightDataDesc._offsets[8]], &lights[i]._lightExponent, sizeof(float));
	}

	_lightBuffer->bind(GL_SHADER_STORAGE_BUFFER);

	if (lightData.empty())
		glBufferData(GL_SHADER_STORAGE_BUFFER, lightData.size(), nullptr, GL_STATIC_DRAW);
	else
		glBufferData(GL_SHADER_STORAGE_BUFFER, lightData.size(), &lightData[0], GL_STATIC_DRAW);
}