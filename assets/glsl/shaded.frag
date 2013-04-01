#version 330 core

uniform sampler2D diffuseSampler;
uniform vec3      lightPosition;
uniform vec3      lightIntensity;

in vec2 fTex;
in vec3 fNorm;

out vec4 color;

void main(void) {
    //color = vec4(vec3(0.5),1.0);
    //color = vec4(fTex.x, fTex.y, 0.0, 1.0);
    //color = vec4(fNorm, 1.0);
    
	color = texture(diffuseSampler, fTex);
}