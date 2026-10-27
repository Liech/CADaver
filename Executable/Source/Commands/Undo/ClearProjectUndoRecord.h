#pragma once

#include "Commands/Undo/UndoRecord.h"
#include "Database/Asset.h"

#include <memory>
#include <vector>

class ClearProjectUndoRecord : public UndoRecord
{
  public:
    ClearProjectUndoRecord(std::vector<std::shared_ptr<Asset>> assets, int selectedIndex);
    virtual ~ClearProjectUndoRecord();

    void undo(Database& database) override;
    void redo(Database& database) override;

    std::string getDescription() const override;
    size_t      getSizeInBytes() const override;

    const std::vector<std::shared_ptr<Asset>>& getAssets() const;
    int                                        getSelectedIndex() const;

  private:
    std::vector<std::shared_ptr<Asset>> assets;
    int                                 selectedIndex = -1;
};
