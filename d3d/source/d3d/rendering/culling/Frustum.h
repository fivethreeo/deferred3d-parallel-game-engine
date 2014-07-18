#pragma once

#include <d3d/constructs/Vec3f.h>
#include <d3d/constructs/AABB3D.h>
#include <d3d/constructs/PlaneEquationf.h>
#include <d3d/constructs/Matrix4x4f.h>

namespace d3d {
	class Frustum {
	private:
		static enum PlaneOrientation {
			planeTop = 0, planeBottom, planeLeft, planeRight, planeNear, planeFar
		};

		std::array<PlaneEquationf, 6> _planes;

		std::array<Vec3f, 8> _corners;

	public:
		static enum ObjectLocation {
			outside, intersect, inside
		};

		void extractFromMatrix(const Matrix4x4f &camera); // ViewProjection

		// Tests
		ObjectLocation testAABB(const AABB3D &aabb) const;
		bool testAABBOutside(const AABB3D &aabb) const;

		void calculateCorners(const Matrix4x4f &cameraInverse); // Inverse ViewProjection

		Vec3f getCorner(size_t index) const {
			return _corners[index];
		}
	};
}