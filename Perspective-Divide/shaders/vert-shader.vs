#version 460

uniform mat4 uModel;
uniform mat4 uProjection;

in vec3 aVertex;
in vec2 aTexture;

out vec3 uv;

void main() {
	gl_Position = uProjection * uModel * vec4(aVertex, 1.0);
	uv = vec3(aTexture, gl_Position.w);
}