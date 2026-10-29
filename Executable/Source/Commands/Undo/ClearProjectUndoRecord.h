#pragma once

#include "CADacomb/Util/GUID.h"
#include "Commands/Undo/UndoRecord.h"
#include "Database/Asset.h"

#include <memory>
#include <vector>

class ClearProjectUndoRecord : public UndoRecord
{
  public:
    ClearProjectUndoRecord(std::vector<std::shared_ptr<Asset>> assets, const CADacomb::GUID& selectedGuid);
    virtual ~ClearProjectUndoRecord();

    void undo(Database& database) override;
    void redo(Database& database) override;

    std::string getDescription() const override;
    size_t      getSizeInBytes() const override;

    const std::vector<std::shared_ptr<Asset>>& getAssets() const;
    const CADacomb::GUID&                      getSelectedGuid() const;

  private:
    std::vector<std::shared_ptr<Asset>> assets;
    CADacomb::GUID                      selectedGuid;
};
