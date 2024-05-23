#version 450

uniform sampler2D text2D;

in vec2 uv;
in float w;

void main() {
    gl_FragColor = texture(text2D, uv);
}