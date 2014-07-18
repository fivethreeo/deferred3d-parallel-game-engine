#version 400

uniform samplerCube trileTexture;
uniform samplerCube trileNormalMap;

uniform float d3dEmissiveColor;

smooth in vec3 viewPosition;
smooth in vec3 viewNormal;
smooth in vec3 viewTangent;
smooth in vec3 viewBitangent;
smooth in vec3 texVec;

layout (location = 0) out vec4 d3dOutputPosition;
layout (location = 1) out vec4 d3dOutputNormal;
layout (location = 2) out vec4 d3dOutputColor;
void main() {
	//mat3 basis = mat3(viewTangent, viewBitangent, viewNormal);

	d3dOutputPosition = vec4(viewPosition, d3dEmissiveColor);
	d3dOutputNormal = vec4(viewNormal, 96.0);//normalize(basis * (texture(trileNormalMap, texVec).rgb * 2.0 - 1.0)), 96.0);
	d3dOutputColor = vec4(texture(trileTexture, texVec).rgb, 0.0);
}