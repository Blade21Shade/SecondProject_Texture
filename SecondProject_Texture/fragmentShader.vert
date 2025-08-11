#version 460 core
out vec4 fragColor;
in vec3 ourColor;
in vec2 texCoord;

uniform sampler2D ourTexture;
uniform sampler2D ourTexture2;
uniform float mixVal;

void main() {
    // fragColor = vec4(ourColor, 1.0);
    // fragColor = texture(ourTexture, texCoord) * vec4(ourColor, 1.0);
    fragColor = mix(texture(ourTexture, texCoord), texture(ourTexture2, texCoord), mixVal);
}