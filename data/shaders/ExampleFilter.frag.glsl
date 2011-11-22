uniform sampler2D fboTex;

varying vec2 texCoord;

vec4 Desaturate(vec3 color, float Desaturation)
{
	vec3 grayXfer = vec3(0.3, 0.59, 0.11);
	vec3 gray = vec3(dot(grayXfer, color));
	return vec4(mix(color, gray, Desaturation), 1.0);
}

vec4 poop(sampler2D tImage0, vec2 tc)
{
	float	Weights[9];
	vec2	Coord[9];

	float fBias = 0.0025f;
	Coord[0].x = tc.x-fBias;
	Coord[0].y = tc.y-fBias;
	Coord[1].x = tc.x;
	Coord[1].y = tc.y-fBias;
	Coord[2].x = tc.x+fBias;
	Coord[2].y = tc.y-fBias;
	Coord[3].x = tc.x-fBias;
	Coord[3].y = tc.y;
	Coord[4].x = tc.x;
	Coord[4].y = tc.y;
	Coord[5].x = tc.x+fBias;
	Coord[5].y = tc.y;
	Coord[6].x = tc.x-fBias;
	Coord[6].y = tc.y+fBias;
	Coord[7].x = tc.x;
	Coord[7].y = tc.y+fBias;
	Coord[8].x = tc.x+fBias;
	Coord[8].y = tc.y+fBias;

	Weights[0] = 0.0625f;
	Weights[1] = 0.125f;
	Weights[2] = 0.0625f;
	Weights[3] = 0.125f;
	Weights[4] = 0.25f;
	Weights[5] = 0.125f;
	Weights[6] = 0.0625f;
	Weights[7] = 0.125f;
	Weights[8] = 0.0625f;

	vec4 Color = vec4(0,0,0,0);
	Color += texture2D( tImage0, Coord[0] ) * Weights[0];
	Color += texture2D( tImage0, Coord[1] ) * Weights[1];
	Color += texture2D( tImage0, Coord[2] ) * Weights[2];
	Color += texture2D( tImage0, Coord[3] ) * Weights[3];
	Color += texture2D( tImage0, Coord[4] ) * Weights[4];
	Color += texture2D( tImage0, Coord[5] ) * Weights[5];
	Color += texture2D( tImage0, Coord[6] ) * Weights[6];
	Color += texture2D( tImage0, Coord[7] ) * Weights[7];
	Color += texture2D( tImage0, Coord[8] ) * Weights[8];
	return Color;
}

void main(void)
{
	//gl_FragColor = Desaturate(texture2D(fboTex, texCoord).rgb, 0.9);
	gl_FragColor = poop(fboTex, texCoord);
}
