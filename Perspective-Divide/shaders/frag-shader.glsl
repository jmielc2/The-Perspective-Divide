#version 450

in vec2 uv;
in float w;

void main() {
    gl_FragColor = vec4(uv.x - int(uv.x), uv.y, 0.0, 1.0);
}