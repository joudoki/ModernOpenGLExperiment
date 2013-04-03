#version 330 core

uniform sampler2D diffuseSampler;

uniform Light {
    vec3 position;
    vec3 intensity;
} light;

in VertexData {
    vec4 coord;
    vec4 normal;
    vec2 texCoord;
    vec4 color;
} fragIn;

out vec4 color;

void main(void) {
    //color.rgb = vec3(0.5);
    //color.rg  = fragIn.texCoord;
    //color     = fragIn.normal;
	color = texture(diffuseSampler, fragIn.texCoord);
}