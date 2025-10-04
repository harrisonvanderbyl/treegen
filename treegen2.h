#ifndef TREE_GEN2_H
#define TREE_GEN2_H

#include "duomesh.h"
#include <scene/3d/mesh_instance_3d.h>
#include <scene/resources/3d/primitive_meshes.h>
// Generator based on a script, like GDScript, C# or NativeScript.
// The script is expected to properly handle multithreading.
class TreeGen2 : public DuoMesh {
	GDCLASS(TreeGen2, DuoMesh)
private:
	Array transforms;
	Transform3D leaf_transform;
	Vector3i splits;

protected:
	void _create_mesh_array(Array &p_arr, Dictionary tr) const override;
	static void _bind_methods() {
		ClassDB::bind_method(D_METHOD("get_transforms"), &TreeGen2::get_transforms);
		ClassDB::bind_method(D_METHOD("set_transforms"), &TreeGen2::set_transforms);
		ClassDB::bind_method(D_METHOD("get_leaf_transform"), &TreeGen2::get_leaf_transform);
		ClassDB::bind_method(D_METHOD("set_leaf_transform"), &TreeGen2::set_leaf_transform);
		ClassDB::bind_method(D_METHOD("get_splits"), &TreeGen2::get_splits);
		ClassDB::bind_method(D_METHOD("set_splits"), &TreeGen2::set_splits);
		
	
		ADD_PROPERTY(PropertyInfo(Variant::TRANSFORM3D, "leaf_transform", PROPERTY_HINT_RANGE, "0.0,20.0,0.01"), "set_leaf_transform", "get_leaf_transform");
		ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "transforms", PROPERTY_HINT_ARRAY_TYPE, "Transform3D"), "set_transforms", "get_transforms");
		ADD_PROPERTY(PropertyInfo(Variant::VECTOR3I, "splits",  PROPERTY_HINT_RANGE, "3,20,1"), "set_splits", "get_splits");
	
	}

public:

	GETSETINPH(TreeGen2,const, Transform3D, leaf_transform, _request_update);
	GETSETINPH(TreeGen2,const, Array, transforms, _request_update);
	GETSETINPH(TreeGen2,const, Vector3i, splits, _request_update);

	void updateParts();
	Dictionary createTree() const override;

	

	TreeGen2(){
	};
};

#endif // TREE_GEN_H
