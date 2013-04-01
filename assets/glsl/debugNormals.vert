#version 330 core

in vec3 vCoord;
in vec3 vNormal;

out vec4 gNormal;

void main(void) {
    // Perform the transformations in the geometry shader
    gl_Position.xyz = vCoord;
    gNormal.xyz     = vNormal;
    gNormal.w       = 0.0;
}