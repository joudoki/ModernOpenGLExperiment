#version 330 core

uniform mat4 transform;

in vec3 vCoord;
in vec3 vColor;

flat out vec3 fColor;

void main(void) {
    gl_Position = transform * vec4(vCoord, 1.0);
    fColor = vColor;
}