#extension GL_ARB_texture_rectangle : enable
uniform sampler2DRect Texture0; //scene
//~ uniform sampler2DRect Texture1; //1/4 size glow
//~ uniform sampler2DRect Texture2; //1/8 size glow
//~ uniform sampler2DRect Texture3; //1/16 size glow
//~ uniform sampler2DRect Texture4; //1/32 size glow
uniform sampler2D Texture1; //temp glow
uniform sampler2D Texture2; //temp streaks

#if 0
vec4 upsample(sampler2DRect tex, vec2 coord)
{
    coord -= 0.5;
    float dist = 0.5;
    vec4 tl = texture2DRect(tex, coord);
    vec4 tr = texture2DRect(tex, coord + vec2(dist, 0.0));
    vec4 bl = texture2DRect(tex, coord + vec2(0.0, dist));
    vec4 br = texture2DRect(tex, coord + vec2(dist, dist));
    vec4 tA = mix(tl, tr, 0.5);
    vec4 tB = mix(bl, br, 0.5);
    return mix(tA, tB, 0.5);
}

void main(void)
{
    vec2 scale = vec2(4.0); //vec2(256.0) / vec2(1024.0, 768.0);
    vec2 coord = gl_FragCoord.xy / scale;
    vec4 scenecolor = texture2DRect(Texture0, gl_FragCoord.xy);
    vec4 glowcolor = upsample(Texture1, coord);
    vec4 glowcolor2 = upsample(Texture2, coord / 2.0);
    vec4 glowcolor3 = upsample(Texture3, coord / 4.0);
    vec4 glowcolor4 = upsample(Texture4, coord / 8.0);
    gl_FragColor = scenecolor + (glowcolor + glowcolor2 + glowcolor3 + glowcolor4) * 0.25;
}
#else
void main(void)
{
    vec4 scenecolor = texture2DRect(Texture0, gl_FragCoord.xy);
    vec4 glowcolor = texture2D(Texture1, gl_TexCoord[0].xy);
    vec4 streakcolor = texture2D(Texture2, gl_TexCoord[0].xy);
    gl_FragColor = scenecolor + glowcolor * 0.7 + streakcolor * 0.6;
}
#endif
