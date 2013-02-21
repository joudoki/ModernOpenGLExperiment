#version 330 core

flat in vec3 fColor;

out vec3 color;

void main(void) {
    color = fColor;

    //color[2] = gl_FragCoord.x/800.0;
    //color[1] = gl_FragCoord.y/600.0;
    //color[0] = 1.0;
}