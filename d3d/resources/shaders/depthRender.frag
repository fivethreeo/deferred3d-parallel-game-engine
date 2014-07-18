#version 400

uniform sampler2D d3dDiffuseMap;

smooth in vec2 texCoord;

out vec4 d3dOutputColor;

void main() {
	if (texture(d3dDiffuseMap, texCoord).a < 0.5)
		discard;

	d3dOutputColor = vec4(1.0);
}