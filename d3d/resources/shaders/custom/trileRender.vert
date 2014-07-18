#version 400

uniform mat4 d3dViewModel;
uniform mat4 d3dProjectionViewModel;
uniform mat3 d3dNormal;

layout(location = 0) in vec3 positionIn;
layout(location = 1) in vec3 normalIn;

smooth out vec3 viewPosition;
smooth out vec3 viewNormal;
smooth out vec3 viewTangent;
smooth out vec3 viewBitangent;
smooth out vec3 texVec;

void main() {
	viewPosition = (d3dViewModel * vec4(positionIn, 1.0)).xyz;

	if (normalIn.y > 0.95 || normalIn.y < -0.95) {
		viewNormal = normalize(d3dNormal * normalIn);
		viewTangent = normalize(d3dNormal * cross(normalIn, vec3(1.0, 0.0, 0.0)));
		viewBitangent = normalize(cross(viewTangent, viewNormal));
	}
	else {
		viewNormal = normalize(d3dNormal * normalIn);
		viewTangent = normalize(d3dNormal * cross(normalIn, vec3(0.0, 1.0, 0.0)));
		viewBitangent = normalize(cross(viewTangent, viewNormal));
	}

	vec3 notFollowingNormalAxes = vec3(1.0, 1.0, 1.0) - abs(normalIn);

	texVec = positionIn * notFollowingNormalAxes + normalIn * 0.5;

	gl_Position = d3dProjectionViewModel * vec4(positionIn, 1.0);
}
