#version 330 core

flat in vec3 fColor;

out vec4 color;

void main(void) {
    color = vec4(fColor, 1.0);
	//color = vec4(fTex.r, fTex.g, 0.0, 1.0);
	//color = texture(tex, fTex);
	
    //color[0] = 1.0;
    //color[1] = gl_FragCoord.y/600.0;
    //color[2] = gl_FragCoord.x/800.0;
}