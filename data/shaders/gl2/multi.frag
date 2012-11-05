#ifdef TEXTURE0
uniform sampler2D texture0;
varying vec2 texCoord0;
#endif
#ifdef UIBACKGROUND
varying vec2 texCoord1;
uniform sampler2D texture1;
#endif
#ifdef VERTEXCOLOR
varying vec4 vertexColor;
#endif

struct Material {
	vec4 diffuse;
};
uniform Material material;

void main(void)
{
#ifdef VERTEXCOLOR
	vec4 color = vertexColor;
#else
	vec4 color = material.diffuse;
#endif
#ifdef TEXTURE0
	color *= texture2D(texture0, texCoord0);
#endif
#ifdef UIBACKGROUND
	vec4 bgcol = texture2D(texture1, texCoord1) * vec4(0.0, 1.0, 0.0, 0.0);
	color += bgcol;
#endif
	gl_FragColor = color;
	SetFragDepth();
}
