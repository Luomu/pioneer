varying vec2 texCoord0;
varying vec3 vertexPos;
varying float detailDist;
varying vec3 ecPos;

void main(void)
{
	gl_Position = logarithmicTransform();

	texCoord0 = gl_MultiTexCoord0.st;
	vertexPos = gl_Vertex.xyz;
	ecPos = (gl_ModelViewMatrix * gl_Vertex).xyz;
}

