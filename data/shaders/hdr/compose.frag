#version 120

uniform sampler2D sceneTexture;
uniform float avgLum;

varying vec2 texCoord;

void main(void)
{
	gl_FragColor = texture2D(sceneTexture, texCoord);
}
