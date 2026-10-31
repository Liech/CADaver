#include "Execution/UndoContainer.h"

#include "Execution/Command.h"

UndoContainer::UndoContainer(std::unique_ptr<Command> commandInput)
  : command(std::move(commandInput))
{
}

UndoContainer::~UndoContainer() {}

std::string UndoContainer::getType() const
{
    return command->getType();
}

size_t UndoContainer::getUndoSize() const
{
    return command->getUndoSize();
}

void UndoContainer::undo(Database& database)
{
    command->undo(database);
}

void UndoContainer::redo(Database& database)
{
    command->redo(database);
}
