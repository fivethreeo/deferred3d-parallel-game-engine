#include <d3d/rendering/Camera.h>

using namespace d3d;

void Camera::extractFrustum() {
	_frustum.extractFromMatrix(_projectionViewMatrix);
	_frustum.calculateCorners(_projectionViewInverseMatrix);
}

void Camera::fullUpdate() {
	updateViewMatrix();
	updateViewInverseMatrix();
	updateNormalMatrix();
	updateProjectionViewMatrix();
	updateProjectionViewInverseMatrix();

	extractFrustum();
}