#pragma once

#include <string>

// Executes Cadaver scripts headlessly and checks process results.
class IntegrationTestRunner
{
  public:
    IntegrationTestRunner();
    virtual ~IntegrationTestRunner();

    static int runScript(const std::string& scriptRelativePath);
};
