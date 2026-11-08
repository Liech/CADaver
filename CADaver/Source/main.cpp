#include "Cadaver.h"
#include "CommandLineArgs.h"
#include "HeadlessScriptRunner.h"
#include "Windows/Console/SystemConsole.h"

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
    CommandLineArgs args = CommandLineArgs::parse(__argc, __argv);
    if (args.hasScript())
    {
        SystemConsole::attachParent();
        HeadlessScriptRunner runner;
        return runner.run(args);
    }

    Cadaver::run();
    return EXIT_SUCCESS;
}
#endif

int main(int argc, char* argv[])
{
    CommandLineArgs args = CommandLineArgs::parse(argc, argv);
    if (args.hasScript())
    {
        SystemConsole::attachParent();
        HeadlessScriptRunner runner;
        return runner.run(args);
    }

    Cadaver::run();
    return EXIT_SUCCESS;
}
