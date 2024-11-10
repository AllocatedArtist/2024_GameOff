#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 fragTexCoords;

void main() {
    fragTexCoords = aTexCoords;
    gl_Position = vec4(aPos, 1.0);
}
#SPLIT
#version 330 core
out vec4 fragColor;

in vec2 fragTexCoords;

uniform sampler2D texture0;

void main() {
    vec4 texture_color = texture(texture0, fragTexCoords);
    fragColor = texture_color;
}
