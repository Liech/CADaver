#pragma once

#include <string>
#include <vector>

namespace Library
{
  class base64
  {
    public:
      static std::string base64_encode(const unsigned char* data, size_t len);
      static std::vector<unsigned char> base64_decode(const std::string& in);
  };
}