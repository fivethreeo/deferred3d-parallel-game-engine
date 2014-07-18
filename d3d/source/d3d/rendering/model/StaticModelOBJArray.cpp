#include <d3d/rendering/model/StaticModelOBJArray.h>

using namespace d3d;

bool StaticModelOBJArray::createAsset(const std::string &fileName) {
	_textureManager.create(Texture2D::assetFactory);

	return _model.loadFromOBJ(fileName, _textureManager, _aabb, false, false);
}