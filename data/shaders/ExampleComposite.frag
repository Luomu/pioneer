#version 120
uniform sampler2D first;
uniform sampler2D second;

varying vec2 texCoord;

void main(void)
{
	gl_FragColor = mix(texture2D(first, texCoord), texture2D(second, texCoord), 0.5);
}
