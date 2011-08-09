#version 120

uniform sampler2D fboTex;

varying vec2 texCoord;

void main(void)
{
	gl_FragColor = texture2D(fboTex, texCoord);
}
