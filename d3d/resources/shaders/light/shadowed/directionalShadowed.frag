#version 400

const int maxNumCascades = 3;

uniform sampler2D d3dGBufferPosition;
uniform sampler2D d3dGBufferNormal;
uniform sampler2D d3dGBufferColor;

uniform sampler2D d3dNoiseMap;

uniform sampler2DShadow d3dCascadeShadowMaps[maxNumCascades];

layout(shared) uniform d3dDirectionalLightShadowed {
	vec3 d3dDirectionalLightColor;
	vec3 d3dDirectionalLightDirection;
	int d3dNumCascades;
	float d3dSplitDistances[maxNumCascades];
	mat4 d3dLightBiasViewProjections[maxNumCascades];
};

uniform vec2 d3dGBufferSizeInv;
uniform vec3 d3dAttenuation;

const int d3dNumSamples = 4;
const float d3dNumSamplesInv = 1.0 / d3dNumSamples;
const float d3dSampleOffset = 0.0009;

out vec4 d3dOutputColor;

void main() {
	vec2 texCoord = gl_FragCoord.xy * d3dGBufferSizeInv;

	vec4 viewNormal = texture(d3dGBufferNormal, texCoord);

	float normalLength = length(viewNormal.xyz);

	float lambert = dot(-d3dDirectionalLightDirection, viewNormal.xyz) * normalLength + 1.0 - normalLength;

	viewNormal.xyz /= normalLength;

	if(lambert <= 0.0) {
		d3dOutputColor = vec4(0.0, 0.0, 0.0, 1.0);
		return;
	}

	float shadowFactor = 1.0;

	vec3 viewPosition = texture(d3dGBufferPosition, texCoord).xyz;

	for (int i = 0; i < d3dNumCascades; i++) {
		if (-viewPosition.z < d3dSplitDistances[i]) {
			// Perform shadowed lighting
			vec4 shadowCoord = d3dLightBiasViewProjections[i] * vec4(viewPosition, 1.0);
			//shadowCoord.xy /= shadowCoord.w;
			shadowCoord.z -= 0.0003 + i * 0.0003; // Increase bias with distance (cascade)

			shadowFactor = 0.0;

			float x, y;

			for(int j = 0; j < d3dNumSamples; j++)
				shadowFactor += texture(d3dCascadeShadowMaps[i], vec3(shadowCoord.xy + d3dSampleOffset / i * (texture(d3dNoiseMap, vec2(j * 0.8475)).xy * 2.0 - 1.0), shadowCoord.z));
					
			shadowFactor *= d3dNumSamplesInv;

			break;
		}
	}

	vec4 color = texture(d3dGBufferColor, texCoord);

	// Specular
	if(color.a == 0.0) // No specular
		d3dOutputColor = vec4(color.rgb * lambert * shadowFactor * d3dDirectionalLightColor, 1.0);
	else { // Specular
		vec3 lightRay = reflect(d3dDirectionalLightDirection, viewNormal.xyz);
		float specularIntensity = pow(max(0.0, dot(lightRay, normalize(-viewPosition))), viewNormal.a);

		d3dOutputColor = vec4(color.rgb * lambert * shadowFactor * d3dDirectionalLightColor + color.a * specularIntensity * d3dDirectionalLightColor, 1.0);
	}
}