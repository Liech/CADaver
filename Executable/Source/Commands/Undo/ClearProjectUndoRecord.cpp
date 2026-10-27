#include "Commands/Undo/ClearProjectUndoRecord.h"

#include "Database/Database.h"

ClearProjectUndoRecord::ClearProjectUndoRecord(std::vector<std::shared_ptr<Asset>> assetsInput, int selectedIndexInput)
  : assets(std::move(assetsInput))
  , selectedIndex(selectedIndexInput)
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
    project.setSelectedIndex(selectedIndex);
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
    size += assets.capacity() * sizeof(std::shared_ptr<Asset>);
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

int ClearProjectUndoRecord::getSelectedIndex() const
{
    return selectedIndex;
}
