#pragma once

#include <vector>
#include <algorithm>
#include <string>

namespace Library
{
    class LoadFileDialog
    {
      public:
        LoadFileDialog();
        virtual ~LoadFileDialog();

        void addFilter(const std::string& text, const std::vector<std::string>& suffixes);
        void setDefaultPath(const std::string&);

        void execute();

        bool        isCancled() const;
        std::string getResultPath() const;
      private:
        std::vector<std::pair<std::string, std::vector<std::string>>> filter;
        std::string                                                   defaultPath = "";

        bool        cancel = false;
        std::string resultPath;
    };
}