#version 330

layout (triangles) in;
layout (line_strip, max_vertices = 2) out;

uniform mat4  modelTransform;
uniform mat3 normalTransform;

in vec4 normal[];
out float t;

void main() {
    for (int i=0; i<gl_in.length(); ++i) {
        gl_Position = modelTransform * (gl_in[i].gl_Position);
        t = 0.0;
        EmitVertex();
        
        gl_Position = modelTransform * (gl_in[i].gl_Position + -1.0 * normal[i]);
        t = 1.0;
        EmitVertex();
        EndPrimitive();
        
        //gl_Position = modelTransform * gl_in[i].gl_Position + normalTransform * normal[i];
    }
}