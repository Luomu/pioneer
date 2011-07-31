varying vec4 color;
varying vec2 texcoord;
uniform sampler2D tex;

void main(void)
{
	vec4 col = texture2D(tex, texcoord);
	const float dist = 0.05;
	col += texture2D(tex, texcoord + vec2(dist, 0.0));
	col += texture2D(tex, texcoord + vec2(dist, dist));
	col += texture2D(tex, texcoord + vec2(0.0, dist));
	col *= 0.25;
	gl_FragColor = col;
}
