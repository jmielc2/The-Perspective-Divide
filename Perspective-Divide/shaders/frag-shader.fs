#version 450

in vec2 uv;

out vec4 fragColor;

void main() {
    fragColor = vec4(1.0f, uv.x, uv.y, 1.0f);
}