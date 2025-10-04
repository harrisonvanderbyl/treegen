/* register_types.cpp */

#include "register_types.h"

#include "duomesh.h"
#include "flowergen.h"
#include "treegen.h"
#include "treegen2.h"

void initialize_treegen_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}
	ClassDB::register_class<FlowerGen>();
	ClassDB::register_class<TreeGen>();
	ClassDB::register_class<TreeGen2>();
}

void uninitialize_treegen_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}
	// Nothing to do here in this example.
}