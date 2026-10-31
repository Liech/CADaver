#include "Database/Asset.h"

Asset::Asset()
  : guid(CADacomb::GUID::random())
{
}

Asset::~Asset() {}

const CADacomb::GUID& Asset::getGuid() const
{
    return guid;
}

const std::string& Asset::getName() const
{
    return name;
}

void Asset::setName(const std::string& newName)
{
    name = newName;
    markModified();
}

size_t Asset::getRevision() const
{
    return revision;
}

void Asset::markModified()
{
    revision++;
}
