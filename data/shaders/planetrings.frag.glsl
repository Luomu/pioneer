uniform sampler2D texture0;
uniform sampler2D texture1;
varying vec2 texCoord0;
varying vec3 vertexPos;
varying vec3 ecPos;

void main(void)
{
	// Bits of ring in shadow!
	vec4 col = vec4(0.0);
	vec4 texCol = texture2D(texture0, texCoord0);

	for (int i=0; i<NUM_LIGHTS; ++i) {
		float l = findSphereEyeRayEntryDistance(-vertexPos, vec3(gl_ModelViewMatrixInverse * gl_LightSource[i].position), 1.0);
		if (l <= 0.0) {
			col = col + texCol*gl_LightSource[i].diffuse;
		}
	}
	col.a = texCol.a;

	//noise detail texturing
	float detailDist = length(ecPos);
	vec2 noiseCoord = vertexPos.xz;
	float coarseNoise = texture2D(texture1, noiseCoord * 100.0).r;
	float fineNoise = texture2D(texture1, noiseCoord * 2000.0).r; //finer detail

	float coarseDistance = clamp((200000.0 - detailDist) / (200000.0 - 5000.0), 0.0, 1.0);
	float fineDistance = pow(coarseDistance, 8.0);

	//mix between fine detail, coarse detail and white
	float detail1 = mix(1.0, coarseNoise, coarseDistance);
	float detail2 = mix(detail1, fineNoise, fineDistance);
	gl_FragColor = col * detail2;

	SetFragDepth();
}
