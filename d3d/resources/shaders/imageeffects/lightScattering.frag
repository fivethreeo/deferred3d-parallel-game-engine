#version 400

uniform sampler2D d3dGBufferPosition;

uniform vec2 d3dSizeInv;

uniform vec4 d3dLightProjectedPosition;
uniform vec3 d3dLightViewPosition;

uniform vec3 d3dColor;

const int d3dSteps = 60;

const float d3dDecay = 0.95;

const float d3dStrengthScalar = 0.2;
const float d3dFalloffScalar = 800.0;

out vec4 d3dOutputColor;

void main() {
	vec2 coord = gl_FragCoord.xy * d3dSizeInv;

	vec2 sampleCoord = d3dLightProjectedPosition.xy / d3dLightProjectedPosition.w * 0.5 + 0.5;

	vec2 deltaCoord = (coord - sampleCoord) * normalize(1.0 / d3dSizeInv);

	deltaCoord /= float(d3dSteps);

	float illum = 1.0;

	float strength = 0.0;

	for (int i = 0; i < d3dSteps; i++) {
		sampleCoord += deltaCoord;

		float depth = -texture(d3dGBufferPosition, sampleCoord).z;

		strength +=  depth > -d3dLightViewPosition.z ? illum : 0.0;

		illum *= d3dDecay;
	}

	d3dOutputColor = vec4(vec3(strength * d3dStrengthScalar / (1.0 + length(deltaCoord) * d3dFalloffScalar)) * d3dColor, 1.0);
}