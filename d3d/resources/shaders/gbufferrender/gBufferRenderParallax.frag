#version 400

uniform sampler2D d3dDiffuseMap;
uniform sampler2D d3dSpecularMap;
uniform sampler2D d3dShininessMap;
uniform sampler2D d3dEmissiveMap;
uniform sampler2D d3dNormalMap;
uniform sampler2D d3dHeightMap;

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

// Undefine to use just parallax mapping
#define PARALLAX_OCCLUSION_MAPPING

mat3 calculateBasis(inout vec3 tangentEyePos) {
	vec3 q0 = dFdx(viewPosition);
	vec3 q1 = dFdy(viewPosition);
	vec2 st0 = dFdx(texCoord);
	vec2 st1 = dFdy(texCoord);
	vec3 S = normalize(q0 * st1.t - q1 * st0.t);
	vec3 T = normalize(-q0 * st1.s + q1 * st0.s);

	tangentEyePos = vec3(dot(viewPosition, S), dot(viewPosition, T), dot(-viewPosition, viewNormal));

	return mat3(T, S, viewNormal);
}

void linearRayCast(sampler2D heightMap, float numSteps, inout vec3 outputPosition, inout vec3 tangentEyeVec) {
	tangentEyeVec /= numSteps;

	float numStepsOneLess = numSteps - 1;

	for(int i = 0; i < numStepsOneLess; i++) { 
		float depth = texture(heightMap, outputPosition.xy).r;

		if(outputPosition.z < depth)
			outputPosition += tangentEyeVec; 
	} 
}

void binarySearchRayCast(sampler2D heightMap, inout vec3 outputPosition, inout vec3 tangentEyeVec) { 
	const int numSteps = 4;

	for(int i = 0; i < numSteps; i++) { 
		float depth = texture(heightMap, outputPosition.xy).r;

		if(outputPosition.z < depth) 
			outputPosition += tangentEyeVec; 
		
		tangentEyeVec *= 0.5; 
		outputPosition -= tangentEyeVec; 
	} 
}

void main() {
	vec3 tangentEyePos;

	mat3 basis = calculateBasis(tangentEyePos);

	float tangentDist = length(tangentEyePos);

	vec3 tangentSpaceEye = tangentEyePos / tangentDist;

#ifdef PARALLAX_OCCLUSION_MAPPING

	float eyeNormalDot = -dot(normalize(viewPosition), viewNormal);

	tangentSpaceEye *= 1.0 / tangentSpaceEye.z;

	tangentSpaceEye.xy *= d3dHeightMapScale * eyeNormalDot;

	vec3 heightMapPos = vec3(texCoord, 0.0); 

	float numSteps =  2.0 + min(2.0, eyeNormalDot * 2.0); 

	linearRayCast(d3dHeightMap, numSteps, heightMapPos, tangentSpaceEye);
	
	// Binary search for more precision after boundaries were found with linear search
	binarySearchRayCast(d3dHeightMap, heightMapPos, tangentSpaceEye);

	vec2 displacedTexCoords = heightMapPos.xy;

#else // Plain parallax mapping

	float height = texture2D(d3dHeightMap, gl_TexCoord[0].st).r * d3dHeightMapScale;

	vec2 displacedTexCoords = tangentSpaceEye.xy * height + gl_TexCoord[0].st;

#endif

	vec4 diffuseTexColor = texture2D(d3dDiffuseMap, displacedTexCoords).rgba;

	vec3 perturbedNormal = normalize(basis * (texture(d3dNormalMap, displacedTexCoords).rgb * 2.0 - 1.0));

	d3dOutputPosition = vec4(viewPosition, texture(d3dEmissiveMap, texCoord).r * d3dEmissiveColor);//position - perturbedNormal * height;
	d3dOutputNormal = vec4(normalLength * perturbedNormal, d3dShininess * texture(d3dShininessMap, texCoord).r);
	d3dOutputColor = vec4(diffuseTexColor.rgb * d3dDiffuseColor, d3dSpecularColor * texture(d3dSpecularMap, texCoord).r);
}