varying vec2 texCoord0;
varying float dist;

void main(void)
{
	texCoord0 = gl_MultiTexCoord0.st;
	vec3 ecposition = vec3(gl_ModelViewMatrix * gl_Vertex);
	dist = abs(ecposition.z);
#ifdef ZHACK
	gl_Position = logarithmicTransform();
#else
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
#endif
	gl_FrontColor = gl_Color;
	gl_TexCoord[0] = gl_ModelViewMatrix * gl_Vertex;
	vec3 tnorm = gl_NormalMatrix * gl_Normal;
	gl_TexCoord[1] = vec4(tnorm.x, tnorm.y, tnorm.z, 0.0);
}
