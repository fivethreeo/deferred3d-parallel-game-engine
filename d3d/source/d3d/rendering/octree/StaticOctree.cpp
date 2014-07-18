#include <d3d/rendering/octree/StaticOctree.h>

#include <d3d/scene/SceneObject.h>

#include <assert.h>

using namespace d3d;

void StaticOctree::add(const SceneObjectRef &oc) {
	std::unique_lock<std::recursive_mutex> lock(_mutex);

	assert(created());

	setOctree(oc);

	// If the occupant fits in the root node
	if(_pRootNode->getRegion().contains(oc->getAABB()))
		_pRootNode->add(oc);
	else
		_outsideRoot.insert(oc);
}