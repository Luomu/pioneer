#version 120
uniform sampler2D sceneTex;
uniform float avgLum;

varying vec2 texCoord;

void main(void)
{
	const vec3 getlum = vec3(0.299, 0.587, 0.114);
	vec3 col = vec3(0.0);
	vec2 p = texCoord;
	vec3 c;
	float lum;
	float minLumToBloom = max(32.0*avgLum, 1.0);

	c = texture2D(sceneTex, vec2(p.x, p.y)).rgb;
	lum = dot(getlum, c);
	if (lum > minLumToBloom) col += c;//*(lum-minLumToBloom);
#if 0
	// This bit causes gradual onset of bloom    ^^^^^^^^^

	c = texture2D(sceneTex, vec2(p.x+0.01, p.y)).rgb;
	lum = dot(getlum, c);
	if (lum > minLumToBloom) col += c;//*(lum-minLumToBloom);

	c = texture2D(sceneTex, vec2(p.x+0.01, p.y+0.01)).rgb;
	lum = dot(getlum, c);
	if (lum > minLumToBloom) col += c;//*(lum-minLumToBloom);

	c = texture2D(sceneTex, vec2(p.x, p.y+0.01)).rgb;
	lum = dot(getlum, c);
	if (lum > minLumToBloom) col += c;//*(lum-minLumToBloom);

	col *= 0.25;
#endif
	gl_FragColor = vec4(col.r, col.g, col.b, 0.0);
}
