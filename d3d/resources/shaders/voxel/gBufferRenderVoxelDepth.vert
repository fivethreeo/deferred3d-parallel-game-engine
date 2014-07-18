#version 400

uniform mat4 d3dViewModel;
uniform mat4 d3dProjectionViewModel;
uniform mat3 d3dNormal;

layout(location = 0) in vec3 positionIn;

void main() {
	gl_Position = d3dProjectionViewModel * vec4(positionIn, 1.0);
}
