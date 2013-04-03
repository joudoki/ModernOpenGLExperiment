#version 330 core

layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

in VertexData {
    vec4 coord;
    vec4 normal;
    vec2 texCoord;
    vec4 color;
} vertexIn[];

out VertexData {
    vec4 coord;
    vec4 normal;
    vec2 texCoord;
    vec4 color;
} vertexOut;

const float NORMAL_SCALE    = 10.0;
const vec4  NORMAL_COLOR_A  = vec4(1.0, 0.0, 0.0, 1.0);
const vec4  NORMAL_COLOR_B  = vec4(0.0, 0.0, 1.0, 1.0); 

void makeVertex(vec4 coord, vec4 color) {
    gl_Position = coord;
    vertexOut.coord = coord;
    vertexOut.color = color;
    EmitVertex();
}

void main() {
    for (int i=0; i<gl_in.length(); ++i) {
        vec4 a = gl_in[i].gl_Position;
        vec4 b = a + NORMAL_SCALE * vertexIn[i].normal;
    
        makeVertex(a, NORMAL_COLOR_A);
        makeVertex(b, NORMAL_COLOR_B);
        
        EndPrimitive();
    }
}