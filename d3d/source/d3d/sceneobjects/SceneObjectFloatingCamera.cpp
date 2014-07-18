#include <d3d/sceneobjects/SceneObjectFloatingCamera.h>

#include <d3d/scene/RenderScene.h>

#include <d3d/util/Math.h>

#include <d3d/rendering/lighting/SceneObjectPointLight.h>

SceneObjectFloatingCamera::SceneObjectFloatingCamera()
: _sensitivity(0.01f), _angleX(0.0f), _angleY(0.0f), _velocity(0.0f, 0.0f, 0.0f),
_acceleration(120.0f), _deceleration(10.0f), _runMultiplier(8.0f),
_acceptingInput(false)
{}

void SceneObjectFloatingCamera::onAdd() {
	_input = getScene()->getNamed("buffIn");
}

void SceneObjectFloatingCamera::update(float dt) {
	d3d::SceneObjectBufferedInput* pBufferedInput = static_cast<d3d::SceneObjectBufferedInput*>(_input.get());

	if (pBufferedInput->isKeyPressed(sf::Keyboard::Escape)) {
		_acceptingInput = !_acceptingInput;

		getRenderScene()->getRenderWindow()->setMouseCursorVisible(!_acceptingInput);
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

		float accel = sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) ? _acceleration * _runMultiplier : _acceleration;

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
			_velocity += getRenderScene()->_logicCamera._rotation * d3d::Vec3f(0.0f, 0.0f, -accel * dt);
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
			_velocity += getRenderScene()->_logicCamera._rotation * d3d::Vec3f(0.0f, 0.0f, accel * dt);

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
			_velocity += getRenderScene()->_logicCamera._rotation * d3d::Vec3f(accel * dt, 0.0f, 0.0f);
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
			_velocity += getRenderScene()->_logicCamera._rotation * d3d::Vec3f(-accel * dt, 0.0f, 0.0f);
	}

	_velocity += -_deceleration * _velocity * dt;

	getRenderScene()->_logicCamera._position += _velocity * dt;
}