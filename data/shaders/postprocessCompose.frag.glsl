#extension GL_ARB_texture_rectangle : enable
uniform sampler2DRect fboTex;
uniform sampler2D bloomTex;
uniform sampler2D streakTex;
uniform float avgLum;
uniform float middleGrey;

void main(void)
{
	vec3 col = vec3(texture2DRect(fboTex, vec2(gl_FragCoord.x, gl_FragCoord.y)));
	vec3 streaks = vec3(texture2D(streakTex, gl_TexCoord[0].xy));
	vec3 glow = vec3(texture2D(bloomTex, gl_TexCoord[0].xy));
	col += glow * 0.3;
	col += streaks;

#if 1
	// This is the reinhard tonemapping algorithm, i think...
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
	col.r = 3.2405*X - 1.5371*Y - 0.4985*Z;
	col.g = -0.9693*X + 1.8760*Y + 0.0416*Z;
	col.b = 0.0556*X - 0.2040*Y + 1.0572*Z;
#endif
	col += glow * 0.3;

	gl_FragColor = vec4(col, 1.0);
}
