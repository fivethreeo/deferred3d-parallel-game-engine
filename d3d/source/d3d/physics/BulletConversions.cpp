#include <d3d/physics/BulletConversions.h>

d3d::Vec3f cons(const btVector3 &vec) {
	return d3d::Vec3f(vec.getX(), vec.getY(), vec.getZ());
}

d3d::Quaternion cons(const btQuaternion &quat) {
	return d3d::Quaternion(quat.getW(), quat.getX(), quat.getY(), quat.getZ());
}

btVector3 bt(const d3d::Vec3f &vec) {
	return btVector3(vec.x, vec.y, vec.z);
}

btQuaternion bt(const d3d::Quaternion &quat) {
	return btQuaternion(quat.x, quat.y, quat.z, quat.w);
}