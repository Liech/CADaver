#include "LoadFileDialog.h"

#include <iostream>
#include <nfd.h>
#include <stdexcept>
#include <string>

namespace Library
{
    LoadFileDialog::LoadFileDialog() {}
    LoadFileDialog::~LoadFileDialog() {}

    void LoadFileDialog::addFilter(const std::string& text, const std::vector<std::string>& suffixes)
    {
        filter.push_back(std::make_pair(text, suffixes));
    }

    void LoadFileDialog::setDefaultPath(const std::string& text)
    {
        defaultPath = text;
    }

    bool LoadFileDialog::isCancled() const
    {
        return cancel;
    }

    std::string LoadFileDialog::getResultPath() const
    {
        return resultPath;
    }

    void LoadFileDialog::execute()
    {
        NFD_Init();
        nfdchar_t*                   outPath = nullptr;
        std::vector<nfdfilteritem_t> nfdFilters;
        size_t                       filterCount = filter.size();

        std::vector<std::string> suffixes;
        if (filterCount > 0)
        {
            suffixes.resize(filterCount);
            nfdFilters.resize(filterCount);
            for (size_t i = 0; i < filterCount; i++)
            {
                nfdFilters[i].name = filter[i].first.c_str();
                for (size_t j = 0; j < filter[i].second.size(); j++)
                {
                    suffixes[i] += filter[i].second[j];
                    if (j < filter[i].second.size() - 1)
                    {
                        suffixes[i] += ",";
                    }
                }
                nfdFilters[i].spec = suffixes[i].c_str();
            }
        }
        nfdresult_t result;
        if (nfdFilters.size() > 0)
            result = NFD_OpenDialog(&outPath, nfdFilters.data(), filterCount, defaultPath.c_str());
        else
            result = NFD_OpenDialog(&outPath, nullptr, filterCount, defaultPath.c_str());

        if (result == NFD_OKAY)
        {
            resultPath = outPath;
            NFD_FreePathU8(outPath);
        }
        else if (result == NFD_CANCEL)
        {
            cancel = true;
        }
        else
        {
            cancel = true;
        }
        NFD_Quit();
        std::replace(resultPath.begin(), resultPath.end(), '\\', '/');
    }
}