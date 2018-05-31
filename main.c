#include <stdio.h>
#include <GLFW/glfw3.h>

void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error (%d) : %s\n", error, description);
}

int main() {
    if (!glfwInit())
    {
        return 1;
    }
    
    glfwSetErrorCallback(error_callback);
    /*
    GLFWwindow* window = glfwCreateWindow(640, 480, "My Title", NULL, NULL);
    if (!window)
    {
        return 2;
    }
    glfwMakeContextCurrent(window);
    */
    // clear the event queue:
    glfwPollEvents();

    for(int i=0;;++i) {
        printf("%d", i);
        // put an event in the queue
        glfwPostEmptyEvent();

        printf(".");
        // consume the event
        glfwWaitEvents();
    }
    
    //glfwDestroyWindow(window);
    return 0;
}

// build on OSX :
// gcc main.c -I"/usr/local/Cellar/glfw/3.2.1/include/GLFW/" -lglfw3
