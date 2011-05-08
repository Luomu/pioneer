//
// Atmospheric scattering vertex shader
//
// Original code: Copyright (c) 2004 Sean O'Neil
//
// http://http.developer.nvidia.com/GPUGems2/gpugems2_chapter16.html
//

uniform vec3 cameraPos;		// The camera's current position
uniform vec3 lightPos;		// The direction vector to the light source
vec3 v3InvWavelength;	// 1 / pow(wavelength, 4) for the red, green, and blue channels
float fCameraHeight;	// The camera's current height
float fCameraHeight2;	// fCameraHeight^2
float fOuterRadius;		// The outer (atmosphere) radius
float fOuterRadius2;	// fOuterRadius^2
uniform float innerRadius;		// The inner (planetary) radius
float fInnerRadius2;	// fInnerRadius^2
float fKrESun;	 // Kr * ESun
float fKmESun;	 // Km * ESun
float fKr4PI;	 // Kr * 4 * PI
float fKm4PI;	 // Km * 4 * PI
float fScale;	 // 1 / (fOuterRadius - fInnerRadius)
float fScaleDepth;		// The scale depth (i.e. the altitude at which the atmosphere's average density is found)
float fScaleOverScaleDepth;	// fScale / fScaleDepth

int nSamples;
float fSamples;

varying vec4 primaryColor;
varying vec4 secondaryColor;

#define PI 3.14159265358979323846


float scale(float fCos)
{
	float x = 1.0 - fCos;
	return fScaleDepth * exp(-0.00287 + x*(0.459 + x*(3.83 + x*(-6.80 + x*5.25))));
}

void main(void)
{
	//todo: calculate these outside shader
	cameraPos = vec3(-cameraPos.x, -cameraPos.y, -cameraPos.z);
	fCameraHeight = length(cameraPos);
	fCameraHeight2 = fCameraHeight * fCameraHeight;

	innerRadius = 1.0;
	fOuterRadius = innerRadius * 1.025;
	fInnerRadius2 = innerRadius * innerRadius;
	fOuterRadius2 = innerRadius * 1.025 * innerRadius * 1.025;
	
	vec3 wl = vec3(0.650, 0.570, 0.475);
	v3InvWavelength.x = 1.0/pow(wl.x, 4.0);
	v3InvWavelength.y = 1.0/pow(wl.y, 4.0);
	v3InvWavelength.z = 1.0/pow(wl.z, 4.0);

	float Kr = 0.0025;
	float Km = 0.0015;
	float ESun = 15.0;
	float mieG = -0.95;
	fKrESun = Kr * ESun;
	fKmESun = Km * ESun;
	fKr4PI = Kr * 4.0 * PI;
	fKm4PI = Km * 4.0 * PI;
	fSamples = 2.0;
	nSamples = 2;
	fScale = 1.0 / (fOuterRadius - innerRadius);
	fScaleDepth = 0.25;
	fScaleOverScaleDepth = (1.0 / (fOuterRadius - innerRadius)) / fScaleDepth;

	// Get the ray from the camera to the vertex and its length (which is the far point of the ray passing through the atmosphere)
	vec3 v3Pos = gl_Vertex.xyz;
	vec3 v3Ray = v3Pos - cameraPos;
	float fFar = length(v3Ray);
	v3Ray /= fFar;

#ifdef SPACE
	// Calculate the closest intersection of the ray with the outer atmosphere (which is the near point of the ray passing through the atmosphere)
	float B = 2.0 * dot(cameraPos, v3Ray);
	float C = fCameraHeight2 - fOuterRadius2;
	float fDet = max(0.0, B*B - 4.0 * C);
	float fNear = 0.5 * (-B - sqrt(fDet));
#endif

	// Calculate the ray's starting position, then calculate its scattering offset
#ifdef SPACE
	vec3 v3Start = cameraPos + v3Ray * fNear;
	fFar -= fNear;
	float fDepth = exp((innerRadius - fOuterRadius) / fScaleDepth);
#else
	vec3 v3Start = cameraPos;
	float fDepth = exp((innerRadius - fCameraHeight) / fScaleDepth);
#endif
	float fCameraAngle = dot(-v3Ray, v3Pos) / length(v3Pos);
	float fLightAngle = dot(lightPos, v3Pos) / length(v3Pos);
	float fCameraScale = scale(fCameraAngle);
	float fLightScale = scale(fLightAngle);
	float fCameraOffset = fDepth*fCameraScale;
	float fTemp = (fLightScale + fCameraScale);

	// Initialize the scattering loop variables
	float fSampleLength = fFar / fSamples;
	float fScaledLength = fSampleLength * fScale;
	vec3 v3SampleRay = v3Ray * fSampleLength;
	vec3 v3SamplePoint = v3Start + v3SampleRay * 0.5;

	// Now loop through the sample rays
	vec3 v3FrontColor = vec3(0.0, 0.0, 0.0);
	vec3 v3Attenuate = vec3(0.0);
	for(int i=0; i<nSamples; i++)
	{
		float fHeight = length(v3SamplePoint);
		float fDepth = exp(fScaleOverScaleDepth * (innerRadius - fHeight));
		float fScatter = fDepth*fTemp - fCameraOffset;
		v3Attenuate = exp(-fScatter * (v3InvWavelength * fKr4PI + fKm4PI));
		v3FrontColor += v3Attenuate * (fDepth * fScaledLength);
		v3SamplePoint += v3SampleRay;
	}

	primaryColor.rgb = v3FrontColor * (v3InvWavelength * fKrESun + fKmESun);

	// Calculate the attenuation factor for the ground
	secondaryColor.rgb = v3Attenuate;
	
	gl_FrontColor = gl_Color;

	gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;
	gl_TexCoord[1] = gl_TextureMatrix[1] * gl_MultiTexCoord1;
#ifdef ZHACK
	gl_Position = logarithmicTransform();
#else
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
#endif
}
