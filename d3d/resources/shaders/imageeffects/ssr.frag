#version 400

uniform sampler2D d3dGBufferPosition;
uniform sampler2D d3dGBufferNormal;
uniform sampler2D d3dGBufferColor;
uniform sampler2D d3dGBufferEffect;
uniform samplerCube d3dCubeMap;
uniform sampler2D d3dNoiseMap;

uniform vec2 d3dSizeInv;

const float d3dRayStep = 0.3;
const int d3dMaxSteps = 30;
const float d3dSearchDist = d3dRayStep * d3dMaxSteps;
const float d3dSearchDistInv = 1.0 / d3dSearchDist;
const int d3dNumBinarySearchSteps = 4;
const float d3dError = 2.0;
const float d3dEnvMapPower = 0.1;
const int d3dSamples = 1;
const float d3dSamplesInv = 1.0 / d3dSamples;
const float d3dScreenEdgePower = 4.0;
const float d3dZAlignedExtraLength = 8.0;
const float d3dEnvMapIntensity = 0.2;

uniform mat4 d3dProjectionMatrix;
uniform mat3 d3dNormalMatrixInv;

out vec4 d3dOutputColor;

void main() {
	vec2 coord = gl_FragCoord.xy * d3dSizeInv;

	// Samples
	float specular = texture(d3dGBufferColor, coord).a;

	if(specular == 0.0) {
		d3dOutputColor = vec4(0.0, 0.0, 0.0, 1.0);
		return;
	}

	vec3 color = vec3(0.0);

	for (int s = 0; s < d3dSamples; s++) {
		vec3 position = texture(d3dGBufferPosition, coord).xyz;
		vec4 normalAndShininess = texture(d3dGBufferNormal, coord);
		vec3 normal = normalize(normalAndShininess.xyz);

		// Reflection vector
		vec3 reflected = normalize(reflect(normalize(position), normal));

		// Perturb reflection
		reflected += (texture(d3dNoiseMap, coord * 532.4323 + vec2(float(s) * 31.0162)).xyz * 2.0 - 1.0) / normalAndShininess.w;

		reflected = normalize(reflected);

		vec3 positionStep = reflected * d3dRayStep;// * (1.0 + d3dZAlignedExtraLength * abs(dot(reflected, vec3(0.0, 0.0, 1.0)))) * -position.z;

		float distance = length(positionStep) * d3dMaxSteps;

		vec4 projectedReflected0 = d3dProjectionMatrix * vec4(position, 1.0);
		vec4 projectedReflected1 = d3dProjectionMatrix * vec4(position + positionStep, 1.0);

		vec2 coordStep = (projectedReflected1.xy / projectedReflected1.w - projectedReflected0.xy / projectedReflected0.w);
	
		float error = d3dError * length(positionStep);

		// Ray cast
		vec3 hitPos = position;
		vec2 hitCoord = coord;

		for (int i = 0; i < d3dMaxSteps; i++) {
			hitPos += positionStep;
			hitCoord += coordStep;

			float depth = texture(d3dGBufferPosition, hitCoord).z;
			//vec3 hitNormal = normalize(texture(d3dGBufferNormal, hitCoord).xyz);

			// && dot(-reflected, hitNormal) > 0.0

			if (hitPos.z < depth && depth - hitPos.z < error) {
				// Binary search
				positionStep *= 0.5;
				coordStep *= 0.5;

				hitPos -= positionStep;
				hitCoord -= coordStep;

				for (int j = 0; j < d3dNumBinarySearchSteps; j++) {
					depth = texture(d3dGBufferPosition, hitCoord).z;
					//hitNormal = normalize(texture(d3dGBufferNormal, hitCoord).xyz);

					// && dot(-reflected, hitNormal) > 0.0
					if (!(hitPos.z < depth && depth - hitPos.z < error)) {
						hitPos += positionStep;
						hitCoord += coordStep;
					}

					positionStep *= 0.5;
					coordStep *= 0.5;

					hitPos -= positionStep;
					hitCoord -= coordStep;
				}

				break;
			}
		}

		vec2 dCoords = 2.0 * abs(vec2(0.5, 0.5) - hitCoord);

		float screenEdgefactor = pow(clamp(1.0 - dCoords.x, 0.0, 1.0) * clamp(1.0 - dCoords.y, 0.0, 1.0), d3dScreenEdgePower);

		vec3 hitNormal = normalize(texture(d3dGBufferNormal, hitCoord).xyz);

		float ssrStrength;
		
		//if (dot(-reflected, hitNormal) < 0.0)
		//	ssrStrength = 0.0;
		//else
			ssrStrength = pow(clamp(((distance - length(hitPos - position)) / distance) * (1.0 - reflected.z) * screenEdgefactor * max(0.0, dot(hitNormal, -reflected)), 0.0, 1.0), d3dEnvMapPower);

		color += texture(d3dGBufferEffect, hitCoord).rgb * ssrStrength + texture(d3dCubeMap, d3dNormalMatrixInv * reflected).rgb * (1.0 - ssrStrength) * d3dEnvMapIntensity;
	}

	vec3 baseColor = texture(d3dGBufferColor, coord).rgb;

	// Get color
	d3dOutputColor = vec4(specular * baseColor * color * d3dSamplesInv, 1.0);
}