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
        auto stl = Library::Triangulation::fromSTLFile(dlg.getResultPath());
        stl->saveAsSTL(dlg.getResultPath() + ".STL");
    }
    std::cout << dlg.getResultPath();
}