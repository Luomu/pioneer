#extension GL_ARB_texture_rectangle : enable
uniform sampler2DRect Texture0;
uniform float scale;

void main(void)
{
    vec4 col;
    vec2 p = gl_FragCoord * scale;
    
    //four-point downsample
    float dist = 2.0;

    //~ col = texture2DRect(Texture0, vec2(p.x, p.y));
    //~ col += texture2DRect(Texture0, vec2(p.x+dist, p.y));
    //~ col += texture2DRect(Texture0, vec2(p.x+dist, p.y+dist));
    //~ col += texture2DRect(Texture0, vec2(p.x, p.y+dist));

    col = texture2DRect(Texture0, vec2(p.x-dist/2.0, p.y-dist/2.0));
    col += texture2DRect(Texture0, vec2(p.x+dist/2.0, p.y-dist/2.0));
    col += texture2DRect(Texture0, vec2(p.x+dist/2.0, p.y+dist/2.0));
    col += texture2DRect(Texture0, vec2(p.x-dist/2.0, p.y+dist/2.0));
    
    //col = col * 0.25 - 0.1;
    
    gl_FragColor = col * 0.25;
}
