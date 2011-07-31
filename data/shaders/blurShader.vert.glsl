varying vec4 color;
varying vec2 texcoord;

void main(void)
{
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;

	// not using gl_FrontColor because it gets clamped in vtx shaders
	color = gl_Color;
	texcoord = gl_MultiTexCoord0.st;
}
