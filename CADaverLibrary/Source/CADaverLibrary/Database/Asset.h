#pragma once

#include "CADacomb/Util/GUID.h"

#include <string>

// Headless base class for project data assets. Must remain decoupled from rendering and graphics APIs.
class Asset
{
  public:
    Asset();
    virtual ~Asset();

    const CADacomb::GUID& getGuid() const;

    const std::string& getName() const;
    void               setName(const std::string& name);

    virtual std::string getType() const        = 0;
    virtual size_t      getSizeInBytes() const = 0;

    size_t getRevision() const;

  protected:
    void markModified();

  private:
    CADacomb::GUID guid;
    std::string    name;
    size_t         revision = 0;
};
