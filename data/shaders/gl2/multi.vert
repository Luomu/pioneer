#ifdef TEXTURE0
varying vec2 texCoord0;
#endif
#ifdef UIBACKGROUND
varying vec2 texCoord1;
#endif
#ifdef VERTEXCOLOR
varying vec4 vertexColor;
#endif

void main(void)
{
	gl_Position = logarithmicTransform();
#ifdef VERTEXCOLOR
	vertexColor = gl_Color;
#endif
#ifdef TEXTURE0
	texCoord0 = gl_MultiTexCoord0.xy;
#endif
#ifdef UIBACKGROUND
	texCoord1 = 0.5 * gl_Position.xy + vec2(0.5);
#endif
}
