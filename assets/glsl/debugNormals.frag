#version 330 core

out vec4 color;
in float t;

void main(void) {
    color = vec4(1-t, 0.0, t, 1.0);
}