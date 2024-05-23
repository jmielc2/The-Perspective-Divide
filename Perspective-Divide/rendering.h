#pragma once

#ifndef RENDERING_H
#define RENDERING_H

extern const int gScreenHeight;
extern const int gScreenWidth;
extern float deltaTime;

extern bool setupRenderer();

extern void processInput();

extern void preDraw();

extern void draw();

extern void cleanUpRenderer();

#endif