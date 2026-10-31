#include "Commands/Undo/ClearProjectUndoRecord.h"

#include "Database/Database.h"

ClearProjectUndoRecord::ClearProjectUndoRecord(std::vector<std::shared_ptr<Asset>> assetsInput)
  : assets(std::move(assetsInput))
{
}

ClearProjectUndoRecord::~ClearProjectUndoRecord() {}

void ClearProjectUndoRecord::undo(Database& database)
{
    auto& project = database.getProject();
    project.clear();
    for (const auto& asset : assets)
    {
        project.addAsset(asset);
    }
}

void ClearProjectUndoRecord::redo(Database& database)
{
    database.getProject().clear();
}

std::string ClearProjectUndoRecord::getDescription() const
{
    return "Clear Project (" + std::to_string(assets.size()) + " assets)";
}

size_t ClearProjectUndoRecord::getSizeInBytes() const
{
    size_t size = sizeof(ClearProjectUndoRecord);
    for (const auto& asset : assets)
    {
        if (asset)
        {
            size += asset->getSizeInBytes();
        }
    }
    return size;
}

const std::vector<std::shared_ptr<Asset>>& ClearProjectUndoRecord::getAssets() const
{
    return assets;
}
