#include <signal.h>
#include <unistd.h>
#include <iostream>
using std::cout;
using std::endl;

/**
 * Ctrl + C handler.
 */
static bool is_exit = 0;
static void sig_int_handler(int s)
{
    cout << "caught signal " << s << ", exit." << endl;
    is_exit = true;
}

int main(int argc, char * argv[])
{
    struct sigaction  sigIntHandler;

    // register SIGINT handler.
    sigIntHandler.sa_handler = sig_int_handler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, 0);

    cout << "SRT live server is running..." << endl;
    while (!is_exit) {
        usleep(10 * 1000);
    }

    return 0;
}
