#version 400

uniform sampler2DArray d3dDiffuseMapArray;
uniform sampler2DArray d3dNormalMapArray;

smooth in vec3 viewPosition;
smooth in vec3 viewNormal;
smooth in vec3 worldPosition;
smooth in vec3 worldNormal;

uniform float d3dShininess = 96.0;
uniform float d3dTextureStretchScalar = 0.3;

uniform float d3dSideSeparationExponent = 7.0;

layout (location = 0)  out vec4 d3dOutputPosition;
layout (location = 1)  out vec4 d3dOutputNormal;
layout (location = 2)  out vec4 d3dOutputColor;

mat3 calculateBasis(vec2 texCoord) {
    vec3 dp1 = dFdx(viewPosition);
    vec3 dp2 = dFdy(viewPosition);
    vec2 duv1 = dFdx(texCoord);
    vec2 duv2 = dFdy(texCoord);
 
    vec3 dp2perp = cross(dp2, viewNormal);
    vec3 dp1perp = cross(viewNormal, dp1);
    vec3 T = dp2perp * duv1.x + dp1perp * duv2.x;
    vec3 B = dp2perp * duv1.y + dp1perp * duv2.y;
 
    float invMax = inversesqrt(max(dot(T, T), dot(B, B)));
    return mat3(T * invMax, B * invMax, viewNormal);
}

void main() {
	// Blend factors
	vec2 coord1 = (worldPosition.xz * d3dTextureStretchScalar);
	vec2 coord2 = (worldPosition.xy * d3dTextureStretchScalar);
	vec2 coord3 = (worldPosition.yz * d3dTextureStretchScalar);

	vec4 blend = vec4(
		pow(max(0.0, dot(worldNormal, vec3(0.0, 1.0, 0.0))), d3dSideSeparationExponent), // Top
		pow(abs(dot(worldNormal, vec3(1.0, 0.0, 0.0))), d3dSideSeparationExponent), // XY
		pow(abs(dot(worldNormal, vec3(0.0, 0.0, 1.0))), d3dSideSeparationExponent), // ZY
		pow(max(0.0, dot(worldNormal, vec3(0.0, -1.0, 0.0))), d3dSideSeparationExponent) // Bottom
	);
	
	blend = normalize(blend);

	d3dOutputPosition = vec4(viewPosition, 0.0);

	d3dOutputNormal = vec4(normalize(
		blend.x * calculateBasis(coord1) * (texture(d3dNormalMapArray, vec3(coord1, 0)).rgb * 2.0 - 1.0) +
		blend.y * calculateBasis(coord3) * (texture(d3dNormalMapArray, vec3(coord3, 1)).rgb * 2.0 - 1.0) +
		blend.z * calculateBasis(coord2) * (texture(d3dNormalMapArray, vec3(coord2, 1)).rgb * 2.0 - 1.0) +
		blend.w * calculateBasis(coord1) * (texture(d3dNormalMapArray, vec3(coord1, 2)).rgb * 2.0 - 1.0)), d3dShininess);

	d3dOutputColor = vec4((
		blend.x * texture(d3dDiffuseMapArray, vec3(coord1, 0)).rgb + 
		blend.y * texture(d3dDiffuseMapArray, vec3(coord3, 1)).rgb +
		blend.z * texture(d3dDiffuseMapArray, vec3(coord2, 1)).rgb +
		blend.w * texture(d3dDiffuseMapArray, vec3(coord1, 2)).rgb),
		0.0);
}