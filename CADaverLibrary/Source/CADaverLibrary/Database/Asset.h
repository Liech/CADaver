#pragma once

#include "CADacomb/Util/GUID.h"
#include "Database/AssetComponent.h"

#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

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

    CADacomb::GUID                               addComponent(std::shared_ptr<AssetComponent> component);
    bool                                         removeComponent(const CADacomb::GUID& componentGuid);
    std::shared_ptr<AssetComponent>              getComponent(const CADacomb::GUID& componentGuid) const;
    std::vector<std::shared_ptr<AssetComponent>> getAttachments() const;

    template<typename T>
    std::shared_ptr<T> addComponent();

    template<typename T>
    std::vector<std::shared_ptr<T>> getAttachments() const;

  protected:
    void markModified();

  private:
    std::map<CADacomb::GUID, std::shared_ptr<AssetComponent>> components;
    CADacomb::GUID                                            guid;
    std::string                                               name;
    size_t                                                    revision = 0;
};

template<typename T>
std::shared_ptr<T> Asset::addComponent()
{
    auto component = std::make_shared<T>();
    if (!addComponent(component).toString().empty())
    {
        return component;
    }
    return nullptr;
}

template<typename T>
std::vector<std::shared_ptr<T>> Asset::getAttachments() const
{
    std::vector<std::shared_ptr<T>> result;
    for (const auto& pair : components)
    {
        auto typed = std::dynamic_pointer_cast<T>(pair.second);
        if (typed)
        {
            result.push_back(typed);
        }
    }
    return result;
}
