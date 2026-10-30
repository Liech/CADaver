#pragma once

#include <string>
#include <vector>

namespace Library
{
    class LoadFileDialog
    {
      public:
        LoadFileDialog();
        virtual ~LoadFileDialog();

        void addFilter(const std::string& text, const std::vector<std::string>& suffixes);
        void setDefaultPath(const std::string& path);

        void execute();

        bool        isCancled() const;
        std::string getResultPath() const;

      private:
        std::vector<std::pair<std::string, std::vector<std::string>>> filter;
        std::string                                                   defaultPath = "";
        std::string                                                   resultPath;
        bool                                                          cancel = false;
    };
}
