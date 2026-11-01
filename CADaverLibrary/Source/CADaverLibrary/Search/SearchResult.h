#pragma once

#include <functional>
#include <string>

// Holds data and activation callback for an individual search result item.
class SearchResult
{
  public:
    SearchResult();
    SearchResult(const std::string& title, const std::string& category, const std::string& description, std::function<void()> action);
    virtual ~SearchResult();

    const std::string& getTitle() const;
    void               setTitle(const std::string& title);

    const std::string& getCategory() const;
    void               setCategory(const std::string& category);

    const std::string& getDescription() const;
    void               setDescription(const std::string& description);

    const std::function<void()>& getAction() const;
    void                         setAction(std::function<void()> action);

    void execute() const;

  private:
    std::string           title;
    std::string           category;
    std::string           description;
    std::function<void()> action;
};
