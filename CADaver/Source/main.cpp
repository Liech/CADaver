#include "Cadaver.h"

#include <cstdlib>

#if defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    Cadaver::run();
    return EXIT_SUCCESS;
}
#endif

int main(int argc, char* argv[])
{
    Cadaver::run();
    return EXIT_SUCCESS;
}
