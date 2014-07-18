#version 400

uniform sampler2D d3dScene;

uniform vec2 d3dSizeInv;

out vec4 d3dOutputColor;

void main() {
	vec2 coord = gl_FragCoord.xy * d3dSizeInv;

	vec3 color = texture(d3dScene, coord).rgb;

	d3dOutputColor = vec4(color, dot(color, vec3(0.299, 0.587, 0.114)));
}