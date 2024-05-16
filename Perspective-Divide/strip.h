#ifndef STRIP_H
#define STRIP_H

#include <glm/glm.hpp>
#include <glad/glad.h>
#include <string>
#include <memory>

extern float rotationAngle;
extern const float rotationSpeed;

class Strip {
private:
	glm::mat4 modelMatrix;
	glm::mat4 projectionMatrix;
	std::shared_ptr<GLuint> shader;
	std::shared_ptr<GLuint> vaoData;
	std::shared_ptr<GLuint> texture;

	void updateUniforms();
	bool shaderCheck();
public:
	void draw();
	void setShader(GLuint& shader);
	void setTexture(GLuint& texture);
	void setPosition(const glm::vec3& pos);
	
	Strip(GLuint& vaoData);
};

#endif
