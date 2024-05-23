#include "stdafx.h"
#include "rendering.h"
#include "strip.h"

#include <glm/glm.hpp>

const float rotationSpeed{ 45.0f };
float rotationAngle{ 0.0f };
bool perspectiveDivide{};
Strip strip;

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
	if (!strip.initialize()) {
		return false;
	}
	return true;
}

void preDraw() {
	rotationAngle += deltaTime * rotationSpeed;
	glViewport(0, 0, gScreenWidth, gScreenHeight);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void draw() {
	// Draw Incorrect Perspective Divide
	perspectiveDivide = false;
	strip.setPosition(glm::vec3(0.0f, 1.0f, -7.0f));
	strip.draw();
	
	// Draw Correct Perspective Divide
	perspectiveDivide = true;
	strip.setPosition(glm::vec3(0.0f, -1.0f, -7.0f));
	strip.draw();
}

void cleanUpRenderer() {
	strip.~Strip();
}

