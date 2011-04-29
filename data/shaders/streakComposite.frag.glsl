#extension GL_ARB_texture_rectangle : enable
uniform sampler2DRect Texture0;
uniform sampler2DRect Texture1;
uniform sampler2DRect Texture2;
uniform sampler2DRect Texture3;

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
    //add four streaks together
    //upsample might be unnecessary now that composite is no longer rectangletex?
    vec2 scale = vec2(1.0);
    vec2 coord = gl_FragCoord.xy / scale;
    vec4 glowcolor, glowcolor2, glowcolor3, glowcolor4;
    glowcolor = upsample(Texture0, coord) * 0.25;
    glowcolor2 = upsample(Texture1, coord) * 0.25;
    glowcolor3 = upsample(Texture2, coord) * 0.25;
    glowcolor4 = upsample(Texture3, coord) * 0.25;

    vec4 final = glowcolor + glowcolor2 + glowcolor3 + glowcolor4;
    gl_FragColor = final;
}
