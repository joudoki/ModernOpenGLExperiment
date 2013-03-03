#version 330 core

uniform sampler2D tex;

in vec2 fTex;

out vec4 color;

void main(void) {
	color = vec4(fTex.x, fTex.y, 0.0, 1.0);
	//color = texture(tex, fTex);
}