#pragma once

#include <d3d/scene/Scene.h>

#include <d3d/constructs/Matrix4x4f.h>

#include <d3d/rendering/model/StaticModelOBJ.h>

#include <iostream>

class SceneObjectProp : public d3d::SceneObject {
private:
	d3d::StaticModelOBJ* _pModelOBJ;

	d3d::SceneObjectRef _batcherRef;

public:
	d3d::Matrix4x4f _transform;

	SceneObjectProp() {
		_renderMask = 0xffff;
	}

	bool create(const std::string &fileName);

	void calculateAABB();

	// Inherited from SceneObject
	void onAdd();
	void deferredRender();

	d3d::StaticModelOBJ* getModel() {
		return _pModelOBJ;
	}

	SceneObject* copyFactory() {
		return new SceneObjectProp(*this);
	}
};