#include <d3d/rendering/mesh/StaticPositionMesh.h>

using namespace d3d;

void StaticPositionMesh::create(bool useBuffer) {
	if (useBuffer) {
		_positionBuffer.create();
		_indexBuffer.create();
	}
}

void StaticPositionMesh::updateBuffers() {
	assert(_positionBuffer.created());
	assert(_indexBuffer.created());

	_numIndices = _indices.size();

	_positionBuffer.bind(GL_ARRAY_BUFFER);

	glBufferData(GL_ARRAY_BUFFER, sizeof(Vec3f) * _vertices.size(), &_vertices[0], GL_STATIC_DRAW);

	_indexBuffer.bind(GL_ELEMENT_ARRAY_BUFFER);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(staticPositionMeshIndexType) * _indices.size(), &_indices[0], GL_STATIC_DRAW);
}

void StaticPositionMesh::renderFromBuffers() {
	glDisableVertexAttribArray(D3D_ATTRIB_NORMAL);
	glDisableVertexAttribArray(D3D_ATTRIB_TEXCOORD);

	_positionBuffer.bind(GL_ARRAY_BUFFER);

	glVertexAttribPointer(D3D_ATTRIB_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3f), 0);

	_indexBuffer.bind(GL_ELEMENT_ARRAY_BUFFER);

	glDrawElements(GL_TRIANGLES, _numIndices, D3D_STATIC_POSITION_MESH_INDEX_TYPE_ENUM, nullptr);

	glEnableVertexAttribArray(D3D_ATTRIB_NORMAL);
	glEnableVertexAttribArray(D3D_ATTRIB_TEXCOORD);
}

void StaticPositionMesh::renderFromArrays() {
	glDisableVertexAttribArray(D3D_ATTRIB_NORMAL);
	glDisableVertexAttribArray(D3D_ATTRIB_TEXCOORD);

	glVertexAttribPointer(D3D_ATTRIB_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3f), &_vertices[0]);

	VBO::unbind(GL_ARRAY_BUFFER);
	VBO::unbind(GL_ELEMENT_ARRAY_BUFFER);

	glDrawElements(GL_TRIANGLES, _indices.size(), D3D_STATIC_POSITION_MESH_INDEX_TYPE_ENUM, &_indices[0]);

	glEnableVertexAttribArray(D3D_ATTRIB_NORMAL);
	glEnableVertexAttribArray(D3D_ATTRIB_TEXCOORD);
}

void StaticPositionMesh::setAttributes() {
	if (hasBuffer()) {
		_positionBuffer.bind(GL_ARRAY_BUFFER);

		glVertexAttribPointer(D3D_ATTRIB_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3f), 0);

		_indexBuffer.bind(GL_ELEMENT_ARRAY_BUFFER);
	}
	else {
		VBO::unbind(GL_ARRAY_BUFFER);
		VBO::unbind(GL_ELEMENT_ARRAY_BUFFER);

		glVertexAttribPointer(D3D_ATTRIB_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3f), &_vertices[0]);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}

void StaticPositionMesh::renderFromAttributes() {
	if (hasBuffer())
		glDrawElements(GL_TRIANGLES, _numIndices, D3D_STATIC_POSITION_MESH_INDEX_TYPE_ENUM, nullptr);
	else
		glDrawElements(GL_TRIANGLES, _indices.size(), D3D_STATIC_POSITION_MESH_INDEX_TYPE_ENUM, &_indices[0]);
}