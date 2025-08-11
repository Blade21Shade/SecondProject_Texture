#version 460 core
out vec4 fragColor;
in vec3 ourColor;
in vec2 texCoord;

uniform sampler2D ourTexture;

void main() {
    // fragColor = vec4(ourColor, 1.0);
    fragColor = texture(ourTexture, texCoord);
}