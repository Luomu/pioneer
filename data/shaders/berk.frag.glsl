uniform sampler2D texture0;

varying vec2 texCoord;

void main(void)
{
	//vec3 col = texture2D(texture0, texCoord).rgb;
	//gl_FragColor = vec4(col.r, col.g, col.b, 1.0);
	gl_FragColor = texture2D(texture0, texCoord);
}
