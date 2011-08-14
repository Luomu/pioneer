#version 120
uniform sampler2D texture0;

varying vec2 texCoord;

float calcLuminance(vec3 color)
{
	return max(dot(color, vec3(0.299, 0.587, 0.114)), 0.0001);
}

// downscale stage of making the bloom texture
void main(void)
{
	vec3 color = texture2D(texture0, texCoord).rgb;
	float lum = calcLuminance(color);
	gl_FragColor = vec4(lum, lum, lum, 1.0);
#if 0
	const float delta = 0.001;
	vec3 col = vec3(texture2D(texture0, texCoord));
	gl_FragColor = vec4(log(delta + dot(col, vec3(0.299,0.587,0.114))));
#endif
}
