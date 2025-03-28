#ifndef GUI_H
#define GUI_H

#include <GLFW/glfw3.h>
#include <string>

#include "datatypes.h"

// layer [0, 1, 2, ...], 0 is the furthest layer, 1 is the next furthest, etc.
void drawSquare(Vector2f pos, int layer, float width, Color color);
void drawSquare(const Square& square, int layer);
void drawTexture(Vector2f pos, int layer, float width, const std::string& texturePath);
void drawTexture(const Square& square, int layer, const std::string& texturePath);
void reshapeWindow(GLFWwindow* window);
GLFWwindow* init(const char* title, int width, int height);

#endif
