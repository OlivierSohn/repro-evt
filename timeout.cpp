#include "assert.h"

#include <stdio.h>
#include <unistd.h>

#include <atomic>
#include <chrono>
#include <thread>

#include <GLFW/glfw3.h>

/*
This program is used to measure the accuracy / granularity of glfwWaitEventsTimeout
so as to ensure that the fix proposed for https://github.com/glfw/glfw/issues/1281
won't make a regression on that side. */

uint64_t nIterations = 0;

void consume() {
    while(true) {
        using namespace std;
        chrono::steady_clock::time_point begin = chrono::steady_clock::now();

        glfwWaitEventsTimeout(0.001);
        // 80 micro seconds is the smallest "non almost 0" interval we can do with select (use 0.000001).
        // then, with 0.0000005, we have 5 microseconds (probably interpreted as no wait)
        
        // 1100 micro seconds is the smallest "non almost 0" interval we can do with poll (use 0.001)
        // then, with 0.0005, we have 5 microseconds (probably interpreted as no wait)

        chrono::steady_clock::time_point end = chrono::steady_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::microseconds>(end-begin).count();
        printf("%d\n", (int)ms);
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
// g++ timeout.cpp -std=c++14 -I"/usr/local/Cellar/glfw/3.2.1/include/GLFW/" -lglfw3

// build on linux:
// g++ timeout.cpp -std=c++14 -lglfw -lX11 -ldl -lpthread

// build on linux with modified glfw:
// (from folder "/home/olivier/dev/glfw_build/"): make && mv ./src/libglfw3.a ./src/libglfw3My.a
// g++ timeout.cpp -std=c++14 -L"/home/olivier/dev/glfw_build/src" -lglfw3My -lX11 -ldl -lpthread

