#version 120
uniform sampler2D texture0;
uniform float sampleDist;

varying vec2 texCoord;
varying vec2 texCoord_c;

void main(void)
{
	vec2 tc = texCoord_c;
	vec4 col = texture2D(texture0, vec2(tc.x, tc.y))*0.5;
	col += texture2D(texture0, vec2(tc.x, tc.y-sampleDist*4.0)) * 0.00390625;
	col += texture2D(texture0, vec2(tc.x, tc.y-sampleDist*3.0)) * 0.015625;
	col += texture2D(texture0, vec2(tc.x, tc.y-sampleDist*2.0)) * 0.0625;
	col += texture2D(texture0, vec2(tc.x, tc.y-sampleDist*1.0)) * 0.25;
	col += texture2D(texture0, vec2(tc.x, tc.y+sampleDist*4.0)) * 0.00390625;
	col += texture2D(texture0, vec2(tc.x, tc.y+sampleDist*3.0)) * 0.015625;
	col += texture2D(texture0, vec2(tc.x, tc.y+sampleDist*2.0)) * 0.0625;
	col += texture2D(texture0, vec2(tc.x, tc.y+sampleDist*1.0)) * 0.25;
	gl_FragColor = col;
}
