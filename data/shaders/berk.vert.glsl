varying vec2 texCoord;   //gl_MultiTexCoord0.xy;
uniform vec2 viewportSize;

void main(void)
{
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	texCoord      = gl_MultiTexCoord0.xy;
}

