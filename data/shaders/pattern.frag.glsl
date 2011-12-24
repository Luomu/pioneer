varying vec3 norm;
uniform sampler2D tex;
uniform sampler2D texGlow;
uniform bool usetex;
uniform bool useglow;

uniform sampler1D colorMap;
uniform sampler2D patternMap;

void main(void)
{
	vec3 tnorm = normalize(norm);
	vec4 amb = vec4(0.0);
	vec4 diff = vec4(0.0);
	vec4 spec = vec4(0.0);
	for (int i=0; i<NUM_LIGHTS; ++i) {
		float nDotVP; 
		float nDotHV;         
		float pf;             
		nDotVP = max(0.0, dot(tnorm, normalize(vec3(gl_LightSource[i].position))));
		nDotHV = max(0.0, dot(tnorm, vec3(gl_LightSource[i].halfVector)));
		if (nDotVP == 0.0) pf = 0.0;
		else pf = pow(nDotHV, gl_FrontMaterial.shininess);
		amb += gl_LightSource[i].ambient;
		diff += gl_LightSource[i].diffuse * nDotVP;
		spec += gl_LightSource[i].specular * pf;
	}
#if 0
	vec4 emission = gl_FrontMaterial.emission;
	if ( useglow )
		emission = texture2D(texGlow, gl_TexCoord[0].st);

	gl_FragColor = 
		gl_LightModel.ambient * gl_FrontMaterial.ambient +
		amb * gl_FrontMaterial.ambient +
		diff * gl_FrontMaterial.diffuse +
		spec * gl_FrontMaterial.specular +
		emission;
	gl_FragColor.w = gl_FrontMaterial.diffuse.w;
	if ( usetex )
		gl_FragColor *= texture2D(tex, gl_TexCoord[0].st);
#endif

	vec2 texCoord = gl_TexCoord[0].st;
	vec4 base     = texture2D(tex, texCoord) * gl_FrontMaterial.diffuse;
	vec4 pat      = texture2D(patternMap, texCoord);
	vec4 col      = texture1D(colorMap, pat.r);
	vec4 emit     = texture2D(texGlow, texCoord);
	gl_FragColor  = diff * (base * col) + (spec * 0.2) + emit;
	SetFragDepth(gl_TexCoord[6].z);
}