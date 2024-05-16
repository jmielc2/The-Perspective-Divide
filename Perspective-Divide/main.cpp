#include "stdafx.h"
#include "rendering.h"

const int gScreenHeight{ 960 };
const int gScreenWidth{ 840 };
const int fps{ 60 };
float deltaTime{ 0.0f };
SDL_Window* gWindow{ nullptr };
SDL_GLContext gContext{ nullptr };
bool gQuit{ false };
constexpr std::uint32_t refreshDelay{ 1000 / fps };

static void getGLVersionInfo() {
	SDL_Log("Vendor              %s\n", glGetString(GL_VENDOR));
	SDL_Log("Renderer            %s\n", glGetString(GL_RENDERER));
	SDL_Log("Version             %s\n", glGetString(GL_VERSION));
	SDL_Log("Shading Language    %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
}

static bool initializeProgram() {
	// Initialize and Load OpenGL
	SDL_Init(SDL_INIT_EVENTS | SDL_INIT_VIDEO);
	gWindow = SDL_CreateWindow("Perspective Divide", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, gScreenWidth, gScreenHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN);
	if (!gWindow) {
		std::cerr << "Window couldn't be created." << std::endl;
		return false;
	}
	gContext = SDL_GL_CreateContext(gWindow);
	if (!gContext) {
		std::cerr << "GL context couldn't be created." << std::endl;
		return false;
	}
	if (!gladLoadGLLoader(SDL_GL_GetProcAddress)) {
		std::cerr << "glad GL functions couldn't be loaded." << std::endl;
		return false;
	}

	// Configure GL Context Attributes
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	getGLVersionInfo();

	if (!setupRenderer()) {
		return false;
	}

	return true;
}

static bool cleanUp() {
	cleanUpRenderer();
	SDL_GL_DeleteContext(gContext);
	SDL_DestroyWindow(gWindow);
	SDL_Quit();
	return true;
}

static void mainLoop() {
	while (!gQuit) {
		const std::uint64_t startTime{ SDL_GetTicks64() };
		processInput();
		preDraw();
		draw();
		SDL_GL_SwapWindow(gWindow);
		const std::uint32_t processingTime{ std::uint32_t(SDL_GetTicks64() - startTime) };
		if (processingTime < refreshDelay) {
			SDL_Delay(refreshDelay - processingTime);
		}
		deltaTime = float(SDL_GetTicks64() - startTime) / 1000.0f;
	}
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]) {
	if (!initializeProgram()) {
		cleanUp();
		return 0;
	}
	SDL_ShowWindow(gWindow);
	mainLoop();

	cleanUp();
	return 0;
}