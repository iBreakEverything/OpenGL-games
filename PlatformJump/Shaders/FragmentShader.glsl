#version 330

// get color value from vertex shader
in vec4 frag_color;
in float noise;
in float platform;
in float frag_effect;

layout(location = 0) out vec4 out_color;

float random(vec3 scale, float seed) {
	return fract(sin(dot(gl_FragCoord.xyz + seed, scale)) * 43758.5453 + seed);
}

void main()
{
	if (platform == 0.0 && frag_effect != 0) {
		float r = .01 * random(vec3(12.9898, 78.233, 151.7182), 0.0);
		float aux = 1.3 * noise + r;
		out_color = vec4(frag_color.xyz * aux, 1.0);
	}

	if (platform == 1.0) {
		out_color = frag_color;
	}
}