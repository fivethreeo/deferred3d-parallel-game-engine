#pragma once

#include <d3d/bvh/BVHNode.h>
#include <d3d/system/Uncopyable.h>

#include <d3d/constructs/AABB3D.h>

#include <d3d/geometry/FormTriangle.h>

#include <array>

namespace d3d {
	class BVHTree : public Uncopyable {
	private:
		std::shared_ptr<BVHNode> _pRootNode;

	public:
		unsigned int _maxSplitsAfterNoTriangleReduction;

		BVHTree();

		void create(const AABB3D &rootRegion);
		void add(FormTriangle &triangle);

		void compile() {
			assert(_pRootNode != nullptr);

			// TODO: Remove useless (empty) nodes
			_pRootNode->split(0);
		}

		bool rayTrace(const Vec3f &origin, const Vec3f &direction,
			FormTriangle &triangle, Vec3f &point, Vec3f &normal);

		const BVHNode* getRootNode() const {
			return _pRootNode.get();
		}

		friend BVHNode;
	};
}