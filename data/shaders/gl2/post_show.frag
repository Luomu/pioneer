varying vec2 uv0;
uniform sampler2D texture0;

void main(void)
{
	gl_FragColor = texture2D(texture0, uv0);
}
