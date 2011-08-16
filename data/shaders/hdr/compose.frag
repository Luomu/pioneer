#version 120

uniform sampler2D sceneTexture;
uniform sampler2D luminanceTexture;
uniform float     luminanceBias;
uniform float     key;
//~ uniform float avgLum;
//~ uniform float middleGrey;

const float LuminanceSaturation = 1.0;
const float WhiteLevel = 1.0;

varying vec2 texCoord;

float log10(float v)
{
	return log2(v)/log2(10);
}

//retrieve log-average luminance from the luminance texture
float getAvgLuminance()
{
	return exp(texture2D(luminanceTexture, texCoord, luminanceBias * 5.0).x);
}

//approximates luminance from an RGB value
float calcLuminance(vec3 color)
{
	return max(dot(color, vec3(0.299, 0.587, 0.114)), 0.0001f);
}

//exposure calculation
vec3 calcExposedColor(vec3 color, float avgLuminance)
{
	avgLuminance = max(avgLuminance, 0.001);
	float exposure = 0.0;
	float keyValue = key; //0.18
	//~ float keyValue = 1.03 - (2.0 / (2.0 + log10(avgLuminance + 1.0)));
	float linearExposure = (keyValue / avgLuminance);
	exposure = log2(max(linearExposure, 0.0001));
	return exp2(exposure) * color;
}

vec3 toneMapFilmicALU(vec3 color)
{
    color = max(vec3(0.0), color - 0.004);
    color = (color * (6.2 * color + 0.5)) / (color * (6.2 * color + 1.7)+ 0.06);

    // result has 1/2.2 baked in
    return pow(color, vec3(2.2));
}

vec3 toneMapDragoLogarithmic(vec3 color)
{
	float Bias = 0.5;
	float pixelLuminance = calcLuminance(color);    
    float toneMappedLuminance = log10(1.0 + pixelLuminance);
	toneMappedLuminance /= log10(1.0 + WhiteLevel);
	toneMappedLuminance /= log10(2.0 + 8.0 * ((pixelLuminance / WhiteLevel) * log10(Bias) / log10(0.5)));
	return toneMappedLuminance * pow(color / pixelLuminance, vec3(LuminanceSaturation)); 
}

vec3 toneMapExponential(vec3 color)
{
	float pixelLuminance = calcLuminance(color);  
	float toneMappedLuminance = 1.0 - exp(-pixelLuminance / WhiteLevel);
	return toneMappedLuminance * pow(color / pixelLuminance, vec3(LuminanceSaturation));
}

vec3 toneMapReinhard(vec3 color)
{
	float pixelLuminance = calcLuminance(color);
	float toneMappedLuminance = pixelLuminance / (pixelLuminance + 1.0);
	return toneMappedLuminance * pow(color / pixelLuminance, vec3(LuminanceSaturation));
}

vec3 toneMapReinhardAlternative(vec3 color)
{
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
	col = toneMapFilmicALU(col);
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
