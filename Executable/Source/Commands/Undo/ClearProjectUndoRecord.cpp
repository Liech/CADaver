#include "Commands/Undo/ClearProjectUndoRecord.h"

#include "Database/Database.h"

ClearProjectUndoRecord::ClearProjectUndoRecord(std::vector<std::shared_ptr<Asset>> assetsInput, const CADacomb::GUID& selectedGuidInput)
  : assets(std::move(assetsInput))
  , selectedGuid(selectedGuidInput)
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
    project.setSelectedAssetGuid(selectedGuid);
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

const CADacomb::GUID& ClearProjectUndoRecord::getSelectedGuid() const
{
    return selectedGuid;
}
