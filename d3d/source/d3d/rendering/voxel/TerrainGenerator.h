#pragma once

#include <d3d/rendering/voxel/VoxelTerrain.h>
#include <d3d/rendering/voxel/VoxelChunk.h>

namespace d3d {
	void terrainGenerator0(d3d::VoxelChunk &chunk, int seed);
	void terrainGeneratorFlatlands(d3d::VoxelChunk &chunk, int seed);
}