#pragma once

#include <d3d/scene/RenderScene.h>

#include <d3d/sceneobjects/SceneObjectProp.h>
#include <d3d/sceneobjects/physics/SceneObjectPhysicsWorld.h>

class SceneObjectBox : public d3d::SceneObject {
private:
	// Rendering
	d3d::StaticModelOBJ* _pModelOBJ;

	// Physics
	d3d::SceneObjectRef _physicsWorld;

	std::shared_ptr<btCollisionShape> _pCollisionShape;
	std::shared_ptr<btDefaultMotionState> _pMotionState;
	std::shared_ptr<btRigidBody> _pRigidBody;
public:
	SceneObjectBox() {
		_renderMask = 0xffff;
	}

	bool create(const std::string &modelFileName, const d3d::Vec3f &startPosition, const d3d::Quaternion &startRotation, float mass, float restitution, float friction);

	// Inherited from SceneObject
	void onAdd();
	void update(float dt);
	void deferredRender();
	void onDestroy();

	void setPosition(const d3d::Vec3f &position) {
		_pRigidBody->getWorldTransform().setOrigin(bt(position));
	}

	SceneObjectBox* copyFactory() {
		return new SceneObjectBox(*this);
	}
};

