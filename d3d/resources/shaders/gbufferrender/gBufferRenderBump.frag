#version 400

uniform sampler2D d3dDiffuseMap;
uniform sampler2D d3dSpecularMap;
uniform sampler2D d3dShininessMap;
uniform sampler2D d3dEmissiveMap;
uniform sampler2D d3dNormalMap;

layout(shared) uniform d3dMaterial {
	vec3 d3dDiffuseColor;
	float d3dSpecularColor;
	float d3dShininess;
	float d3dEmissiveColor;
	float d3dHeightMapScale;
};

smooth in vec3 viewPosition;
smooth in vec3 viewNormal;
smooth in vec2 texCoord;
smooth in float normalLength;

layout (location = 0) out vec4 d3dOutputPosition;
layout (location = 1) out vec4 d3dOutputNormal;
layout (location = 2) out vec4 d3dOutputColor;

mat3 calculateBasis() {
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
	// Alpha testing
	vec4 diffuseTexColor = texture(d3dDiffuseMap, texCoord);

	if(diffuseTexColor.a < 0.5)
		discard;

	d3dOutputPosition = vec4(viewPosition, texture(d3dEmissiveMap, texCoord).r * d3dEmissiveColor);
	d3dOutputNormal = vec4(normalLength * normalize(calculateBasis() * (texture(d3dNormalMap, texCoord).rgb * 2.0 - 1.0)), d3dShininess * texture(d3dShininessMap, texCoord).r);
	d3dOutputColor = vec4(diffuseTexColor.rgb * d3dDiffuseColor, d3dSpecularColor * texture(d3dSpecularMap, texCoord).r);
}