#pragma once

#include <d3d/scene/Scene.h>

#include <d3d/sceneobjects/input/SceneObjectBufferedInput.h>

class SceneObjectFloatingCamera : public d3d::SceneObject {
private:
	d3d::SceneObjectRef _input;

public:
	float _sensitivity;
	float _acceleration;
	float _deceleration;

	float _runMultiplier;

	float _angleX, _angleY;

	d3d::Vec3f _velocity;

	bool _acceptingInput;

	SceneObjectFloatingCamera();

	// Inherited from SceneObject
	void onAdd();
	void update(float dt);

	SceneObject* copyFactory() {
		return new SceneObjectFloatingCamera(*this);
	}
};