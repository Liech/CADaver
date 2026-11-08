#pragma once

#include <string>

// Abstract base class for educational concepts explaining system models and file formats.
class Concept
{
  public:
    Concept();
    virtual ~Concept();

    virtual std::string getType() const        = 0;
    virtual std::string getDescription() const = 0;
};
