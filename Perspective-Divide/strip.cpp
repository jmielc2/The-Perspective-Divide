#include "stdafx.h"
#include "strip.h"

#include <iostream>
#include <fstream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <SDL_image.h>

extern bool useTexture;
extern const int gScreenWidth;
extern const int gScreenHeight;
extern bool perspectiveDivide;

const float square[]{
	// vertex				// texture
	-2.5f, -0.5f, -0.0f,	0.0f, 0.0f,
	 2.5f, -0.5f, -0.0f,	5.0f, 0.0f,
	-2.5f,  0.5f, -0.0f,	0.0f, 1.0f,
	 2.5f,  0.5f, -0.0f,	5.0f, 1.0f
};
const int indices[]{
	0, 1, 3,
	0, 3, 2
};

Strip::Strip() : projectionMatrix(1.0f), pos(0.0f), shader(0), vao(0), vbo(0), ebo(0), texture(0) {
	return;
}

Strip::~Strip() {
	glDeleteProgram(shader);
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ebo);
	glDeleteTextures(1, &texture);
}

void Strip::setPosition(const glm::vec3& newPos) {
	pos = newPos;
}

void Strip::draw() {
	updateUniforms();
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}

bool Strip::initialize() {
	if (compileShaderProgram() && bufferData() && loadTexture()) {
		configureUniforms();
		return true;
	}
	return false;
}

/*
Private Methods
*/

void Strip::updateUniforms() {
	GLint modelMatrixLocation{ glGetUniformLocation(shader, "uModel") };
	glm::mat4 modelMatrix(glm::translate(glm::mat4(1.0f), pos));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(rotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(modelMatrixLocation, 1, false, glm::value_ptr(modelMatrix));
	GLint perspectiveDivideLocation{ glGetUniformLocation(shader, "uPerspectiveDivide") };
	glUniform1i(perspectiveDivideLocation, perspectiveDivide);
}

static bool readFile(const std::string& filename, std::string& contents) {
	std::ifstream file(filename, std::ios::ate);
	if (!file.is_open()) {
		std::cerr << "Couldn't open '" << filename << "'" << std::endl;
		return false;
	}
	file.seekg(0, file.beg);
	std::stringstream buffer;
	buffer << file.rdbuf();
	contents = buffer.str();
	return true;
}

static GLuint compileShader(GLenum type, const std::string& src) {
	GLuint shader = glCreateShader(type);
	const char* c_src{ src.c_str() };
	glShaderSource(shader, 1, &c_src, nullptr);
	glCompileShader(shader);
	GLint shaderCompileStatus{};
	glGetShaderiv(shader, GL_COMPILE_STATUS, &shaderCompileStatus);
	if (!shaderCompileStatus) {
		char buf[256];
		glGetShaderInfoLog(shader, sizeof(buf), nullptr, buf);
		std::cout << src << "\n";
		SDL_Log("%s Shader Compiler Error:\n%s\n", (type == GL_VERTEX_SHADER) ? "Vertex" : "Fragment", buf);
		return 0;
	}
	std::cout << "Shader compiled successfully!\n";
	return shader;
}

bool Strip::compileShaderProgram() {
	std::string vertShaderSrc{ };
	if (!readFile("shaders/vert-shader.glsl", vertShaderSrc)) {
		return false;
	}
	const std::string fragmentShaderFile = (useTexture) ? "shaders/texture-frag-shader.glsl" : "shaders/frag-shader.glsl";
	GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertShaderSrc);
	std::string fragShaderSrc;
	if (!vertexShader || !readFile(fragmentShaderFile, fragShaderSrc)) {
		return false;
	}
	GLuint fragShader = compileShader(GL_FRAGMENT_SHADER, fragShaderSrc);
	if (!fragShader) {
		return false;
	}

	// Link Program
	shader = glCreateProgram();
	glAttachShader(shader, vertexShader);
	glAttachShader(shader, fragShader);
	glLinkProgram(shader);
	glDeleteShader(vertexShader);
	glDeleteShader(fragShader);

	// Check Program Status
	GLint programLinkStatus{};
	glGetProgramiv(shader, GL_LINK_STATUS, &programLinkStatus);
	if (!programLinkStatus) {
		char buf[256];
		glGetProgramInfoLog(shader, sizeof(buf), nullptr, buf);
		SDL_Log("Shader Program Linker Error: %s\n", buf);
		return false;
	}
	glUseProgram(shader);
	std::cout << "Shader program linked successfully!\n";
	return true;
}

bool Strip::bufferData() {
	// Configure Square Data
	glCreateVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Buffer Square Data & Indices
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(square), square, GL_STATIC_DRAW);
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Configure Attributes
	GLint vertexAttribLoc{ glGetAttribLocation(shader, "aVertex") };
	if (vertexAttribLoc != -1) {
		glEnableVertexAttribArray((GLuint)vertexAttribLoc);
		glVertexAttribPointer((GLuint)vertexAttribLoc, 3, GL_FLOAT, true, sizeof(float) * 5, (void*)0);
	} else {
		std::cerr << "Couldn't find the aVertex attribute.\n";
		return false;
	}
	GLint textureAttribLoc{ glGetAttribLocation(shader, "aTexture") };
	if (textureAttribLoc != -1) {
		glEnableVertexAttribArray((GLuint)textureAttribLoc);
		glVertexAttribPointer((GLuint)textureAttribLoc, 2, GL_FLOAT, true, sizeof(float) * 5, (GLvoid*)(3 * sizeof(float)));
	} else {
		std::cerr << "Couldn't find the aTexture attribute.\n";
		return false;
	}
	std::cout << "Data buffered successfully!\n";
	return true;
}

bool Strip::loadTexture() {
	if (!useTexture) {
		std::cout << "No texture requested.\n";
		return true;
	}
	const std::string textureFile = "assets/checkerboard.png";
	SDL_Surface* imgSource{ IMG_Load(textureFile.c_str()) };
	if (!imgSource) {
		std::cerr << "Couldn't open '" << textureFile << "'" << std::endl;
		return false;
	}
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imgSource->w, imgSource->h, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, imgSource->pixels);
	glGenerateMipmap(GL_TEXTURE_2D);
	SDL_FreeSurface(imgSource);
	std::cout << "Texture loaded successfully!\n";
	return true;
}

void Strip::configureUniforms() {
	// Configure Uniforms
	GLint projectionMatrixLocation = glGetUniformLocation(shader, "uProjection");
	if (projectionMatrixLocation != -1) {
		projectionMatrix = glm::perspective(glm::radians(45.0f), float(gScreenWidth) / float(gScreenHeight), 0.1f, 10.0f);
		glUniformMatrix4fv(projectionMatrixLocation, 1, false, glm::value_ptr(projectionMatrix));
	} else {
		std::cerr << "Couldn't find the uProjection uniform.\n";
	}
}

