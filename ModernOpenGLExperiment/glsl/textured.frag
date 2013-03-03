#version 330 core

in vec2 fTex;

out vec4 color;

void main(void) {
	color = vec4(fTex.u, fTex.v, 0.0, 1.0);
	//color = texture(tex, fTex);
}