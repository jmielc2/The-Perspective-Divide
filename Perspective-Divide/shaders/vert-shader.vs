#version 450

uniform mat4 uModel;
uniform mat4 uProjection;

in vec3 aVertex;
in vec2 aTexture;

out vec2 uv;

void main() {
	gl_Position = uProjection * uModel * vec4(aVertex, 1.0);
	uv = aTexture;
}