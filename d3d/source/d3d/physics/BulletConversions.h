#pragma once

#include <d3d/constructs/Vec3f.h>
#include <d3d/constructs/Quaternion.h>

#include <LinearMath/btVector3.h>
#include <LinearMath/btQuaternion.h>

d3d::Vec3f cons(const btVector3 &vec);
d3d::Quaternion cons(const btQuaternion &quat);

btVector3 bt(const d3d::Vec3f &vec);
btQuaternion bt(const d3d::Quaternion &quat);