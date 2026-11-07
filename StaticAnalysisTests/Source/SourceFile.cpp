#include "SourceFile.h"

#include <fstream>
#include <sstream>

SourceFile::SourceFile() {}

SourceFile::SourceFile(const std::string& filePathInput, const std::string& contentInput)
{
    filePath = filePathInput;
    content  = contentInput;

    std::stringstream stream(content);
    std::string       line;
    while (std::getline(stream, line))
    {
        if (!line.empty() && line.back() == '\r')
        {
            line.pop_back();
        }
        lines.push_back(line);
    }
}

SourceFile::~SourceFile() {}

const std::string& SourceFile::getFilePath() const
{
    return filePath;
}

const std::string& SourceFile::getContent() const
{
    return content;
}

const std::vector<std::string>& SourceFile::getLines() const
{
    return lines;
}

SourceFile SourceFile::loadFromFile(const std::string& absolutePath)
{
    std::ifstream stream(absolutePath);
    if (!stream.is_open())
    {
        return SourceFile(absolutePath, "");
    }

    std::stringstream buffer;
    buffer << stream.rdbuf();
    return SourceFile(absolutePath, buffer.str());
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("SourceFile/ConstructorAndSplitting")
{
    std::string text = "line1\nline2\r\nline3";
    SourceFile  file("test.cpp", text);

    CHECK(file.getFilePath() == "test.cpp");
    CHECK(file.getContent() == text);
    REQUIRE(file.getLines().size() == 3);
    CHECK(file.getLines()[0] == "line1");
    CHECK(file.getLines()[1] == "line2");
    CHECK(file.getLines()[2] == "line3");
}
#endif
