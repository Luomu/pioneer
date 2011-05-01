varying vec4 color;
#extension GL_ARB_texture_rectangle : enable
uniform sampler2DRect Texture0; //1/4 size glow
uniform sampler2DRect Texture1; //1/8 size glow
uniform sampler2DRect Texture2; //1/16 size glow
uniform sampler2DRect Texture3; //1/32 size glow
uniform sampler2DRect Texture4; //1/64 size glow

vec4 zupsample(sampler2DRect tex, vec2 coord)
{
    return texture2DRect(tex, coord);
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

vec4 upsample(sampler2DRect tex, vec2 coord)
{
    vec4 sum = texture2DRect(tex, coord+vec2(0.0,0.0));
    sum += texture2DRect(tex, coord+vec2(0.5,0.0));
    sum += texture2DRect(tex, coord+vec2(0.5,0.5));
    sum += texture2DRect(tex, coord+vec2(0.0,0.5));
    return sum * 0.25;
}

void main(void)
{
    //add four different sized glows together
    //upsample might be unnecessary now that composite is no longer rectangletex?
    vec2 scale = vec2(1.0); //vec2(256.0) / vec2(1024.0, 768.0);
    vec2 coord = gl_FragCoord.xy / scale;
    vec4 glowcolor, glowcolor2, glowcolor3, glowcolor4, glowcolor5;
    float[5] weights = {0.8,0.5,0.4,0.001,0.000};
    glowcolor = upsample(Texture0, coord) * weights[0];
    glowcolor2 = upsample(Texture1, coord / 2.0) * weights[1];
    glowcolor3 = upsample(Texture2, coord / 4.0) * weights[2];
    glowcolor4 = upsample(Texture3, coord / 8.0) * weights[3];
    glowcolor5 = upsample(Texture4, coord / 16.0) * weights[4];
    vec4 final = glowcolor + glowcolor2 + glowcolor3 + glowcolor4 + glowcolor5;
    gl_FragColor = final;
}
