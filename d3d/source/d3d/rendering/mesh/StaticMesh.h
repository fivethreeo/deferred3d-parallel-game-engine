#pragma once

#include <d3d/constructs/Vec3f.h>
#include <d3d/constructs/Vec2f.h>

#include <d3d/rendering/SFMLOGL.h>
#include <d3d/rendering/bufferobjects/VBO.h>
#include <d3d/rendering/material/Material.h>

#define D3D_STATIC_MESH_INDEX_TYPE_ENUM GL_UNSIGNED_SHORT

namespace d3d {
	typedef GLushort staticMeshIndexType;

	class StaticMesh {
	public:
		struct Vertex {
			Vec3f _position;
			Vec3f _normal;
			Vec2f _texCoord;
		};
	private:
		VBO _interleavedBuffer;
		VBO _indexBuffer;

	public:
		std::vector<Vertex> _vertices;
		std::vector<staticMeshIndexType> _indices;

		size_t _numIndices;

		StaticMesh()
			: _numIndices(0)
		{}

		void create(bool useBuffer = true);

		void updateBuffers();

		void clearArrays() {
			_vertices.clear();
			_indices.clear();
		}

		void renderFromBuffers();
		void renderFromArrays();

		void setAttributes();
		void renderFromAttributes();

		void render() {
			if (hasBuffer())
				renderFromBuffers();
			else
				renderFromArrays();
		}

		bool hasBuffer() const {
			return _interleavedBuffer.created();
		}
	};
}