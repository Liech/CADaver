#include "register_types.h"

#include "CADShape/CADShape.h"
#include "CADShape/CADCompound.h"
#include "CADShape/CADCompSolid.h"
#include "CADShape/CADEdge.h"
#include "CADShape/CADFace.h"
#include "CADShape/CADShell.h"
#include "CADShape/CADSolid.h"
#include "CADShape/CADVertex.h"
#include "CADShape/CADWire.h"
#include "VoxelShape.h"

#include "Util/SaveFileDialog.h"
#include "Util/LoadFileDialog.h"
#include "Util/CADOperation.h"

#include <gdextension_interface.h>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>

using namespace godot;

void initialize_module(ModuleInitializationLevel p_level)
{
    if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE)
    {
        return;
    }

    ClassDB::register_class<CADShape>();
    ClassDB::register_class<CADCompound>();
    ClassDB::register_class<CADCompSolid>();
    ClassDB::register_class<CADEdge>();
    ClassDB::register_class<CADFace>();
    ClassDB::register_class<CADShell>();
    ClassDB::register_class<CADSolid>();
    ClassDB::register_class<CADVertex>();
    ClassDB::register_class<CADWire>();
    ClassDB::register_class<SaveFileDialog>();
    ClassDB::register_class<LoadFileDialog>();
    ClassDB::register_class<CADOperation>();
    ClassDB::register_class<VoxelShape>();
}

void uninitialize_module(ModuleInitializationLevel p_level)
{
    if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE)
    {
        return;
    }
}

extern "C"
{
    // Initialization.
    GDExtensionBool GDE_EXPORT library_init(GDExtensionInterfaceGetProcAddress p_get_proc_address, const GDExtensionClassLibraryPtr p_library, GDExtensionInitialization* r_initialization)
    {
        godot::GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);

        init_obj.register_initializer(initialize_module);
        init_obj.register_terminator(uninitialize_module);
        init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);

        return init_obj.init();
    }
}