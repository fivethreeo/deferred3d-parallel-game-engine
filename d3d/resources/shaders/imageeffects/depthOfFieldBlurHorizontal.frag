#version 400

uniform sampler2D d3dScene;
uniform sampler2D d3dGBufferPosition;
uniform vec2 d3dSizeInv;

uniform float d3dBlurSize;

uniform float d3dFocalDistance;
uniform float d3dFocalRange;

const float d3dBlurPower = 2.0;
const float d3dBlurStrengthScalar = 200.0;

out vec4 d3dOutputColor;

void main() {
	vec3 sum = vec3(0.0);

	vec2 coord = gl_FragCoord.xy * d3dSizeInv;

	float distFromFocal = abs(d3dFocalDistance + texture(d3dGBufferPosition, coord).z);

	float blurSize = min(d3dBlurSize, d3dBlurStrengthScalar * pow(d3dBlurSize * max(0.0, distFromFocal - d3dFocalRange), d3dBlurPower));

	sum += texture(d3dScene, vec2(coord.x - 4.0 * blurSize, coord.y)).rgb * 0.05;
	sum += texture(d3dScene, vec2(coord.x - 3.0 * blurSize, coord.y)).rgb * 0.09;
	sum += texture(d3dScene, vec2(coord.x - 2.0 * blurSize, coord.y)).rgb * 0.12;
	sum += texture(d3dScene, vec2(coord.x - blurSize, coord.y)).rgb * 0.15;
	sum += texture(d3dScene, vec2(coord.x, coord.y)).rgb * 0.16;
	sum += texture(d3dScene, vec2(coord.x + blurSize, coord.y)).rgb * 0.15;
	sum += texture(d3dScene, vec2(coord.x + 2.0 * blurSize, coord.y)).rgb * 0.12;
	sum += texture(d3dScene, vec2(coord.x + 3.0 * blurSize, coord.y)).rgb * 0.09;
	sum += texture(d3dScene, vec2(coord.x + 4.0 * blurSize, coord.y)).rgb * 0.05;
 
	d3dOutputColor = vec4(sum, 1.0);
}