varying vec4 color;
varying vec2 tcoord;
varying vec3 viewdir;

void main(void)
{
#ifdef ZHACK
	gl_Position = logarithmicTransform();
#else
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
#endif
	// not using gl_FrontColor because it gets clamped in vtx shaders
	color = gl_Color;
	tcoord = gl_MultiTexCoord0.xy;
	tcoord = vec2(tcoord.x, tcoord.y);
	viewdir = 
		normalize(vec3(-gl_ModelViewMatrix[2][0], -gl_ModelViewMatrix[2][1], -gl_ModelViewMatrix[2][2]));
}

