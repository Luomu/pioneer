varying vec2 uv0;

void main(void)
{
	gl_Position = gl_Vertex;
	uv0 = gl_MultiTexCoord0.xy;
}

