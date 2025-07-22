#pragma once

#include <vector>
#include <fstream>
#include <string>

namespace MagicaVoxImporter {
  class Writer {
  public:

    static void write(std::ofstream& stream, const int& value);
    static void write(std::vector<unsigned char>& stream, const int& value);
    static void write(std::vector<unsigned char>& stream, const std::string& value);
  private:

  };
}