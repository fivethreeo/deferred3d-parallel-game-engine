#version 400

uniform sampler2D d3dScene;
uniform vec2 d3dGBufferSizeInv;

out vec4 d3dOutputColor;

void main() {
	d3dOutputColor = texture(d3dScene, gl_FragCoord.xy * d3dGBufferSizeInv);
}