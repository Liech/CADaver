#include <iostream>

#include "Library/Util/LoadFileDialog.h"
#include "Library/Util/SaveFileDialog.h"
#include "Library/Triangle/Triangulation.h"

int main()
{
      Library::LoadFileDialog dlg;
    dlg.addFilter("all files", { "*" });

    dlg.execute();
    if (!dlg.isCancled())
    {
        std::cout << "file: " << dlg.getResultPath() << std::endl;
    }
    std::cout << dlg.getResultPath();
}