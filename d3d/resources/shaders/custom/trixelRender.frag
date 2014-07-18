#version 400

smooth in vec3 viewPosition;
smooth in vec3 viewNormal;
in vec3 color;

layout (location = 0) out vec4 d3dOutputPosition;
layout (location = 1) out vec4 d3dOutputNormal;
layout (location = 2) out vec4 d3dOutputColor;

void main() {
	d3dOutputPosition = vec4(viewPosition, 0.15);
	d3dOutputNormal = vec4(viewNormal, 96.0);
	d3dOutputColor = vec4(color, 0.0);
}