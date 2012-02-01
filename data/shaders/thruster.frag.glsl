varying vec4 color;
varying vec2 tcoord;
uniform sampler2D flaretex;
varying vec3 viewdir;
uniform bool inverse;

float saturate( float inValue)
{
   return clamp( inValue, 0.0, 1.0);
}

void main(void)
{
	//blue
	//vec4 dcolor = vec4(0.2, 0.4, 1.0, 1.0)
	//orange
	vec4 dcolor = vec4(1.0, 0.4, 0.2, 1.0);
	vec3 coronadir = vec3(0.0, 0.0, -1.0);
	float glow;
	if (inverse) // only draw within 'view cone'
		glow   = pow(saturate(dot(viewdir, coronadir)), 4.0);
	else // don't draw when looking from the front
		glow   = pow(1.0 - saturate(dot(viewdir, coronadir)), 0.5);

	gl_FragColor = glow * texture2D(flaretex, tcoord) * dcolor;

#ifdef ZHACK
	SetFragDepth(gl_TexCoord[6].z);
#endif
}
