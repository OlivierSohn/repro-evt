#include "assert.h"

#include <stdio.h>
#include <unistd.h>

#include <atomic>
#include <ctime>
#include <thread>

#include <GLFW/glfw3.h>

/*
This program is used to measure the cpu consumption when waiting for an event with
glfwWaitEvents, so as to ensure that the fix proposed for https://github.com/glfw/glfw/issues/1281
won't make a regression on that side. */

uint64_t nIterations = 0;

void consume() {
    while(true) {
        glfwWaitEvents(); printf("c");
    }
}


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

    printf("glfwGetVersionString = %s\n\n", glfwGetVersionString());

    glfwSetErrorCallback(error_callback);
    GLFWwindow* window = glfwCreateWindow(640, 480, "My Title", NULL, NULL);
    if (!window)
    {
        return 2;
    }
    glfwMakeContextCurrent(window);
    
    // Window creation has generated some events in the queue so we clear the queue:
    glfwPollEvents();
    for(int i=0; i<10; ++i) {
        glfwWaitEventsTimeout(0.1);
        glfwPollEvents();
    }

    {
        consume();
    }

    glfwDestroyWindow(window);
    return 0;
}

// build on OSX :
// g++ justwait.cpp -std=c++14 -I"/usr/local/Cellar/glfw/3.2.1/include/GLFW/" -lglfw3

// build on linux:
// g++ justwait.cpp -std=c++14 -lglfw -lX11 -ldl -lpthread

// build on linux with modified glfw:
// (from folder "/home/olivier/dev/glfw_build/"): make && mv ./src/libglfw3.a ./src/libglfw3My.a
// g++ justwait.cpp -std=c++14 -L"/home/olivier/dev/glfw_build/src" -lglfw3My -lX11 -ldl -lpthread

