#version 400

uniform sampler2D d3dGBufferPosition;
uniform sampler2D d3dGBufferNormal;

uniform sampler2D d3dNoiseMap;

uniform vec2 d3dSizeInv;

uniform float d3dRadius;
uniform float d3dStrength;

uniform mat4 d3dProjectionMatrix;

out vec4 d3dOutputColor;

void main() {
	const int d3dSampleKernelSize = 8; // 8 sphere samples
	const float d3dSampleKernelSizeInv = 1.0 / d3dSampleKernelSize;

	const float d3dNoiseScale = 431.8293;

	const vec3 d3dSampleKernal[8] = vec3[8] (
		/*vec3(-0.71139721306844, 0.090695798894178, 0.4135832902777),
		vec3(0.14465329259179, -0.034248374246736, 0.89409288765134),
		vec3(0.58360584008039, 0.44579354263549, 0.47210918149907),
		vec3(0.047548117268109, 0.0030572801751948, 0.10299966569938),
		vec3(-0.21051528102914, -0.069836323762254, 0.11384665140169),
		vec3(0.097336485439401, -0.01082057009773, 0.035590220164099),
		vec3(-0.31928118029602, -0.079396180775845, 0.5184908904021),
		vec3(0.24461479160575, 0.25759929246472, 0.59937776422696),*/
		vec3(-0.17906078187744, -0.53807070920131, 0.31131590821295),
		vec3(0.11925984563736, 0.12737879026415, 0.32825843762036),
		vec3(0.23396103061314, 0.14105650025399, 0.89225951550769),
		vec3(0.015711929311137, -0.14279639826434, 0.27672856343535),
		vec3(0.24737787541745, -0.19831704566618, 0.79865570368882),
		vec3(0.19465182088177, 0.19578779305265, 0.35939170086383),
		vec3(-0.021866072947625, -0.018960301488754, 0.11786785239381),
		vec3(-0.016971704335328, -0.055696815075632, 0.13888215827939)
	);

	vec2 coord = gl_FragCoord.xy * d3dSizeInv;

	vec3 position = texture(d3dGBufferPosition, coord).xyz;
	vec3 normal = texture(d3dGBufferNormal, coord).xyz;

	float normalLength = length(normal);

	normal /= normalLength;

	float occlusion = 0.0;

	vec3 random = texture(d3dNoiseMap, coord * d3dNoiseScale).xyz * 2.0 - 1.0;
	vec3 tangent = normalize(random - normal * dot(random, normal));
	vec3 bitangent = cross(normal, tangent);
	mat3 tbn = mat3(tangent, bitangent, normal);

	for (int i = 0; i < d3dSampleKernelSize; i++) {
		vec3 occSample = tbn * d3dSampleKernal[i];
		occSample = occSample * d3dRadius + position;
  
		vec4 offset = vec4(occSample, 1.0);
		offset = d3dProjectionMatrix * offset;
		offset.xy /= offset.w;
		offset.xy = offset.xy * 0.5 + 0.5; 

		float sampleZ = texture(d3dGBufferPosition, offset.xy).z;
  
		float rangeCheck = abs(position.z - sampleZ) < d3dRadius ? 1.0 : 0.0;

		occlusion += step(-sampleZ, -occSample.z) * rangeCheck;
	}
 
	d3dOutputColor = vec4(pow(1.0 - normalLength * occlusion * d3dSampleKernelSizeInv, d3dStrength));
}