#version 330 core
out vec4 FragColor;
in vec3 ourColour;
in vec2 TexCoord;
uniform sampler2D tex1;
uniform sampler2D tex2;
void main() {
 FragColor = texture(tex1, TexCoord);
}

