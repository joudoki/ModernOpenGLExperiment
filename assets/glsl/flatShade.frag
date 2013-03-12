#version 330 core

flat in vec3 fColor;
out vec4 color;

void main(void) {
    color = vec4(fColor, 1.0);
    
    //color = vec4(
    //    1.0,
    //    gl_FragCoord.y / 600.0,
    //    gl_FragCoord.x / 800.0,
    //    1.0
    //);
}