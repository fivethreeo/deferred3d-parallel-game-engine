#version 400

uniform sampler2D d3dScene;
uniform sampler2D d3dGBufferPosition;
uniform sampler2D d3dGBufferNormal;
uniform vec2 d3dSizeInv;

uniform float d3dBlurSize;

const float d3dEdgeFactor = 0.5;

out vec4 d3dOutputColor;

void main() {
	vec3 sum = vec3(0.0);

	vec2 coord = gl_FragCoord.xy * d3dSizeInv;

	vec3 centerNormal = texture(d3dGBufferNormal, coord).rgb;

	vec2 coord0 = vec2(coord.x - 4.0 * d3dBlurSize, coord.y);
	vec2 coord1 = vec2(coord.x - 3.0 * d3dBlurSize, coord.y);
	vec2 coord2 = vec2(coord.x - 2.0 * d3dBlurSize, coord.y);
	vec2 coord3 = vec2(coord.x - d3dBlurSize, coord.y);
	vec2 coord4 = vec2(coord.x, coord.y);
	vec2 coord5 = vec2(coord.x + d3dBlurSize, coord.y);
	vec2 coord6 = vec2(coord.x + 2.0 * d3dBlurSize, coord.y);
	vec2 coord7 = vec2(coord.x + 3.0 * d3dBlurSize, coord.y);
	vec2 coord8 = vec2(coord.x + 4.0 * d3dBlurSize, coord.y);

	vec3 centerColor = texture(d3dScene, coord4).rgb;

	sum += dot(centerNormal, texture(d3dGBufferNormal, coord0).rgb) > d3dEdgeFactor ? (texture(d3dScene, coord0).rgb * 0.05) : (centerColor * 0.05);
	sum += dot(centerNormal, texture(d3dGBufferNormal, coord1).rgb) > d3dEdgeFactor ? (texture(d3dScene, coord1).rgb * 0.09) : (centerColor * 0.09);
	sum += dot(centerNormal, texture(d3dGBufferNormal, coord2).rgb) > d3dEdgeFactor ? (texture(d3dScene, coord2).rgb * 0.12) : (centerColor * 0.12);
	sum += dot(centerNormal, texture(d3dGBufferNormal, coord3).rgb) > d3dEdgeFactor ? (texture(d3dScene, coord3).rgb * 0.15) : (centerColor * 0.15);
	sum += dot(centerNormal, texture(d3dGBufferNormal, coord4).rgb) > d3dEdgeFactor ? (texture(d3dScene, coord4).rgb * 0.16) : (centerColor * 0.16);
	sum += dot(centerNormal, texture(d3dGBufferNormal, coord5).rgb) > d3dEdgeFactor ? (texture(d3dScene, coord5).rgb * 0.15) : (centerColor * 0.15);
	sum += dot(centerNormal, texture(d3dGBufferNormal, coord6).rgb) > d3dEdgeFactor ? (texture(d3dScene, coord6).rgb * 0.12) : (centerColor * 0.12);
	sum += dot(centerNormal, texture(d3dGBufferNormal, coord7).rgb) > d3dEdgeFactor ? (texture(d3dScene, coord7).rgb * 0.09) : (centerColor * 0.09);
	sum += dot(centerNormal, texture(d3dGBufferNormal, coord8).rgb) > d3dEdgeFactor ? (texture(d3dScene, coord8).rgb * 0.05) : (centerColor * 0.05);
 
	d3dOutputColor = vec4(sum, 1.0);
}