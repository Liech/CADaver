#include "Writer.h"


namespace MagicaVoxImporter {
  void Writer::write(std::ofstream& stream, const int& value) {
    unsigned char bytes[4];
    bytes[3] = ((value >> 24) & 0xFF);
    bytes[2] = ((value >> 16) & 0xFF);
    bytes[1] = ((value >> 8) & 0xFF);
    bytes[0] = (value & 0xFF);
    stream.write((const char*)(bytes), 4);
  }

  void Writer::write(std::vector<unsigned char>& stream, const int& value) {
    stream.push_back((value & 0xFF));
    stream.push_back(((value >> 8) & 0xFF));
    stream.push_back(((value >> 16) & 0xFF));
    stream.push_back(((value >> 24) & 0xFF));
  }

  void Writer::write(std::vector<unsigned char>& stream, const std::string& value) {
    stream.insert(stream.end(), value.begin(), value.end());
  }
}