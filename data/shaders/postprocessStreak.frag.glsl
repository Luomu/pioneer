#extension GL_ARB_texture_rectangle : enable
uniform sampler2DRect fboTex;
uniform int iteration;
uniform vec2 direction;
uniform float attenuation;
varying vec4 color;

#define NUM_SAMPLES 4

void main()
{
	vec2 sampleCoord = vec2(0.0);
	vec3 cOut = vec3(0.0);
	
	vec2 pxSize = vec2(0.15);

	float b = pow(float(NUM_SAMPLES), float(iteration));
	float sf = 0.0;
	
	float fattenuation = 0.99;

	for(int s = 0; s < NUM_SAMPLES; s++)
	{
		sf = float(s);
		float weight = pow(fattenuation, b * sf) * 0.5;

		sampleCoord = gl_FragCoord.xy + (direction * b * vec2(sf) * pxSize);
		/*if(iteration == 1)
			sampleCoord /= 0.5;*/

		cOut += weight * texture2DRect(fboTex, sampleCoord).rgb;
		//cOut = clamp(cOut, 0.0, 1.0);
	}

	vec3 streak = clamp(cOut, 0.0, 1.0);
	gl_FragColor = vec4(streak, 1.0);
}
