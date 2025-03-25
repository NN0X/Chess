#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/gl.h>

#include <iostream>

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

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        return window;
}

void draw()
{

}

int main(int argc, char** argv)
{
        GLFWwindow* window = init("OpenGL", 800, 600);
        if (window == NULL)
                return -1;

        while (!glfwWindowShouldClose(window))
        {
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                reshapeWindow(window);
                draw();

                glfwSwapBuffers(window);
                glfwPollEvents();
        }

        glfwTerminate();

        return 0;
}
