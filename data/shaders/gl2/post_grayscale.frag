varying vec2 uv0;
uniform sampler2D texture0;
uniform sampler2D texture1;

//uniform vec2 texelSize;

void main(void)
{
	vec2 texelSize = vec2(1.0/1024.0, 1.0/768.0);
	vec4 tc = texture2D(texture0, uv0);
	tc += texture2D(texture0, uv0 + texelSize * vec2(3.0, 0.0));
	tc += texture2D(texture0, uv0 + texelSize * vec2(3.0, 3.0));
	tc += texture2D(texture0, uv0 + texelSize * vec2(0.0, 3.0));
	tc *= 0.25;
	float gray = dot(tc.rgb, vec3(0.299, 0.587, 0.114));
	gl_FragColor = texture2D(texture1, uv0 * 10.0) * gray;
}
