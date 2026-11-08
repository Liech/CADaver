#pragma once

#include "Polyglot/Node.h"

#include <cstdint>
#include <string>
#include <vector>

// Utility for grabbing and delivering binary or file-based asset data.
class IOValueGrabber
{
  public:
    IOValueGrabber();
    virtual ~IOValueGrabber();

    static bool isFilePath(const Polyglot::Node& node);
    static bool isFunction(const Polyglot::Node& node);
    static bool isBinary(const Polyglot::Node& node);

    static std::vector<uint8_t> grabBytes(const Polyglot::Node& source);
    static std::string          grabFilePath(const Polyglot::Node& node);

    static bool deliverBytes(const Polyglot::Node& destination, const std::vector<uint8_t>& bytes);
};
