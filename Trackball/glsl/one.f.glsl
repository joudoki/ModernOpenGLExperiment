#version 330 core

flat in vec3 fColor;

out vec3 color;

void main(void) {
  color = fColor;
//"  gl_FragColor[2] = gl_FragCoord.x/800.0;
//"  gl_FragColor[1] = gl_FragCoord.y/600.0;
//"  gl_FragColor[0] = 1.0;
//"  gl_FragColor = gl_Color;
}