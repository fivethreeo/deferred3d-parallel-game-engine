#pragma once

#include <d3d/scene/Scene.h>

#include <btBulletDynamicsCommon.h>
#include <btBulletCollisionCommon.h>

#include <d3d/physics/BulletConversions.h>

#include <BulletCollision/CollisionDispatch/btGhostObject.h>

#include <BulletDynamics/Featherstone/btMultiBodyDynamicsWorld.h>
#include <BulletDynamics/Featherstone/btMultiBodyConstraintSolver.h>

namespace d3d {
	class SceneObjectPhysicsWorld : public d3d::SceneObject {
	public:
		int _steps;
		int _subSteps;

		std::shared_ptr<btBroadphaseInterface> _pBroadphase;
		std::shared_ptr<btDefaultCollisionConfiguration> _pCollisionConfiguration;
		std::shared_ptr<btCollisionDispatcher> _pDispatcher;

		std::shared_ptr<btMultiBodyConstraintSolver> _pSolver;

		std::shared_ptr<btMultiBodyDynamicsWorld> _pDynamicsWorld;

		std::shared_ptr<btGhostPairCallback> _pGhostPairCallBack;

		SceneObjectPhysicsWorld()
			: _steps(1), _subSteps(1)
		{}

		// Inherited from SceneObject
		void onQueue();
		void onDestroy();
		void synchronousUpdate(float dt);

		SceneObject* copyFactory() {
			return new SceneObjectPhysicsWorld(*this);
		}
	};
}