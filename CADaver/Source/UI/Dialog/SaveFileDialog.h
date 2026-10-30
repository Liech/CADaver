#pragma once

#include <string>
#include <vector>

namespace Library
{
    class SaveFileDialog
    {
      public:
        SaveFileDialog();
        virtual ~SaveFileDialog();

        void addFilter(const std::string& text, const std::vector<std::string>& suffixes);
        void setSaveFileName(const std::string& text);
        void setDefaultPath(const std::string& path);

        void execute();

        bool        isCancled() const;
        std::string getResultPath() const;

      private:
        std::vector<std::pair<std::string, std::vector<std::string>>> filter;
        std::string                                                   name        = "Save";
        std::string                                                   defaultPath = "";
        std::string                                                   resultPath;
        bool                                                          cancel = false;
    };
}
