#version 400

uniform sampler2D d3dGBufferPosition;
uniform sampler2D d3dGBufferColor;

uniform vec2 d3dGBufferSizeInv;

out vec4 d3dOutputColor;

void main() {
	vec2 texCoord = gl_FragCoord.xy * d3dGBufferSizeInv;
	
	d3dOutputColor = texture(d3dGBufferColor, texCoord) * vec4(texture(d3dGBufferPosition, texCoord).a);
}