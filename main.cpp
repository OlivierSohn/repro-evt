#include <stdio.h>
#include <unistd.h>

#include <GLFW/glfw3.h>

void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error (%d) : %s\n", error, description);
}

int main() {

    // disable stdout buffering, to see logs immediately.
    setbuf(stdout, NULL);
    
    if (!glfwInit())
    {
        return 1;
    }
    
    glfwSetErrorCallback(error_callback);
    GLFWwindow* window = glfwCreateWindow(640, 480, "My Title", NULL, NULL);
    if (!window)
    {
        return 2;
    }
    glfwMakeContextCurrent(window);
    
    while(!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        printf(".");
        usleep(10000);
    }

    glfwDestroyWindow(window);
    return 0;
}

// build on OSX :
// g++ main.cpp -std=c++14 -I"/usr/local/Cellar/glfw/3.2.1/include/GLFW/" -lglfw3
// build on linux:
// g++ main.c -std=c++14 -lglfw -lpthread
