#version 400

uniform sampler2D d3dDiffuseMap;
uniform sampler2D d3dSpecularMap;
uniform sampler2D d3dShininessMap;
uniform sampler2D d3dEmissiveMap;

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

void main() {
	// Alpha testing
	vec4 diffuseTexColor = texture(d3dDiffuseMap, texCoord);

	if(diffuseTexColor.a < 0.5)
		discard;

	d3dOutputPosition = vec4(viewPosition, texture(d3dEmissiveMap, texCoord).r * d3dEmissiveColor);
	d3dOutputNormal = vec4(normalLength * viewNormal, d3dShininess * texture(d3dShininessMap, texCoord).r);
	d3dOutputColor = vec4(diffuseTexColor.rgb * d3dDiffuseColor, d3dSpecularColor * texture(d3dSpecularMap, texCoord).r);
}