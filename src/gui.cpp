#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/gl.h>
#include <array>

#include "datatypes.h"

// layer [0, 1, 2, ...], 0 is the furthest layer, 1 is the next furthest, etc.
void drawSquare(Vector2f pos, int layer, float width, Color color)
{
        glPushMatrix();
        glTranslatef(pos.x - width / 2, pos.y - width / 2, 0.0f);
        glBegin(GL_QUADS);
        glColor3f(color.r, color.g, color.b);
        glVertex3f(-width / 2, -width / 2, layer);
        glVertex3f(width / 2, -width / 2, layer);
        glVertex3f(width / 2, width / 2, layer);
        glVertex3f(-width / 2, width / 2, layer);
        glEnd();
        glPopMatrix();
}

void drawSquare(const Square& square, int layer)
{
        drawSquare(square.pos, layer, square.size, square.color);
}

void reshapeWindow(GLFWwindow* window)
{
        int w, h;
        glfwGetFramebufferSize(window, &w, &h);

        double aspectRatio = (double)w / h;
        glViewport(0, 0, w, h);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        if (w <= h)
                glOrtho(-1.0, 1.0, -1.0 / aspectRatio, 1.0 / aspectRatio, 1.0, -1.0);
        else
                glOrtho(-1.0 * aspectRatio, 1.0 * aspectRatio, -1.0, 1.0, 1.0, -1.0);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
}

GLFWwindow* init(const char* title, int width, int height)
{
        if (!glfwInit())
                return NULL;

        GLFWwindow* window = glfwCreateWindow(width, height, title, NULL, NULL);
        if (!window)
        {
                glfwTerminate();
                return NULL;
        }

        glfwMakeContextCurrent(window);

        glewInit();

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);

        reshapeWindow(window);

        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

        return window;
}
