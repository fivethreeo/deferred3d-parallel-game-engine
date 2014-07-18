#pragma once

#include <d3d/constructs/Vec3f.h>
#include <d3d/constructs/Vec2f.h>
#include <d3d/constructs/AABB3D.h>
#include <d3d/rendering/material/Material.h>

namespace d3d {
	class Form {
	public:
		virtual ~Form() {};

		virtual bool rayTest(const Vec3f &start, const Vec3f &dir, float &t, Vec3f &hitPos) = 0;
		virtual Vec3f getBarycentricCoords(const Vec3f &hitPos) = 0;
		virtual Vec2f getImageCoord(const Vec3f &barycentricCoords) = 0;
		virtual Vec3f getNormal(const Vec3f &barycentricCoords) = 0;
		virtual AABB3D getAABB() = 0;
		virtual Vec3f getCenter() = 0;
		virtual float getArea() = 0;
		virtual unsigned int getMaterialIndex() = 0;
	};
}