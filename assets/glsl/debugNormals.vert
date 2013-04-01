#version 330 core

uniform mat4 modelTransform;
uniform mat3 normalTransform;

in vec3 vCoord;
in vec3 vNormal;

out vec4 gNormal;

void main(void) {
    // Perform the transformations in the geometry shader
    gl_Position = modelTransform * vec4(vCoord, 1.0);
    gNormal     = vec4(normalTransform * vNormal, 0.0);
    //gNormal.xyz     = vNormal;
    //gNormal.w       = 0.0;
}