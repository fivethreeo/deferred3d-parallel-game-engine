#version 400

uniform sampler2D d3dGBufferPosition;
uniform sampler2D d3dGBufferNormal;
uniform sampler2D d3dGBufferColor;

layout(shared) uniform d3dPointLight {
	vec3 d3dPointLightPosition;
	vec3 d3dPointLightColor;
	float d3dPointLightRange;
	float d3dPointLightRangeInv;
};

uniform vec2 d3dGBufferSizeInv;
uniform vec3 d3dAttenuation;

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

	float strength = fallOff / (d3dAttenuation.x + d3dAttenuation.y * distance + d3dAttenuation.z * distance * distance);

	vec4 color = texture(d3dGBufferColor, texCoord);

	if(color.a == 0.0) // No specular
		d3dOutputColor = vec4(color.rgb * strength * lambert * d3dPointLightColor, 1.0);
	else {
		vec3 lightRay = reflect(-lightDirection, viewNormal.xyz);
		float specularIntensity = strength * pow(max(0.0, dot(lightRay, normalize(-viewPosition))), viewNormal.a);

		d3dOutputColor = vec4(color.rgb * strength * lambert * d3dPointLightColor + color.a * specularIntensity * d3dPointLightColor, 1.0);
	}
}