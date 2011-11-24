#version 120
uniform sampler2D texture0;
uniform sampler2D texture1;

varying vec2 texCoord;

void main(void)
{
	vec4 scene = texture2D(texture0, texCoord);
	vec4 final = texture2D(texture1, vec2(scene.r, scene.b));
	gl_FragColor = final;
}
