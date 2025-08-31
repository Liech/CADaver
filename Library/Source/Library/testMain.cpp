#define CATCH_CONFIG_MAIN
#include "Library/catch.hpp"

#include <filesystem>

void enforceWorkingDir(std::string exeDir) {
  const size_t last_slash_idx = exeDir.find_last_of("\\/");
  if (std::string::npos != last_slash_idx) {
    exeDir.erase(last_slash_idx + 1);
  }
  std::filesystem::current_path(exeDir);
}

int main(int argc, char* argv[]) {
  enforceWorkingDir(std::string(argv[0]));
  int result = Catch::Session().run(argc, argv);
  return result;
}