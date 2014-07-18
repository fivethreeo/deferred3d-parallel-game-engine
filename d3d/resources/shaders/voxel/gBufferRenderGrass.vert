#version 400

uniform mat4 d3dViewModel;
uniform mat4 d3dProjectionViewModel;
uniform mat3 d3dNormal;

uniform float d3dWaveTime;
uniform float d3dCompletelyVisibleDistance;
uniform float d3dInvFadeRange; // = 1.0 / (d3dCompletelyInvisibleDistance - d3dCompletelyVisibleDistance)
uniform float d3dAirWavDistMultInv = 0.4;
uniform float d3dWaveDistance = 0.04;

layout(location = 0) in vec3 positionIn;
layout(location = 1) in vec3 normalIn;
layout(location = 2) in vec2 texCoordIn;

smooth out vec3 viewPosition;
smooth out vec3 viewNormal;
smooth out vec2 texCoord;
smooth out float normalLength;
smooth out float fadeFactor;

void main() {
	vec3 position = positionIn;

	position.xz += vec2(sin(d3dWaveTime + position.x * d3dAirWavDistMultInv), cos(d3dWaveTime + position.z * d3dAirWavDistMultInv)) * d3dWaveDistance * texCoordIn.y;

	viewPosition = (d3dViewModel * vec4(position, 1.0)).xyz;
	viewNormal = normalize(d3dNormal * normalIn);
	texCoord = texCoordIn;

	normalLength = length(normalIn);

	gl_Position = d3dProjectionViewModel * vec4(position, 1.0);

	// Distance from camera
	float dist = length(viewPosition);

	fadeFactor = max(0.0, (dist - d3dCompletelyVisibleDistance) * d3dInvFadeRange);
}
