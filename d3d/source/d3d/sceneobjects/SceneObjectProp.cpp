#include <d3d/sceneobjects/SceneObjectProp.h>

#include <d3d/rendering/model/SceneObjectStaticModelBatcher.h>

#include <d3d/rendering/shader/Shader.h>

bool SceneObjectProp::create(const std::string &fileName) {
	assert(getScene() != nullptr);

	std::shared_ptr<d3d::Asset> asset;

	if (!getScene()->getAssetManager("MOBJ", d3d::StaticModelOBJ::assetFactory)->getAsset(fileName, asset))
		return false;

	_pModelOBJ = static_cast<d3d::StaticModelOBJ*>(asset.get());

	_pModelOBJ->_model.genMipMaps();

	_transform = d3d::Matrix4x4f::identityMatrix();

	return true;
}

void SceneObjectProp::calculateAABB() {
	_aabb = _pModelOBJ->getAABB().getTransformedAABB(_transform);

	if (getScene() != nullptr)
		updateAABB();
}

void SceneObjectProp::onAdd() {
	_batcherRef = getScene()->getNamed("smb");

	assert(_batcherRef.isAlive());
}

void SceneObjectProp::deferredRender() {
	d3d::SceneObjectStaticModelBatcher* pBatcher = static_cast<d3d::SceneObjectStaticModelBatcher*>(_batcherRef.get());

	_pModelOBJ->render(pBatcher, _transform);
}