#version 120
varying vec2 texCoord;   //gl_MultiTexCoord0.xy;
varying vec2 texCoord_c; //half-pixel adjusted for better centering of downsampled textures
uniform vec2 viewportSize;

void main(void)
{
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	texCoord_c.x  = gl_MultiTexCoord0.x + 0.5 * (1.0/viewportSize.x);
	texCoord_c.y  = gl_MultiTexCoord0.y + 0.5 * (1.0/viewportSize.y);
	texCoord      = gl_MultiTexCoord0.xy;
}

