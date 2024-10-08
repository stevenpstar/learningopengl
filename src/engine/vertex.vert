#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColour;
layout (location = 2) in vec2 aTexCoord;
out vec3 ourColour;
out vec2 TexCoord;
uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
void main() {
 #gl_Position = proj * view * model * vec4(aPos, 1.0);
 gl_Position = vec4(aPos, 1.0);
 ourColour = aColour;
 TexCoord = aTexCoord;
}

