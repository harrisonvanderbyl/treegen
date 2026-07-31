#include "register_types.h"

#include <gdextension_interface.h>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>

#include "flower_generator.h"
#include "tree_generator.h"
#include "dual_mesh.h"

using namespace godot;

void initialize_treegen_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}

	ClassDB::register_class<FlowerGenerator>();
	ClassDB::register_abstract_class<DualMesh>();
	ClassDB::register_class<TreeGenerator>();
}

void uninitialize_treegen_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}
}

extern "C" {

// Initialization.
GDExtensionBool GDE_EXPORT treegen_library_init(GDExtensionInterfaceGetProcAddress p_get_proc_address, GDExtensionClassLibraryPtr p_library, GDExtensionInitialization *r_initialization) {
	godot::GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);

	init_obj.register_initializer(initialize_treegen_module);
	init_obj.register_terminator(uninitialize_treegen_module);
	init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);

	return init_obj.init();
}

} // extern "C"
