#include <iostream>

#include "Library/Util/LoadFileDialog.h"
#include "Library/Util/SaveFileDialog.h"
#include "Library/Triangle/AssimpAsset.h"

int main()
{
    Library::LoadFileDialog dlg;
    dlg.addFilter("assimp asset files", { "fbx",       "dae", "gltf", "glb",  "blend", "3ds", "ase", "obj", "ifc",     "xgl", "zgl", "ply", "dxf", "lwo", "lws", "lxo",     "stl",
                                          "x",         "ac",  "ms3d", "cob",  "scn",   "bvh", "csm", "xml", "irrmesh", "irr", "mdl", "md2", "md3", "pk3", "mdc", "md5mesh", "md5anim",
                                          "md5camera", "smd", "vta",  "ogex", "3d",    "b3d", "q3d", "q3s", "nff",     "off", "raw", "ter", "hmp", "ndo", "3mf", "q3o",     "q3" });
    dlg.addFilter("all files", { "*" });

    dlg.execute();
    if (!dlg.isCancled())
    {
        std::cout << "file: " << dlg.getResultPath() << std::endl;
        auto asset = Library::AssimpAsset::load(dlg.getResultPath());
        Library::SaveFileDialog sdlg;
        sdlg.addFilter("assimp asset files", { "fbx",       "dae", "gltf", "glb",  "blend", "3ds", "ase", "obj", "ifc",     "xgl", "zgl", "ply", "dxf", "lwo", "lws", "lxo",     "stl",
                                              "x",         "ac",  "ms3d", "cob",  "scn",   "bvh", "csm", "xml", "irrmesh", "irr", "mdl", "md2", "md3", "pk3", "mdc", "md5mesh", "md5anim",
                                              "md5camera", "smd", "vta",  "ogex", "3d",    "b3d", "q3d", "q3s", "nff",     "off", "raw", "ter", "hmp", "ndo", "3mf", "q3o",     "q3" });
        sdlg.execute();
        if (!sdlg.isCancled())
        {
            asset->save(sdlg.getResultPath());
        }
    }
    std::cout << dlg.getResultPath();
}