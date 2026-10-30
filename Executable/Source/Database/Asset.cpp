#include "Database/Asset.h"

Asset::Asset(const std::string& nameInput, const std::string& typeInput)
  : guid(CADacomb::GUID::random())
  , name(nameInput)
  , type(typeInput)
{
}

Asset::Asset(const CADacomb::GUID& guidInput, const std::string& nameInput, const std::string& typeInput)
  : guid(guidInput)
  , name(nameInput)
  , type(typeInput)
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

const std::string& Asset::getType() const
{
    return type;
}

size_t Asset::getRevision() const
{
    return revision;
}

void Asset::markModified()
{
    revision++;
}
