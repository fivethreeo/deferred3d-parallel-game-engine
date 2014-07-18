#version 400

uniform sampler2D d3dGBufferPosition;

uniform vec2 d3dSizeInv;

uniform vec3 d3dFogColor;
uniform float d3dFogStartDistance;

out vec4 d3dOutputColor;

void main() {
	vec2 coord = gl_FragCoord.xy * d3dSizeInv;

	// Get color
	d3dOutputColor = vec4(d3dFogColor * max(0.0, -texture(d3dGBufferPosition, coord).z - d3dFogStartDistance), 1.0);
}