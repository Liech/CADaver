#pragma once

#include <cstddef>
#include <string>
#include <vector>

// Evaluates query matching using delimiter space-mapping, CamelCase splitting, and Damerau-Levenshtein distance.
class SearchMatcher
{
  public:
    SearchMatcher();
    virtual ~SearchMatcher();

    bool   matches(const std::string& query, const std::string& text) const;
    bool   matchesAny(const std::string& query, const std::string& title, const std::string& description) const;
    int    calculateScore(const std::string& query, const std::string& title, const std::string& description) const;
    size_t damerauLevenshtein(const std::string& first, const std::string& second) const;

  private:
    std::string              replaceDelimiters(const std::string& text) const;
    std::string              removeDelimiters(const std::string& text) const;
    std::string              toLower(const std::string& input) const;
    std::vector<std::string> tokenize(const std::string& text) const;
    std::vector<std::string> extractWords(const std::string& text) const;
    bool                     matchToken(const std::string& token, const std::vector<std::string>& words, const std::string& compactText) const;
};
