#version 400

uniform sampler2D d3dScene;
uniform vec2 d3dSizeInv;

uniform float d3dBlurSize;

out vec4 d3dOutputColor;

void main() {
	vec3 sum = vec3(0.0);

	vec2 coord = gl_FragCoord.xy * d3dSizeInv;

	sum += texture(d3dScene, vec2(coord.x, coord.y - 4.0 * d3dBlurSize)).rgb * 0.05;
	sum += texture(d3dScene, vec2(coord.x, coord.y - 3.0 * d3dBlurSize)).rgb * 0.09;
	sum += texture(d3dScene, vec2(coord.x, coord.y - 2.0 * d3dBlurSize)).rgb * 0.12;
	sum += texture(d3dScene, vec2(coord.x, coord.y - d3dBlurSize)).rgb * 0.15;
	sum += texture(d3dScene, vec2(coord.x, coord.y)).rgb * 0.16;
	sum += texture(d3dScene, vec2(coord.x, coord.y + d3dBlurSize)).rgb * 0.15;
	sum += texture(d3dScene, vec2(coord.x, coord.y + 2.0 * d3dBlurSize)).rgb * 0.12;
	sum += texture(d3dScene, vec2(coord.x, coord.y + 3.0 * d3dBlurSize)).rgb * 0.09;
	sum += texture(d3dScene, vec2(coord.x, coord.y + 4.0 * d3dBlurSize)).rgb * 0.05;
 
	d3dOutputColor = vec4(sum, 1.0);
}