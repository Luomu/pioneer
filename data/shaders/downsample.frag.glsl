#extension GL_ARB_texture_rectangle : enable
uniform sampler2DRect Texture0;
uniform float scale;

#if 0
//overkill?
#define SAMPLES 16
vec4 downsample(sampler2DRect tex, vec2 p)
{
        vec4 sum = vec4(0.0);
        
        vec2 offset[16] = {
            vec2(-1.5,1.5),vec2(-0.5,1.5),vec2(0.5,1.5),vec2(1.5,1.5),
            vec2(-1.5,0.5),vec2(-0.5,0.5),vec2(0.5,0.5),vec2(1.5,0.5),
            vec2(-1.5,-0.5),vec2(-0.5,-0.5),vec2(0.5,-0.5),vec2(1.5,-0.5),
            vec2(-1.5,-1.5),vec2(-0.5,-1.5),vec2(0.5,-1.5),vec2(1.5,-1.5)
        };

        for(int i=0; i<SAMPLES;i++) {
            sum += texture2DRect(tex, p+offset[i]);
        }

        return sum * 1.0/SAMPLES;
}
#else
vec4 downsample(sampler2DRect tex, vec2 p)
{
    //four-point downsample
    float dist = 2.0;

    vec4 col = texture2DRect(tex, vec2(p.x-dist/2.0, p.y-dist/2.0));
    col += texture2DRect(tex, vec2(p.x+dist/2.0, p.y-dist/2.0));
    col += texture2DRect(tex, vec2(p.x+dist/2.0, p.y+dist/2.0));
    col += texture2DRect(tex, vec2(p.x-dist/2.0, p.y+dist/2.0));
    
    return col / 4;
}
#endif

void main(void)
{
    vec4 col;
    vec2 p = gl_FragCoord.xy * scale;
    
    gl_FragColor = downsample(Texture0, p);
}
