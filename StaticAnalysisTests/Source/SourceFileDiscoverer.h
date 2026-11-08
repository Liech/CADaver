#pragma once

#include <string>
#include <vector>

// Discovers source files across a directory hierarchy according to file extension filters.
class SourceFileDiscoverer
{
  public:
    SourceFileDiscoverer();
    virtual ~SourceFileDiscoverer();

    std::vector<std::string> discoverFiles(const std::string& directoryPath, const std::vector<std::string>& extensions) const;

  private:
    bool shouldIgnorePath(const std::string& path) const;
};
