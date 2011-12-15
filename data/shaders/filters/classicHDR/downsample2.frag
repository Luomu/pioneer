#version 120
uniform sampler2D texture0;
uniform vec2 viewportSize;
varying vec2 texCoord;

void main(void)
{
	//viewport is the size of the target
	vec2 texel_size = vec2(1.0/(viewportSize.x), 1.0/(viewportSize.y));
	float dist = 2.0;
	vec4 color =
			 texture2D(texture0, texCoord + vec2(0.0,  0.0 ) * texel_size);
	color += texture2D(texture0, texCoord + vec2(0.0,  dist) * texel_size);
	color += texture2D(texture0, texCoord + vec2(dist, dist) * texel_size);
	color += texture2D(texture0, texCoord + vec2(dist, 0.0 ) * texel_size);
	gl_FragColor = color * 0.25;
}
