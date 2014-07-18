#version 400

uniform sampler2D d3dGBufferPosition;
uniform sampler2D d3dGBufferNormal;
uniform sampler2D d3dGBufferColor;

layout(shared) uniform d3dDirectionalLight {
	vec3 d3dDirectionalLightColor;
	vec3 d3dDirectionalLightDirection;
};

uniform vec2 d3dGBufferSizeInv;
uniform vec3 d3dAttenuation;

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

	vec4 color = texture(d3dGBufferColor, texCoord);

	// Specular
	if(color.a == 0.0) // No specular
		d3dOutputColor = vec4(color.rgb * lambert * d3dDirectionalLightColor, 1.0);
	else { // Specular
		vec3 viewPosition = texture(d3dGBufferPosition, texCoord).xyz;

		vec3 lightRay = reflect(d3dDirectionalLightDirection, viewNormal.xyz);
		float specularIntensity = pow(max(0.0, dot(lightRay, normalize(-viewPosition))), viewNormal.a);

		d3dOutputColor = vec4(color.rgb * lambert * d3dDirectionalLightColor + color.a * specularIntensity * d3dDirectionalLightColor, 1.0);
	}
}