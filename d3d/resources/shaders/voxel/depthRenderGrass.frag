#version 400

uniform sampler2D d3dDiffuseMap;

uniform sampler2D d3dNoiseMap;

uniform float d3dShininess = 96.0;

smooth in vec2 texCoord;
smooth in float fadeFactor;

out vec4 d3dOutputColor;

void main() {
	float noise = (texture(d3dNoiseMap, texCoord * 2.0).r + 1.0) * fadeFactor;

	if(noise > 0.9)
		discard;

	// Alpha testing
	vec4 diffuseTexColor = texture(d3dDiffuseMap, texCoord);

	if(diffuseTexColor.a < 0.5)
		discard;

	d3dOutputColor = vec4(1.0, 1.0, 1.0, 1.0);
}