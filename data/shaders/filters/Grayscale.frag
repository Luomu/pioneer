#version 120
uniform sampler2D texture0;

varying vec2 texCoord;

void main(void)
{
	//http://www.cacs.louisiana.edu/~cice/lal/index.html
	vec4 col = texture2D(texture0, texCoord);
	float intensity = dot(col, vec4(0.299, 0.587, 0.114, 0.0));
	gl_FragColor = vec4(intensity, intensity, intensity, col.a);
}
