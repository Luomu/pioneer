//
// Atmospheric scattering vertex shader
//
// Original code: Copyright (c) 2004 Sean O'Neil
//
// http://http.developer.nvidia.com/GPUGems2/gpugems2_chapter16.html
//

uniform vec3 lightPos;
//~ uniform float g;
//~ uniform float g2;

varying vec3 v3Direction;


void main (void)
{
	float g = -0.95f;
	float g2 = g * g;
	float fCos = dot(lightPos, v3Direction) / length(v3Direction);
	float fRayleighPhase = 0.75 * (1.0 + fCos*fCos);
	float fMiePhase = 1.5 * ((1.0 - g2) / (2.0 + g2)) * (1.0 + fCos*fCos) / pow(1.0 + g2 - 2.0*g*fCos, 1.5);
	gl_FragColor = fRayleighPhase * gl_Color + fMiePhase * gl_SecondaryColor;
	//gl_FragColor.a = gl_FragColor.b;
#ifdef ZHACK
	SetFragDepth(gl_TexCoord[6].z);
#endif
}

