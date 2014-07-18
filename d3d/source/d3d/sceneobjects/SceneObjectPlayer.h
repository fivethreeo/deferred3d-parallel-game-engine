#pragma once

#include <d3d/scene/Scene.h>
#include <d3d/sceneobjects/input/SceneObjectBufferedInput.h>
#include <d3d/sceneobjects/physics/SceneObjectPhysicsWorld.h>
#include <d3d/physics/DynamicCharacterController.h>
#include <d3d/sceneobjects/gui/SceneObjectGUI.h>

class SceneObjectPlayer : public d3d::SceneObject {
private:
	d3d::SceneObjectRef _input;
	d3d::SceneObjectRef _gui;

	std::shared_ptr<d3d::DynamicCharacterController> _characterController;

	float _radius, _height, _mass, _stepHeight;

	sf::Vector2i _lastMousePosition;

public:
	float _sensitivity;

	float _noClipAcceleration;
	float _noClipDeceleration;

	float _noClipRunMultiplier;

	float _acceleration;
	float _deceleration;

	float _runMultiplier;

	float _cameraHeightOffset;

	float _angleX, _angleY;

	d3d::Vec3f _noClipVelocity;

	bool _allowNoclipChange;

	bool _acceptingInput;

	SceneObjectPlayer();

	void setNoClip(bool noClip);

	// Inherited from SceneObject
	void onAdd();
	void update(float dt);
	void synchronousUpdate(float dt);

	SceneObjectPlayer* copyFactory() {
		return new SceneObjectPlayer(*this);
	}
};