#version 120
varying vec2 texCoord;
uniform vec2 viewportSize;

void main(void)
{
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	texCoord.x  = gl_MultiTexCoord0.x + 0.5 * (1.0/viewportSize.x);
	texCoord.y  = gl_MultiTexCoord0.y + 0.5 * (1.0/viewportSize.y);
}

