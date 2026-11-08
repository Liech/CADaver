#include "SourceFileDiscoverer.h"

#include <algorithm>
#include <filesystem>

SourceFileDiscoverer::SourceFileDiscoverer() {}

SourceFileDiscoverer::~SourceFileDiscoverer() {}

std::vector<std::string> SourceFileDiscoverer::discoverFiles(const std::string& directoryPath, const std::vector<std::string>& extensions) const
{
    std::vector<std::string> discovered;
    if (!std::filesystem::exists(directoryPath))
    {
        return discovered;
    }

    for (const auto& entry : std::filesystem::recursive_directory_iterator(directoryPath))
    {
        if (!entry.is_regular_file())
        {
            continue;
        }

        std::string pathStr = entry.path().lexically_normal().string();
        std::replace(pathStr.begin(), pathStr.end(), '\\', '/');

        if (shouldIgnorePath(pathStr))
        {
            continue;
        }

        std::string ext = entry.path().extension().string();
        for (const auto& allowedExt : extensions)
        {
            if (ext == allowedExt)
            {
                discovered.push_back(pathStr);
                break;
            }
        }
    }
    return discovered;
}

bool SourceFileDiscoverer::shouldIgnorePath(const std::string& path) const
{
    static const std::vector<std::string> ignoredSubstrings = { "/.git/", "/build/", "/out/", "/bin/", "/.vs/", "/vcpkg_installed/", "/StaticAnalysisTests/", "/ThirdParty/", "/third_party/" };

    for (const auto& ignored : ignoredSubstrings)
    {
        if (path.find(ignored) != std::string::npos)
        {
            return true;
        }
    }
    return false;
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("SourceFileDiscoverer/DiscoverExtensions")
{
    SourceFileDiscoverer discoverer;
    auto                 files = discoverer.discoverFiles(".", { ".nonexistent_extension_xyz" });
    CHECK(files.empty());
}
#endif
