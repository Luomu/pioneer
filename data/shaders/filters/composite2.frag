#version 120
uniform sampler2D texture0;
uniform sampler2D texture1;
uniform float mixFactor;

varying vec2 texCoord;

void main(void)
{
	//combine two textures
	gl_FragColor = mix(texture2D(texture0, texCoord), texture2D(texture1, texCoord), mixFactor);
}
