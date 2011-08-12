#version 120
uniform sampler2D texture0;

varying vec2 texCoord;

// downscale stage of making the bloom texture
void main(void)
{
	const float delta = 0.001;
	vec3 col = vec3(texture2D(texture0, texCoord));
	gl_FragColor = vec4(log(delta + dot(col, vec3(0.299,0.587,0.114))));
}
