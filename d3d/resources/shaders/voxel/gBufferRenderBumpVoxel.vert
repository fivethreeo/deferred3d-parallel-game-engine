#version 400

uniform mat4 d3dViewModel;
uniform mat4 d3dProjectionViewModel;
uniform mat3 d3dNormal;

layout(location = 0) in vec3 positionIn;
layout(location = 1) in vec3 normalIn;

smooth out vec3 viewPosition;
smooth out vec3 viewNormal;
smooth out vec3 worldPosition;
smooth out vec3 worldNormal;

void main() {
	viewPosition = (d3dViewModel * vec4(positionIn, 1.0)).xyz;
	viewNormal = normalize(d3dNormal * normalIn);

	worldPosition = positionIn;
	worldNormal = normalIn;

	gl_Position = d3dProjectionViewModel * vec4(positionIn, 1.0);
}
