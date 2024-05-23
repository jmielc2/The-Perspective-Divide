#version 450

uniform mat4 uModel;
uniform mat4 uProjection;
uniform bool uPerspectiveDivide;

in vec3 aVertex;
in vec2 aTexture;

out vec2 uv;
out float w;

void main() {
	vec4 pos = uProjection * uModel * vec4(aVertex, 1.0);
	uv = aTexture;
	if (!uPerspectiveDivide) {
		pos /= pos.w;
	}
	gl_Position = pos;
}