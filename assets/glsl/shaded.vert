#version 330 core

uniform mat4  modelTransform;
uniform mat3 normalTransform;

in vec3 coord;
in vec3 norm;
in vec2 tex;

out vec3 fNorm;
out vec2 fTex;

void main(void) {
    gl_Position = modelTransform * vec4(coord, 1.0);
    fNorm = normalTransform * norm;
    fTex = tex;
}