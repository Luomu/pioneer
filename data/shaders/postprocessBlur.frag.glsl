#extension GL_ARB_texture_rectangle : enable
uniform sampler2DRect Texture0;
uniform int iteration;
uniform float pixelWidth;
uniform float pixelHeight;

void main()
{
    vec2 Texcoord = gl_FragCoord.xy;
    vec2 texCoordSample = vec2(0.0);
    vec2 halfPixelSize = vec2(pixelWidth, pixelHeight) / 2.0;
    vec2 dUV = (vec2(pixelWidth, pixelHeight) * float(iteration)) + halfPixelSize;
    
    dUV = vec2(0.5);
    
    vec4 cOut = vec4(0.0);

    ///Sample top left pixel
    texCoordSample.x = Texcoord.x - dUV.x;
    texCoordSample.y = Texcoord.y + dUV.y;
    cOut = texture2DRect(Texture0, texCoordSample);

    ///Sample top right pixel
    texCoordSample.x = Texcoord.x + dUV.x;
    texCoordSample.y = Texcoord.y + dUV.y;
    cOut += texture2DRect(Texture0, texCoordSample);

    ///Sample bottom right pixel
    texCoordSample.x = Texcoord.x + dUV.x;
    texCoordSample.y = Texcoord.y - dUV.y;
    cOut += texture2DRect(Texture0, texCoordSample);

    ///Sample bottom left pixel
    texCoordSample.x = Texcoord.x - dUV.x;
    texCoordSample.y = Texcoord.y - dUV.y;
    cOut += texture2DRect(Texture0, texCoordSample);

    gl_FragColor = cOut * 0.25;
}
