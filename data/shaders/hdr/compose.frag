#version 120

uniform sampler2D sceneTexture;
uniform sampler2D luminanceTexture;
uniform float     luminanceBias;
//~ uniform float avgLum;
//~ uniform float middleGrey;

varying vec2 texCoord;

//retrieve log-average luminance from the luminance texture
float getAvgLuminance()
{
	return exp(texture2D(luminanceTexture, texCoord, luminanceBias * 2.0).x);
}

//approximates luminance from an RGB value
float calcLuminance(vec3 color)
{
	return max(dot(color, vec3(0.299, 0.587, 0.114)), 0.0001f);
}

//exposure calculation
vec3 calcExposedColor(vec3 color, float avgLuminance)
{
	float keyValue = 1.0;
	float exposure = avgLuminance;
	float linearExposure = (keyValue / avgLuminance);
	exposure = log2(max(linearExposure, 0.0001f));
	return exp2(exposure) * color;
}

vec3 toneMapExponential(vec3 color)
{
	float WhiteLevel = 1.0;
	float LuminanceSaturation = 1.0;
	float pixelLuminance = calcLuminance(color);  
	float toneMappedLuminance = 1.0 - exp(-pixelLuminance / WhiteLevel);
	return toneMappedLuminance * pow(color / pixelLuminance, vec3(LuminanceSaturation));
}

vec3 toneMapReinhard(vec3 color)
{
	float pixelLuminance = calcLuminance(color);
	float toneMappedLuminance = pixelLuminance / (pixelLuminance + 1.0);
	return toneMappedLuminance * pow(color / pixelLuminance, vec3(1.0));
}

vec3 toneMapReinhardAlternative(vec3 color)
{
	float WhiteLevel = 1.0;
	float LuminanceSaturation = 1.0;
	float pixelLuminance = calcLuminance(color);
	float toneMappedLuminance = pixelLuminance
		* (1.0f + pixelLuminance / (WhiteLevel * WhiteLevel))
		/ (1.0f + pixelLuminance);
	return toneMappedLuminance * pow(color / pixelLuminance, vec3(LuminanceSaturation));
}

void main(void)
{

	float avgLuminance = getAvgLuminance();
	vec3 col = vec3(texture2D(sceneTexture, texCoord));
#if 1
	col = calcExposedColor(col, avgLuminance);
	col = toneMapReinhardAlternative(col);
#else
	float avgLum = avgLuminance;
	float middleGrey = 1.8;
	// This is the reinhard tonemapping algorithm, i think...
	//convert to CIE XYZ color space
	float X,Y,Z,x,y;
	X = dot(vec3(0.4124, 0.3576, 0.1805), col);
	Y = dot(vec3(0.2126, 0.7152, 0.0722), col);
	Z = dot(vec3(0.0193, 0.1192, 0.9505), col);
	x = X / (X+Y+Z);
	y = Y / (X+Y+Z);
	// Y is luminance. fiddle with it
	Y = Y * (middleGrey / avgLum); // scale luminance
//	Y *= lum;
	Y = Y / (1.0 + Y); // compress luminance
	// convert back to RGB
	X = x*(Y/y);
	Z = (1.0-x-y)*(Y/y);
	col.r =  3.2405 *X - 1.5371*Y - 0.4985*Z;
	col.g = -0.9693 *X + 1.8760*Y + 0.0416*Z;
	col.b =  0.0556 *X - 0.2040*Y + 1.0572*Z;
#endif
	gl_FragColor = vec4(col.r, col.g, col.b, 1.0);
}
