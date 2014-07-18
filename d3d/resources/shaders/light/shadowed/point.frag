#version 400

uniform sampler2D d3dGBufferPosition;
uniform sampler2D d3dGBufferNormal;
uniform sampler2D d3dGBufferColor;

uniform sampler2D d3dNoiseMap;

uniform samplerCubeShadow d3dShadowMap;

layout(shared) uniform d3dPointLightShadowed {
	vec3 d3dPointLightPosition;
	vec3 d3dPointLightColor;
	float d3dPointLightRange;
	float d3dPointLightRangeInv;

	mat4 d3dToLightSpace;

	// Instead of using full projection matrix, use only elements of it that are needed
	float d3dProj22;
	float d3dProj23;
};

uniform vec2 d3dGBufferSizeInv;
uniform vec3 d3dAttenuation;

const int d3dNumSamples = 16;
const float d3dNumSamplesInv = 1.0 / d3dNumSamples;
const float d3dSampleOffset = 0.05;

out vec4 d3dOutputColor;

void main() {
	vec2 texCoord = gl_FragCoord.xy * d3dGBufferSizeInv;

	vec3 viewPosition = texture(d3dGBufferPosition, texCoord).xyz;

	vec3 lightDirection = d3dPointLightPosition - viewPosition;
	float distance = length(lightDirection);
	
	lightDirection /= distance;

	vec4 viewNormal = texture(d3dGBufferNormal, texCoord);

	float normalLength = length(viewNormal.xyz);

	float lambert = dot(lightDirection, viewNormal.xyz) * normalLength + 1.0 - normalLength;

	viewNormal.xyz /= normalLength;

	if(lambert <= 0.0) {
		d3dOutputColor = vec4(0.0, 0.0, 0.0, 1.0);
		return;
	}

	float fallOff = (d3dPointLightRange - distance) * d3dPointLightRangeInv;

	if(fallOff <= 0.0) {
		d3dOutputColor = vec4(0.0, 0.0, 0.0, 1.0);
		return;
	}

	vec3 lightSpacePosition = (d3dToLightSpace * vec4(viewPosition, 1.0)).xyz;
	vec3 absLightSpacePosition = abs(lightSpacePosition);
	float maxDist = max(absLightSpacePosition.x, max(absLightSpacePosition.y, absLightSpacePosition.z));

	float depth = (d3dProj22 * -maxDist + d3dProj23) / maxDist * 0.5 + 0.5;

	// Bias
	depth -= 0.005;

	float shadow = 0.0;
	
	for (int i = 0; i < d3dNumSamples; i++) {
		vec3 sampleRay = lightSpacePosition + d3dSampleOffset * (texture(d3dNoiseMap, vec2(i * 0.8475)).xyz * 2.0 - 1.0);

		shadow += texture(d3dShadowMap, vec4(sampleRay, depth));
	}

	shadow *= d3dNumSamplesInv;

	float strength = shadow * fallOff / (d3dAttenuation.x + d3dAttenuation.y * distance + d3dAttenuation.z * distance * distance);

	vec4 color = texture(d3dGBufferColor, texCoord);

	if(color.a == 0.0) // No specular
		d3dOutputColor = vec4(color.rgb * strength * lambert * d3dPointLightColor, 1.0);
	else {
		vec3 lightRay = reflect(-lightDirection, viewNormal.xyz);
		float specularIntensity = strength * pow(max(0.0, dot(lightRay, normalize(-viewPosition))), viewNormal.a);

		d3dOutputColor = vec4(color.rgb * strength * lambert * d3dPointLightColor + color.a * specularIntensity * d3dPointLightColor, 1.0);
	}
}