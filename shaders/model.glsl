#version 330 core
layout (location = 0) in vec3 aPos;

void main() {
    gl_Position = vec4(aPos, 1.0);
}
#SPLIT
#version 330 core
out vec4 fragColor;

void main() {
    fragColor = vec4(0.9, 0.0, 0.0, 1.0);
}
