#include <d3d/sceneobjects/SceneObjectPlayer.h>

#include <d3d/scene/RenderScene.h>

#include <d3d/util/Math.h>

SceneObjectPlayer::SceneObjectPlayer() 
: _sensitivity(0.01f), _angleX(0.0f), _angleY(0.0f), _noClipVelocity(0.0f, 0.0f, 0.0f),
_noClipAcceleration(120.0f), _noClipDeceleration(10.0f), _noClipRunMultiplier(8.0f),
_acceleration(20.0f), _deceleration(4.0f), _runMultiplier(2.5f),
_radius(0.5f), _height(2.0f), _mass(70.0f), _stepHeight(0.2f),
_cameraHeightOffset(1.0f),
_allowNoclipChange(true), _acceptingInput(false),
_lastMousePosition(128, 128)
{}

void SceneObjectPlayer::setNoClip(bool noClip) {
	if (noClip)
		_characterController.reset();
	else {
		assert(getScene() != nullptr);

		d3d::SceneObjectRef physicsWorld = getScene()->getNamed("physWrld");

		assert(physicsWorld.isAlive());

		_characterController.reset(new d3d::DynamicCharacterController(getScene(), static_cast<d3d::SceneObjectPhysicsWorld*>(physicsWorld.get()), getScene()->_logicCamera._position + d3d::Vec3f(0.0f, -_cameraHeightOffset, 0.0f), _radius, _height, _mass, _stepHeight));
	}
}

void SceneObjectPlayer::onAdd() {
	_input = getScene()->getNamed("buffIn");

	assert(_input.isAlive());

	_gui = getScene()->getNamed("gui");

	assert(_gui.isAlive());
}

void SceneObjectPlayer::update(float dt) {
	d3d::SceneObjectBufferedInput* pBufferedInput = static_cast<d3d::SceneObjectBufferedInput*>(_input.get());

	if (pBufferedInput->isKeyPressed(sf::Keyboard::Escape)) {
		_acceptingInput = !_acceptingInput;
		
		if (_acceptingInput) {
			_lastMousePosition = sf::Mouse::getPosition(*getRenderScene()->getRenderWindow());

			sf::Mouse::setPosition(sf::Vector2i(128, 128), *getRenderScene()->getRenderWindow());
		}
		else
			sf::Mouse::setPosition(_lastMousePosition, *getRenderScene()->getRenderWindow());
	}

	if (_acceptingInput) {
		sf::Vector2i mousePosition = sf::Mouse::getPosition(*getRenderScene()->getRenderWindow()) - sf::Vector2i(128, 128);
		sf::Mouse::setPosition(sf::Vector2i(128, 128), *getRenderScene()->getRenderWindow());
		
		_angleX -= mousePosition.x * _sensitivity;
		_angleY -= mousePosition.y * _sensitivity;

		_angleX = std::fmodf(_angleX, d3d::_piTimes2);

		if (_angleY < -d3d::_piOver2)
			_angleY = -d3d::_piOver2;
		else if (_angleY > d3d::_piOver2)
			_angleY = d3d::_piOver2;

		getRenderScene()->_logicCamera._rotation = d3d::Quaternion(_angleX, d3d::Vec3f(0.0f, 1.0f, 0.0f)) * d3d::Quaternion(_angleY, d3d::Vec3f(1.0f, 0.0f, 0.0f));

		if (_allowNoclipChange)
		if (pBufferedInput->isKeyPressed(sf::Keyboard::N))
			setNoClip(_characterController != nullptr);

		if (_characterController == nullptr) {
			float accel = sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) ? _noClipAcceleration * _noClipRunMultiplier : _noClipAcceleration;

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
				_noClipVelocity += getRenderScene()->_logicCamera._rotation * d3d::Vec3f(0.0f, 0.0f, -accel * dt);
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
				_noClipVelocity += getRenderScene()->_logicCamera._rotation * d3d::Vec3f(0.0f, 0.0f, accel * dt);

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
				_noClipVelocity += getRenderScene()->_logicCamera._rotation * d3d::Vec3f(accel * dt, 0.0f, 0.0f);
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
				_noClipVelocity += getRenderScene()->_logicCamera._rotation * d3d::Vec3f(-accel * dt, 0.0f, 0.0f);

			_noClipVelocity += -_noClipDeceleration * _noClipVelocity * dt;

			getRenderScene()->_logicCamera._position += _noClipVelocity * dt;
		}
		else {
			float accel = sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) ? _acceleration * _runMultiplier : _acceleration;

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
				_characterController->walk(getRenderScene()->_logicCamera._rotation * d3d::Vec3f(0.0f, 0.0f, -accel * dt));
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
				_characterController->walk(getRenderScene()->_logicCamera._rotation * d3d::Vec3f(0.0f, 0.0f, accel * dt));

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
				_characterController->walk(getRenderScene()->_logicCamera._rotation * d3d::Vec3f(accel * dt, 0.0f, 0.0f));
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
				_characterController->walk(getRenderScene()->_logicCamera._rotation * d3d::Vec3f(-accel * dt, 0.0f, 0.0f));

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
				_characterController->jump();

			_characterController->_deceleration = _deceleration;

			_characterController->update(dt);

			getRenderScene()->_logicCamera._position = _characterController->getPosition() + d3d::Vec3f(0.0f, _cameraHeightOffset, 0.0f);
		}
	}
}

void SceneObjectPlayer::synchronousUpdate(float dt) {
	SceneObjectGUI* pGUI = static_cast<SceneObjectGUI*>(_gui.get());

	pGUI->_enabled = !_acceptingInput;

	getRenderScene()->getRenderWindow()->setMouseCursorVisible(!_acceptingInput);
}