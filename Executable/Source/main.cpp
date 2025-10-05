#include <iostream>

#include "Library/Util/LoadFileDialog.h"
#include "Library/Util/SaveFileDialog.h"
#include "Library/Triangle/AssimpAsset.h"
#include "Library/Triangle/Triangulation.h"

int main()
{
    Library::LoadFileDialog dlg;
    dlg.addFilter("assimp asset files",Library::AssimpAsset::getSupportedFormats());
    dlg.addFilter("all files", { "*" });

    dlg.execute();
    if (!dlg.isCancled())
    {
        std::cout << "file: " << dlg.getResultPath() << std::endl;
        auto asset = Library::AssimpAsset::load(dlg.getResultPath());
        //Library::SaveFileDialog sdlg;
        //sdlg.addFilter("assimp asset files", Library::AssimpAsset::getSupportedFormats());
        //sdlg.execute();
        //if (!sdlg.isCancled())
        //{
            //asset->save(sdlg.getResultPath());
        asset->toTriangulation()->saveAsSTL(dlg.getResultPath() + ".stl");
        //}
    }
    std::cout << dlg.getResultPath();
}