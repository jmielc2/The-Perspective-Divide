#version 460

in vec2 uv;

void main() {
    gl_FragColor = vec4(1, uv.x, uv.y, 1);
}