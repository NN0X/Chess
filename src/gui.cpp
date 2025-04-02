#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/gl.h>
#include <array>
#include <string>

#include "datatypes.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

unsigned char* loadIMG(const std::string& filename, int& width, int& height, int& channels)
{
        unsigned char* img = stbi_load(filename.c_str(), &width, &height, &channels, STBI_rgb_alpha);
        if (!img)
                return nullptr;
        return img;
}
// layer [0, 1, 2, ...], 0 is the furthest layer, 1 is the next furthest, etc.
void drawSquare(Vector2f pos, int layer, float width, Color color)
{
        float layerDecimated = (float)layer / 10000.0f;
        glPushMatrix();
        glBegin(GL_QUADS);
        glColor3f(color.r, color.g, color.b);
        // Top left
        glVertex3f(pos.x, pos.y, -layerDecimated);
        // Top right
        glVertex3f(pos.x + width, pos.y, -layerDecimated);
        // Bottom right
        glVertex3f(pos.x + width, pos.y - width, -layerDecimated);
        // Bottom left
        glVertex3f(pos.x, pos.y - width, -layerDecimated);
        glEnd();
        glPopMatrix();
}

void drawSquare(const Square& square, int layer)
{
        drawSquare(square.pos, layer, square.size, square.color);
}

void drawTexture(Vector2f pos, int layer, float width, const std::string& texturePath)
{
        int texWidth, texHeight, texChannels;
        unsigned char* texDat = loadIMG(texturePath, texWidth, texHeight, texChannels);
        float layerDecimated = (float)layer / 10000.0f;

        if (!texDat)
                return;

        GLuint tex;
        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texWidth, texHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, texDat);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(texDat);

        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, tex);

        glPushMatrix();
        // Translate directly to the top-left corner
        glTranslatef(pos.x, pos.y, 0.0f);

        glBegin(GL_QUADS);
        // Define vertices relative to top left.
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(0, 0, -layerDecimated);
        glTexCoord2f(1.0f, 0.0f);
        glVertex3f(width, 0, -layerDecimated);
        glTexCoord2f(1.0f, 1.0f);
        glVertex3f(width, -width, -layerDecimated);
        glTexCoord2f(0.0f, 1.0f);
        glVertex3f(0, -width, -layerDecimated);
        glEnd();

        glPopMatrix();
        glDisable(GL_TEXTURE_2D);

        glDeleteTextures(1, &tex);
}

void drawTexture(const Square& square, int layer, const std::string& texturePath)
{
        drawTexture(square.pos, layer, square.size, texturePath);
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

        glEnable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_ALPHA_TEST);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        reshapeWindow(window);

        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

        return window;
}

void convertToOpenGLCoords(double& x, double& y, GLFWwindow* window)
{
        int width, height;
        glfwGetWindowSize(window, &width, &height);
        double aspectRatio = (double)width / height;

        x = (x / width) * 2.0 - 1.0;
        y = 1.0 - (y / height) * 2.0;

        if (width <= height)
                y /= aspectRatio;
        else
                x *= aspectRatio;
}
