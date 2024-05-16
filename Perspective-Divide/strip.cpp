#include "stdafx.h"
#include "strip.h"

#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Strip::Strip(GLuint& vaoData) : modelMatrix(1.0), projectionMatrix(1.0), shader(nullptr), vaoData(&vaoData), texture(nullptr) {
	return;
}

void Strip::updateUniforms() {
	if (!shaderCheck()) {
		return;
	}
	glUseProgram(*shader.get());
	GLint modelMatrixLocation{ glGetUniformLocation(*shader.get(), "uModel") };
	modelMatrix = glm::rotate(modelMatrix, glm::radians(rotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(modelMatrixLocation, 1, false, glm::value_ptr(modelMatrix));
	glUseProgram(0);
}

bool Strip::shaderCheck() {
	if (!shader.get()) {
		std::cout << "No shader program has been set for strip " << std::hex << this << '\n';
		return false;
	}
	return true;
}

void Strip::setPosition(const glm::vec3& pos) {
	if (!shaderCheck()) {
		return;
	}
	glUseProgram(*shader.get());
	GLint modelMatrixLocation{ glGetUniformLocation(*shader.get(), "uModel") };
	modelMatrix = glm::translate(glm::mat4(1.0), pos);
	modelMatrix = glm::rotate(modelMatrix, glm::radians(rotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(modelMatrixLocation, 1, false, glm::value_ptr(modelMatrix));
	glUseProgram(0);
}

void Strip::setTexture(GLuint& newTexture) {
	texture = std::make_shared<GLuint>(newTexture);
}

void Strip::draw() {
	if (!shaderCheck()) {
		return;
	}
	glUseProgram(*shader.get());
	glBindVertexArray(*vaoData.get());
	glBindTexture(GL_TEXTURE_2D, *texture.get());
	updateUniforms();
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}

void Strip::setShader(GLuint& newShader) {
	shader = std::make_shared<GLuint>(newShader);
}



