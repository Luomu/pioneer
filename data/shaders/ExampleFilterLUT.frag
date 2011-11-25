#version 120
uniform sampler2D texture0;
uniform sampler2D texture1;

varying vec2 texCoord;

void main(void)
{
	vec4 scene = texture2D(texture0, texCoord);
	float lum = (scene.r + scene.g + scene.b) / 3.0; 
	//vec4 final = texture2D(texture1, vec2(scene.r, scene.b));
	vec4 sat = texture2D(texture1, vec2(lum, 0.0));
	gl_FragColor = scene * sat.r;
}
