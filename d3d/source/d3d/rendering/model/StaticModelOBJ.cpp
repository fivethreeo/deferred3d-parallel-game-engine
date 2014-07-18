#include <d3d/rendering/model/StaticModelOBJ.h>

using namespace d3d;

bool StaticModelOBJ::createAsset(const std::string &fileName) {
	_textureManager.create(Texture2D::assetFactory);

	return _model.loadFromOBJ(fileName, _textureManager, _aabb, true, false);
}