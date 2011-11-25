#version 120
uniform sampler2D fboTex;

varying vec2 texCoord;

vec4 Desaturate(vec3 color, float Desaturation)
{
	vec3 grayXfer = vec3(0.3, 0.59, 0.11);
	vec3 gray = vec3(dot(grayXfer, color));
	return vec4(mix(color, gray, Desaturation), 1.0);
}

void main(void)
{
	gl_FragColor = Desaturate(texture2D(fboTex, texCoord).rgb, 0.9);
}
