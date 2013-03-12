#version 330 core

uniform mat4 transform;

in vec3 coord;
in vec3 color;

flat out vec3 fColor;

void main(void) {
    gl_Position = transform * vec4(coord, 1.0);
    fColor = color;
}