#include "Search/SearchResult.h"

SearchResult::SearchResult() {}

SearchResult::SearchResult(const std::string& title, const std::string& category, const std::string& description, std::function<void()> action)
  : title(title)
  , category(category)
  , description(description)
  , action(std::move(action))
{
}

SearchResult::~SearchResult() {}

const std::string& SearchResult::getTitle() const
{
    return title;
}

void SearchResult::setTitle(const std::string& newTitle)
{
    title = newTitle;
}

const std::string& SearchResult::getCategory() const
{
    return category;
}

void SearchResult::setCategory(const std::string& newCategory)
{
    category = newCategory;
}

const std::string& SearchResult::getDescription() const
{
    return description;
}

void SearchResult::setDescription(const std::string& newDescription)
{
    description = newDescription;
}

const std::function<void()>& SearchResult::getAction() const
{
    return action;
}

void SearchResult::setAction(std::function<void()> newAction)
{
    action = std::move(newAction);
}

void SearchResult::execute() const
{
    if (action)
    {
        action();
    }
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("SearchResult/ConstructionAndExecution")
{
    bool         executed = false;
    SearchResult result("MyTitle", "TestCategory", "Some Description", [&executed]() { executed = true; });

    REQUIRE(result.getTitle() == "MyTitle");
    REQUIRE(result.getCategory() == "TestCategory");
    REQUIRE(result.getDescription() == "Some Description");

    result.execute();
    REQUIRE(executed);
}
#endif
