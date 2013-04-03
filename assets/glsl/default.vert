#version 330 core

//uniform Transform {
uniform mat4 modelTransform;
uniform mat3 normalTransform;
//};// transform;

in VertexIn {
    vec4 coord;
    vec3 normal;
    vec2 texCoord;
    vec4 color;
};

out VertexData {
    vec4 coord;
    vec4 normal;
    vec2 texCoord;
    vec4 color;
} vertexOut;

void main(void) {
    gl_Position         = modelTransform * coord;
    
    vertexOut.coord     = gl_Position;
    vertexOut.normal    = vec4(normalize(normalTransform * normal), 0.0);
    vertexOut.texCoord  = texCoord;
    vertexOut.color     = color;
}