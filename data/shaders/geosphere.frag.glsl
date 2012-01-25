uniform vec4 atmosColor;
// to keep distances sane we do a nearer, smaller scam. this is how many times
// smaller the geosphere has been made
uniform float geosphereScale;
uniform float geosphereAtmosTopRad;
uniform vec3 geosphereCenter;
uniform float geosphereAtmosFogDensity;

uniform sampler2D texture0;
uniform sampler2D texture1;
varying vec2 texCoord0;

varying float dist;

void main(void)
{
	vec4 hidetail = texture2D(texture0, texCoord0 * 16.0);
	vec4 lodetail = texture2D(texture1, texCoord0 * 2.0);
	vec3 eyepos = vec3(gl_TexCoord[0]);
	vec3 tnorm = normalize(vec3(gl_TexCoord[1]));
	vec4 diff = vec4(0.0);

	//float fog = exp(-0.005 * dist);
	//float fog2 = exp(-0.001 * dist);
	float fog = exp(-0.004 * dist);
	float fog2 = exp(-0.001 * dist);
	//vec4 fogcol = mix(texcol, vec4(1.0), fog);
	//hidetail = vec4(1.0, 0.0, 0.0, 1.0);
	//lodetail = vec4(0.0, 0.0, 1.0, 0.0);
	vec4 fogcol1 = mix(lodetail, hidetail, fog);
	vec4 fogcol = mix(vec4(1.0), fogcol1, fog2);
	
	for (int i=0; i<NUM_LIGHTS; ++i) {
		float nDotVP = max(0.0, dot(tnorm, normalize(vec3(gl_LightSource[i].position))));
		diff += gl_LightSource[i].diffuse * nDotVP;
	}

	// when does the eye ray intersect atmosphere
	float atmosStart = findSphereEyeRayEntryDistance(geosphereCenter, eyepos, geosphereAtmosTopRad);
	
	float fogFactor;
	{
		float atmosDist = geosphereScale * (length(eyepos) - atmosStart);
		float ldprod;
		vec3 dir = normalize(eyepos);
		vec3 a = (atmosStart * dir - geosphereCenter) / geosphereAtmosTopRad;
		vec3 b = (eyepos - geosphereCenter) / geosphereAtmosTopRad;
		ldprod = AtmosLengthDensityProduct(a, b, atmosColor.w*geosphereAtmosFogDensity, atmosDist);
		fogFactor = 1.0 / exp(ldprod);
	}

	vec4 atmosDiffuse = vec4(0.0,0.0,0.0,1.0);
	{
		vec3 surfaceNorm = normalize(eyepos - geosphereCenter);
		for (int i=0; i<NUM_LIGHTS; ++i) {
			atmosDiffuse += gl_LightSource[i].diffuse * max(0.0, dot(surfaceNorm, normalize(vec3(gl_LightSource[i].position))));
		}
	}
	atmosDiffuse.a = 1.0;
//	float sun = dot(normalize(eyepos),normalize(vec3(gl_LightSource[0].position)));
	vec4 final = gl_Color * fogcol;
	gl_FragColor = (fogFactor)*(diff)*final + gl_LightModel.ambient*gl_Color +
		(1.0-fogFactor)*(atmosDiffuse*atmosColor) + gl_FrontMaterial.emission;

#ifdef ZHACK
	SetFragDepth(gl_TexCoord[6].z);
#endif
}
