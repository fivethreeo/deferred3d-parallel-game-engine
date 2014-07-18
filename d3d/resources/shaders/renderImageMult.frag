#version 400

uniform sampler2D d3dScene;
uniform sampler2D d3dColor;
uniform vec2 d3dGBufferSizeInv;

out vec4 d3dOutputColor;

void main() {
	vec2 coord = gl_FragCoord.xy * d3dGBufferSizeInv;
	d3dOutputColor = texture(d3dColor, coord) * texture(d3dScene, coord);
}