#include "stdafx.h"
#include "rendering.h"
#include "strip.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <SDL_image.h>

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

glm::mat4 modelMatrix1{ 1.0f };
glm::mat4 modelMatrix2{ 1.0f };
glm::mat4 projectionMatrix{ 1.0f };

const float rotationSpeed{ 45.0f };
float rotationAngle{ 70.0f };
GLuint vbo{};
GLuint vao{};
GLuint ebo{};
GLuint checkerboard{};
GLuint* curProgram{ nullptr };
GLuint shaderProgram{};
GLuint pdShaderProgram{};

void updateUniforms(glm::mat4& modelMatrix);
bool loadTextures(const std::string& filename, GLuint& texture);
void bufferData();
GLuint compileShaderProgram(const std::string& vertexShaderFile, const std::string& fragmnetShaderFile);
void configureUniforms(const glm::vec3& startPos, glm::mat4& modelMatrix);

void processInput() {
	SDL_Event e{};
	while (SDL_PollEvent(&e)) {
		if (e.type == SDL_QUIT) {
			gQuit = true;
		}
	}
	return;
}

bool setupRenderer() {
	// Enable GL features we need
	glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	// Compile Shaders
	shaderProgram = compileShaderProgram("shaders/vert-shader.glsl", "shaders/frag-shader.glsl");
	if (!shaderProgram) {
		return false;
	}
	glUseProgram(shaderProgram);
	curProgram = &shaderProgram;
	configureUniforms(glm::vec3(0.0f, 1.0f, -7.0f), modelMatrix1);

	pdShaderProgram = compileShaderProgram("shaders/vert-shader.glsl", "shaders/pd-frag-shader.glsl");
	if (!pdShaderProgram) {
		return false;
	}
	glUseProgram(pdShaderProgram);
	curProgram = &pdShaderProgram;
	configureUniforms(glm::vec3(0.0f, -0.5f, -7.0f), modelMatrix2);
	std::cout << "Shaders compiled successfully!\n";

	// Load Data
	bufferData();
	if (!loadTextures("assets/checkerboard.png", checkerboard)) {
		return false;
	}
	return true;
}

void preDraw() {
	
}

void draw() {
	rotationAngle = deltaTime * rotationSpeed;
	glBindVertexArray(vao);
	glBindTexture(GL_TEXTURE_2D, checkerboard);
	glViewport(0, 0, gScreenWidth, gScreenHeight);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	curProgram = &shaderProgram;
	glUseProgram(shaderProgram);
	// updateUniforms(modelMatrix1);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

	curProgram = &pdShaderProgram;
	glUseProgram(pdShaderProgram);
	// updateUniforms(modelMatrix2);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}

void cleanUpRenderer() {
	glDeleteBuffers(1, &ebo);
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
	glDeleteProgram(shaderProgram);
	glDeleteProgram(pdShaderProgram);
	glDeleteTextures(1, &checkerboard);
}

void updateUniforms(glm::mat4& modelMatrix) {
	GLint modelMatrixLocation = glGetUniformLocation(*curProgram, "uModel");
	modelMatrix = glm::rotate(modelMatrix, glm::radians(rotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(modelMatrixLocation, 1, false, glm::value_ptr(modelMatrix));
}

void bufferData() {
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
	GLint vertexAttribLoc{ glGetAttribLocation(shaderProgram, "aVertex") };
	if (vertexAttribLoc != -1) {
		glEnableVertexAttribArray((GLuint) vertexAttribLoc);
		glVertexAttribPointer((GLuint) vertexAttribLoc, 3, GL_FLOAT, true, sizeof(float) * 5, (void*)0);
	} else {
		std::cerr << "Couldn't find the aVertex attribute.\n";
	}
	GLint textureAttribLoc{ glGetAttribLocation(shaderProgram, "aTexture") };
	if (textureAttribLoc != -1) {
		glEnableVertexAttribArray((GLuint) textureAttribLoc);
		glVertexAttribPointer((GLuint) textureAttribLoc, 2, GL_FLOAT, true, sizeof(float) * 5, (GLvoid*)(3 * sizeof(float)));
	} else {
		std::cerr << "Couldn't find the aTexture attribute.\n";
	}
}

void configureUniforms(const glm::vec3& startPos, glm::mat4& modelMatrix) {
	// Configure Uniforms
	GLint modelMatrixLocation = glGetUniformLocation(*curProgram, "uModel");
	if (modelMatrixLocation != -1) {
		modelMatrix = glm::translate(glm::mat4(1.0f), startPos);
		modelMatrix = glm::rotate(modelMatrix, glm::radians(rotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(modelMatrixLocation, 1, false, glm::value_ptr(modelMatrix));
	} else {
		std::cerr << "Couldn't find the uModel uniform.\n";
	}
	GLint projectionMatrixLocation = glGetUniformLocation(*curProgram, "uProjection");
	if (projectionMatrixLocation != -1) {
		projectionMatrix = glm::perspective(glm::radians(45.0f), float(gScreenWidth) / float(gScreenHeight), 0.1f, 10.0f);
		glUniformMatrix4fv(projectionMatrixLocation, 1, false, glm::value_ptr(projectionMatrix));
	} else {
		std::cerr << "Couldn't find the uProjection uniform.\n";
	}
}

bool loadTextures(const std::string& filename, GLuint& texture) {
	texture = 0;
	SDL_Surface* imgSource{ IMG_Load("assets/checkerboard.png") };
	if (!imgSource) {
		std::cerr << "Couldn't open '" << filename << "'" << std::endl;
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
	return true;
}

bool readFile(const std::string& filename, std::string& contents) {
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

GLuint compileShader(GLenum type, const std::string& src) {
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
		SDL_Log("%s Shader Compiler Error:\n%s\n", (type == GL_VERTEX_SHADER)? "Vertex" : "Fragment", buf);
		return 0;
	}
	return shader;
}

GLuint compileShaderProgram(const std::string& vertexShaderFile, const std::string& fragmentShaderFile) {
	// Compile Shaders
	std::string vertShaderSrc{ };
	if (!readFile(vertexShaderFile, vertShaderSrc)) {
		return 0;
	}
	GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertShaderSrc);
	std::string fragShaderSrc;
	if (!vertexShader || !readFile(fragmentShaderFile, fragShaderSrc)) {
		return 0;
	}
	GLuint fragShader = compileShader(GL_FRAGMENT_SHADER, fragShaderSrc);
	if (!fragShader) {
		return 0;
	}

	// Link Program
	GLuint program = glCreateProgram();
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragShader);
	glLinkProgram(program);
	glDeleteShader(vertexShader);
	glDeleteShader(fragShader);

	// Check Program Status
	GLint programLinkStatus{};
	glGetProgramiv(program, GL_LINK_STATUS, &programLinkStatus);
	if (!programLinkStatus) {
		char buf[256];
		glGetProgramInfoLog(program, sizeof(buf), nullptr, buf);
		SDL_Log("Shader Program Linker Error: %s\n", buf);
		return 0;
	}
	return program;
}