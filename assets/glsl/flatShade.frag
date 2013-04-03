#version 330 core

in VertexData {
    vec4 coord;
    vec4 normal;
    vec2 texCoord;
    vec4 color;
} fragIn;

out vec4 color;

void main(void) {
    color = fragIn.color;
}