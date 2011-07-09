#extension GL_ARB_texture_rectangle : enable
uniform sampler2DRect fboTex;
uniform float avgLum;
uniform float middleGrey;
uniform float threshold; //orig 32
// 1st downscale stage of making the bloom texture
// only takes bits of fbo that are bright enough

#if 0
void main(void)
{
	const vec3 getlum = vec3(0.299, 0.587, 0.114);
	vec3 col = vec3(0.0);
	vec2 p = 4.0*gl_FragCoord.xy;
	vec3 c;
	float lum;
	float minLumToBloom = max((threshold*32.0)*avgLum, 1.0);
	
	c = vec3(texture2DRect(fboTex, vec2(p.x, p.y)));
	lum = dot(getlum, c);
	if (lum > minLumToBloom) col += c;// *(lum-minLumToBloom);
	// This bit causes gradual onset of bloom    ^^^^^^^^^
	
	c = vec3(texture2DRect(fboTex, vec2(p.x+1.0, p.y)));
	lum = dot(getlum, c);
	if (lum > minLumToBloom) col += c;// *(lum-minLumToBloom);
	
	c = vec3(texture2DRect(fboTex, vec2(p.x+1.0, p.y+1.0)));
	lum = dot(getlum, c);
	if (lum > minLumToBloom) col += c;// *(lum-minLumToBloom);
	
	c = vec3(texture2DRect(fboTex, vec2(p.x, p.y+1.0)));
	lum = dot(getlum, c);
	if (lum > minLumToBloom) col += c;// *(lum-minLumToBloom);
	
	col *= 0.25;
	gl_FragColor = vec4(col.r, col.g, col.b, 0.0);
}
#else
//let's try this again...
//~ const float threshold = 5.0;
const float brightOffset = 10.0; // Offset for BrightPass filter - separate light sources from lit objects

vec4 downsample(sampler2DRect tex, vec2 coord)
{
	vec4 c = texture2DRect(tex, coord);
	c += texture2DRect(tex, coord+vec2(1.0,0.0));
	c += texture2DRect(tex, coord+vec2(1.0,1.0));
	c += texture2DRect(tex, coord+vec2(0.0,1.0));
	c *= 0.25;
	return c;
}

void main(void)
{
	vec2 coord = gl_FragCoord.xy*4.0;
	vec3 vColor = downsample(fboTex, coord).rgb;
	float fLum = avgLum;
	vColor *= middleGrey / (fLum + 0.001);
	vColor = max(vec3(0.0), vColor - threshold);
	vColor /= (brightOffset + vColor); //map to 0.0-1.0
	gl_FragColor = vec4(vColor, 1.0);
}
#endif
