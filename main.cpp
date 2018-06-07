#include "assert.h"

#include <stdio.h>
#include <unistd.h>

#include <atomic>
#include <ctime>
#include <thread>

//#include "../glfw/include/GLFW/glfw3.h"
#include <GLFW/glfw3.h>

/*
In this test, a producer thread posts empty events, and a consumer thread consumes them.

- The producer thread waits for the consumer to consume the event before posting a new event.

- The consumer thread waits for the producer to declare that an event /will/ be produced before
   trying to consume it.
*/

std::atomic_int event_status;
constexpr int CONSUMED = 0;
constexpr int POSTED = 1;

void consume() {
    while(true) {
        while(event_status != POSTED) {
            std::this_thread::yield();
        }

        glfwWaitEvents(); printf("c");

        event_status = CONSUMED;
    }
}

void produce() {
    while(true) {
        using namespace std;
        clock_t begin = clock();
        while(event_status != CONSUMED) {
            this_thread::yield();
            clock_t end = clock();
            double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
            if(elapsed_secs > 3) {
                printf(
                    "\nThe bug has been reproduced:"
                    "\nThe main thread is blocked since 3 seconds in glfwWaitEvents(), "
                    "eventhough an empty event has been posted to unblock it.\n");
                exit(1);
            }
        }

        event_status = POSTED;

        glfwPostEmptyEvent(); printf(".");

        // Note that if we move 'event_status = POSTED;' here
        // (thereby ensuring that glfwPostEmptyEvent() and glfwWaitEvents()
        // do not execute at the same time), the bug is not reproduced.
        //
        // In other words, the bug seems to exist only when glfwPostEmptyEvent()
        // and glfwWaitEvents() are executing at the same time.
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

    printf(
        "Please stay here in the terminal while the test is running, \n"
        "to ensure that no event is sent to the glfw window.\n"
        "\nPress a key to start the test...");
    getchar();
    

    {
        event_status = CONSUMED;
        std::thread producer(produce);
        consume();
    }

    glfwDestroyWindow(window);
    return 0;
}

// build on OSX :
// g++ main.cpp -std=c++14 -I"/usr/local/Cellar/glfw/3.2.1/include/GLFW/" -lglfw3

// build on linux:
// g++ main.cpp -std=c++14 -lglfw3 -lX11 -ldl -lpthread

// build on linux with modified glfw:
// (from folder "/home/olivier/dev/glfw_build/"): make && mv ./src/libglfw3.a ./src/libglfw3My.a
// g++ main.cpp -std=c++14 -L"/home/olivier/dev/glfw_build/src" -lglfw3My -lX11 -ldl -lpthread

