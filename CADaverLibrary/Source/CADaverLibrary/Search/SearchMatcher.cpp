#include "Search/SearchMatcher.h"

#include <algorithm>
#include <cctype>
#include <sstream>

SearchMatcher::SearchMatcher() {}

SearchMatcher::~SearchMatcher() {}

bool SearchMatcher::matches(const std::string& query, const std::string& text) const
{
    return matchesAny(query, text, "");
}

bool SearchMatcher::matchesAny(const std::string& query, const std::string& title, const std::string& description) const
{
    auto queryTokens = tokenize(query);
    if (queryTokens.empty())
    {
        return false;
    }

    std::string combined    = title + " " + description;
    auto        words       = extractWords(combined);
    std::string compactText = removeDelimiters(toLower(combined));

    for (const auto& token : queryTokens)
    {
        if (!matchToken(token, words, compactText))
        {
            return false;
        }
    }

    return true;
}

int SearchMatcher::calculateScore(const std::string& query, const std::string& title, const std::string& description) const
{
    if (!matchesAny(query, title, description))
    {
        return 0;
    }

    int score = 10;

    std::string lowerQuery   = toLower(query);
    std::string lowerTitle   = toLower(title);
    std::string compactTitle = removeDelimiters(lowerTitle);
    std::string compactQuery = removeDelimiters(lowerQuery);

    if (compactTitle == compactQuery)
    {
        score += 200;
    }
    else if (compactTitle.find(compactQuery) != std::string::npos)
    {
        score += 100;
    }

    auto titleWords  = extractWords(title);
    auto queryTokens = tokenize(query);

    for (const auto& token : queryTokens)
    {
        for (const auto& word : titleWords)
        {
            if (word == token)
            {
                score += 40;
            }
            else if (word.rfind(token, 0) == 0)
            {
                score += 25;
            }
        }
    }

    return score;
}

size_t SearchMatcher::damerauLevenshtein(const std::string& first, const std::string& second) const
{
    size_t len1 = first.size();
    size_t len2 = second.size();

    if (len1 == 0)
    {
        return len2;
    }
    if (len2 == 0)
    {
        return len1;
    }

    std::vector<std::vector<size_t>> d(len1 + 1, std::vector<size_t>(len2 + 1, 0));

    for (size_t i = 0; i <= len1; i++)
    {
        d[i][0] = i;
    }
    for (size_t j = 0; j <= len2; j++)
    {
        d[0][j] = j;
    }

    for (size_t i = 1; i <= len1; i++)
    {
        for (size_t j = 1; j <= len2; j++)
        {
            size_t cost = (first[i - 1] == second[j - 1]) ? 0 : 1;

            size_t del = d[i - 1][j] + 1;
            size_t ins = d[i][j - 1] + 1;
            size_t sub = d[i - 1][j - 1] + cost;

            d[i][j] = std::min({ del, ins, sub });

            if (i > 1 && j > 1 && first[i - 1] == second[j - 2] && first[i - 2] == second[j - 1])
            {
                d[i][j] = std::min(d[i][j], d[i - 2][j - 2] + cost);
            }
        }
    }

    return d[len1][len2];
}

std::string SearchMatcher::replaceDelimiters(const std::string& text) const
{
    std::string result = text;
    for (char& c : result)
    {
        if (c == '_' || c == '-' || c == '/' || c == '\\' || c == '.' || c == ',')
        {
            c = ' ';
        }
    }
    return result;
}

std::string SearchMatcher::removeDelimiters(const std::string& text) const
{
    std::string result;
    result.reserve(text.size());
    for (char c : text)
    {
        if (c != '_' && c != '-' && c != '/' && c != '\\' && c != '.' && c != ',' && c != ' ' && c != '\t' && c != '\r' && c != '\n')
        {
            result.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(c))));
        }
    }
    return result;
}

std::string SearchMatcher::toLower(const std::string& input) const
{
    std::string result = input;
    std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return result;
}

std::vector<std::string> SearchMatcher::tokenize(const std::string& text) const
{
    std::string              spaced = replaceDelimiters(toLower(text));
    std::istringstream       stream(spaced);
    std::vector<std::string> tokens;
    std::string              token;

    while (stream >> token)
    {
        tokens.push_back(token);
    }
    return tokens;
}

std::vector<std::string> SearchMatcher::extractWords(const std::string& text) const
{
    std::vector<std::string> words;
    std::string              currentWord;

    for (size_t i = 0; i < text.size(); i++)
    {
        char c = text[i];
        if (c == '_' || c == '-' || c == '/' || c == '\\' || c == '.' || c == ',' || std::isspace(static_cast<unsigned char>(c)))
        {
            if (!currentWord.empty())
            {
                words.push_back(toLower(currentWord));
                currentWord.clear();
            }
            continue;
        }

        if (std::isupper(static_cast<unsigned char>(c)))
        {
            bool prevLower = (i > 0 && std::islower(static_cast<unsigned char>(text[i - 1])));
            bool nextLower = (i + 1 < text.size() && std::islower(static_cast<unsigned char>(text[i + 1])));
            if (prevLower || (currentWord.size() > 1 && nextLower))
            {
                if (!currentWord.empty())
                {
                    words.push_back(toLower(currentWord));
                    currentWord.clear();
                }
            }
        }
        currentWord.push_back(c);
    }

    if (!currentWord.empty())
    {
        words.push_back(toLower(currentWord));
    }

    return words;
}

bool SearchMatcher::matchToken(const std::string& token, const std::vector<std::string>& words, const std::string& compactText) const
{
    if (token.empty())
    {
        return true;
    }

    if (compactText.find(token) != std::string::npos)
    {
        return true;
    }

    for (const auto& word : words)
    {
        if (word == token || word.find(token) != std::string::npos)
        {
            return true;
        }

        if (token.size() >= 4)
        {
            size_t maxDist = (token.size() >= 7) ? 2 : 1;
            if (damerauLevenshtein(token, word) <= maxDist)
            {
                return true;
            }
        }
    }

    return false;
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("SearchMatcher/BrepMatchesBRep")
{
    SearchMatcher matcher;
    REQUIRE(matcher.matches("brep", "B-Rep CAD geometry"));
    REQUIRE(matcher.matches("b-rep", "B-Rep CAD geometry"));
    REQUIRE(matcher.matches("b_rep", "B-Rep CAD geometry"));
}

TEST_CASE("SearchMatcher/MultiTermSaveVoxel")
{
    SearchMatcher matcher;
    REQUIRE(matcher.matches("save voxel", "SaveVoxelAssetWindow"));
    REQUIRE(matcher.matches("save voxel", "Save Voxel Asset Window"));
    REQUIRE(matcher.matches("save_voxel", "SaveVoxelAssetWindow"));
    REQUIRE(matcher.matches("voxel save", "SaveVoxelAssetWindow"));
}

TEST_CASE("SearchMatcher/DamerauLevenshteinTypos")
{
    SearchMatcher matcher;
    REQUIRE(matcher.matches("voxl", "VoxelAsset"));
    REQUIRE(matcher.matches("cadavr", "Cadaver"));
}
#endif
