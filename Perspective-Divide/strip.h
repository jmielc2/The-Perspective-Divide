#ifndef STRIP_H
#define STRIP_H

#include <glm/glm.hpp>
#include <glad/glad.h>
#include <string>

extern float rotationAngle;
extern const float rotationSpeed;

class Strip {
private:
	glm::mat4 projectionMatrix;
	glm::vec3 pos;
	GLuint shader;
	GLuint vao;
	GLuint vbo;
	GLuint ebo;
	GLuint texture;

	void updateUniforms();
	bool compileShaderProgram();
	bool bufferData();
	bool loadTexture();
	void configureUniforms();
public:
	void draw();
	bool initialize();
	void setPosition(const glm::vec3& newPos);
	
	Strip();
	~Strip();
};

#endif
