//~ void main(void)
//~ {
	//~ gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
//~ }
varying vec4 color;

void main(void){
   gl_Position =  gl_Vertex * 2.0 - 1.0;
   gl_TexCoord[0]  = gl_Vertex;
   color = gl_Color;
}
