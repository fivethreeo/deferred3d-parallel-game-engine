#pragma once

#include <d3d/scene/Scene.h>

#include <d3d/constructs/Matrix4x4f.h>

#include <d3d/rendering/model/StaticModelOBJ.h>

#include <iostream>

class SceneObjectPropLOD : public d3d::SceneObject {
private:
	std::vector<d3d::StaticModelOBJ*> _pModelsOBJ;

	d3d::SceneObjectRef _batcherRef;

public:
	d3d::Matrix4x4f _transform;

	float _lodSwitchDistance;

	SceneObjectPropLOD()
		: _lodSwitchDistance(20.0f)
	{
		_renderMask = 0xffff;
	}

	bool create(const std::vector<std::string> &fileNames);

	void calculateAABB();

	// Inherited from SceneObject
	void onAdd();
	void deferredRender();

	SceneObject* copyFactory() {
		return new SceneObjectPropLOD(*this);
	}
};