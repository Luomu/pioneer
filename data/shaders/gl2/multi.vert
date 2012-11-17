#ifdef TEXTURE0
varying vec2 texCoord0;
#endif
#ifdef VERTEXCOLOR
varying vec4 vertexColor;
#endif
#if (NUM_LIGHTS > 0)
varying vec3 eyePos;
varying vec3 v_normal;
#ifdef MAP_NORMAL
attribute vec3 a_tangent;
varying vec3 v_tangent;
varying vec3 v_bitangent;
#endif
#endif //num_lights

void main(void)
{
	gl_Position = logarithmicTransform();
#ifdef VERTEXCOLOR
	vertexColor = gl_Color;
#endif
#ifdef TEXTURE0
	texCoord0 = gl_MultiTexCoord0.xy;
#endif
#if (NUM_LIGHTS > 0)
	eyePos = vec3(gl_ModelViewMatrix * gl_Vertex);
	v_normal = normalize(gl_NormalMatrix * gl_Normal);
#ifdef MAP_NORMAL
	v_tangent = gl_NormalMatrix * a_tangent;
	v_bitangent = gl_NormalMatrix * cross(gl_Normal, a_tangent);
#endif
#endif
}
