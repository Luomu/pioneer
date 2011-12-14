#version 120
uniform sampler2D sceneTex;

varying vec2 texCoord;

void main(void)
{
	const float delta = 0.001;
	vec3 col          = texture2D(sceneTex, texCoord).rgb;
	gl_FragColor      = vec4(log(delta + dot(col, vec3(0.299,0.587,0.114))));
}
