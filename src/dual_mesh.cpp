#include "dual_mesh.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/packed_int32_array.hpp>
#include <godot_cpp/variant/packed_vector3_array.hpp>

namespace godot {

DualMesh::DualMesh() {
	flip_faces = false;
	mesh = RenderingServer::get_singleton()->mesh_create();
	primitive_type = Mesh::PRIMITIVE_TRIANGLES;
	pending_request = true;
	leaf_transforms = Array();
	leaf_transforms.push_back(Transform3D(Basis(0, 0, 1, 0, 1, 0, 1, 0, 0), Vector3(0, 0, 0)));
}

DualMesh::~DualMesh() {
	RenderingServer::get_singleton()->free_rid(mesh);
}

void DualMesh::_update() const {
	Dictionary tr = create_tree();
	Array arr;
	arr.resize(Mesh::ARRAY_MAX);
	_create_mesh_array(arr, tr);

	PackedVector3Array points = arr[Mesh::ARRAY_VERTEX];

	aabb = AABB();

	int pc = points.size();
	ERR_FAIL_COND(pc == 0);
	{
		const Vector3 *r = points.ptr();
		for (int i = 0; i < pc; i++) {
			if (i == 0) {
				aabb.position = r[i];
			} else {
				aabb.expand_to(r[i]);
			}
		}
	}

	PackedInt32Array indices = arr[Mesh::ARRAY_INDEX];

	if (flip_faces) {
		PackedVector3Array normals = arr[Mesh::ARRAY_NORMAL];

		if (normals.size() && indices.size()) {
			{
				int nc = normals.size();
				Vector3 *w = normals.ptrw();
				for (int i = 0; i < nc; i++) {
					w[i] = -w[i];
				}
			}

			{
				int ic = indices.size();
				int32_t *w = indices.ptrw();
				for (int i = 0; i < ic; i += 3) {
					SWAP(w[i + 0], w[i + 1]);
				}
			}
			arr[Mesh::ARRAY_NORMAL] = normals;
			arr[Mesh::ARRAY_INDEX] = indices;
		}
	}

	array_len = pc;
	index_array_len = indices.size();
	// in with the new
	RenderingServer::get_singleton()->mesh_clear(mesh);
	RenderingServer::get_singleton()->mesh_add_surface_from_arrays(mesh, (RenderingServer::PrimitiveType)primitive_type, arr);
	RenderingServer::get_singleton()->mesh_surface_set_material(mesh, 0, material.is_null() ? RID() : material->get_rid());

	Array arr2;
	arr2.resize(Mesh::ARRAY_MAX);
	if (mesh_a.is_valid()) {
		Array tree = tr["tree"];
		Array leaf = tr["leaf"];
		mesh_a->create_flower(arr2, leaf);

		PackedVector3Array pointsa = arr2[Mesh::ARRAY_VERTEX];

		array_len_a = pointsa.size();
		index_array_len_a = Array(arr2[Mesh::ARRAY_INDEX]).size();
		// in with the new
		RenderingServer::get_singleton()->mesh_add_surface_from_arrays(mesh, (RenderingServer::PrimitiveType)primitive_type, arr2);
		RenderingServer::get_singleton()->mesh_surface_set_material(mesh, 1, mesh_a->get_material().is_null() ? RID() : mesh_a->get_material()->get_rid());
	}
	pending_request = false;

	const_cast<DualMesh *>(this)->emit_changed();
}

void DualMesh::_request_update() {
	if (pending_request) {
		return;
	}
	_update();
}

int32_t DualMesh::_get_surface_count() const {
	if (pending_request) {
		_update();
	}
	return 1;
}

int32_t DualMesh::_surface_get_array_len(int32_t p_index) const {
	ERR_FAIL_INDEX_V(p_index, 1, -1);
	if (pending_request) {
		_update();
	}
	if (p_index == 0)
		return array_len;
	else
		return array_len_a;
}

int32_t DualMesh::_surface_get_array_index_len(int32_t p_index) const {
	ERR_FAIL_INDEX_V(p_index, 1, -1);
	if (pending_request) {
		_update();
	}
	if (p_index == 0)
		return index_array_len;
	else
		return index_array_len_a;
}

Array DualMesh::_surface_get_arrays(int32_t p_index) const {
	ERR_FAIL_INDEX_V(p_index, 1, Array());
	if (pending_request) {
		_update();
	}

	return RenderingServer::get_singleton()->mesh_surface_get_arrays(mesh, 0);
}

TypedArray<Array> DualMesh::_surface_get_blend_shape_arrays(int32_t p_index) const {
	ERR_FAIL_INDEX_V(p_index, 1, Array());
	if (pending_request) {
		_update();
	}

	return Array();
}

Dictionary DualMesh::_surface_get_lods(int32_t p_index) const {
	return Dictionary();
}

uint32_t DualMesh::_surface_get_format(int32_t p_index) const {
	ERR_FAIL_INDEX_V(p_index, 1, 0);
	if (pending_request) {
		_update();
	}

	return (uint32_t)(Mesh::ARRAY_FORMAT_VERTEX | Mesh::ARRAY_FORMAT_NORMAL | Mesh::ARRAY_FORMAT_TANGENT | Mesh::ARRAY_FORMAT_TEX_UV | Mesh::ARRAY_FORMAT_INDEX);
}

uint32_t DualMesh::_surface_get_primitive_type(int32_t p_index) const {
	return (uint32_t)primitive_type;
}

void DualMesh::set_mesh_a(const Ref<FlowerGenerator> &p_mesh) {
	mesh_a = p_mesh;
	pending_request = true;
	_request_update();
}

Ref<FlowerGenerator> DualMesh::get_mesh_a() const {
	return mesh_a;
}

void DualMesh::_surface_set_material(int32_t p_index, const Ref<Material> &p_material) {
	ERR_FAIL_INDEX(p_index, 1);

	set_material(p_material);
}

Ref<Material> DualMesh::_surface_get_material(int32_t p_index) const {
	ERR_FAIL_INDEX_V(p_index, 1, nullptr);

	return material;
}

int32_t DualMesh::_get_blend_shape_count() const {
	return 0;
}

StringName DualMesh::_get_blend_shape_name(int32_t p_index) const {
	return StringName();
}

void DualMesh::_set_blend_shape_name(int32_t p_index, const StringName &p_name) {
}

AABB DualMesh::_get_aabb() const {
	if (pending_request) {
		_update();
	}

	return aabb;
}

RID DualMesh::_get_rid() const {
	if (pending_request) {
		_update();
	}
	return mesh;
}

void DualMesh::_bind_methods() {
	ClassDB::bind_method(D_METHOD("_update"), &DualMesh::_update);

	ClassDB::bind_method(D_METHOD("set_mesh_a", "mesh"), &DualMesh::set_mesh_a);
	ClassDB::bind_method(D_METHOD("get_mesh_a"), &DualMesh::get_mesh_a);

	ClassDB::bind_method(D_METHOD("set_material", "material"), &DualMesh::set_material);
	ClassDB::bind_method(D_METHOD("get_material"), &DualMesh::get_material);

	ClassDB::bind_method(D_METHOD("get_mesh_arrays"), &DualMesh::get_mesh_arrays);

	ClassDB::bind_method(D_METHOD("set_custom_aabb", "aabb"), &DualMesh::set_custom_aabb);
	ClassDB::bind_method(D_METHOD("get_custom_aabb"), &DualMesh::get_custom_aabb);

	ClassDB::bind_method(D_METHOD("set_flip_faces", "flip_faces"), &DualMesh::set_flip_faces);
	ClassDB::bind_method(D_METHOD("get_flip_faces"), &DualMesh::get_flip_faces);

	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "mesh_a", PROPERTY_HINT_RESOURCE_TYPE, "FlowerGenerator"), "set_mesh_a", "get_mesh_a");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "material", PROPERTY_HINT_RESOURCE_TYPE, "Material"), "set_material", "get_material");
	ADD_PROPERTY(PropertyInfo(Variant::AABB, "custom_aabb", PROPERTY_HINT_NONE, ""), "set_custom_aabb", "get_custom_aabb");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "flip_faces"), "set_flip_faces", "get_flip_faces");
}

void DualMesh::set_material(const Ref<Material> &p_material) {
	material = p_material;
	if (!pending_request) {
		// just apply it, else it'll happen when _update is called.
		RenderingServer::get_singleton()->mesh_surface_set_material(mesh, 0, material.is_null() ? RID() : material->get_rid());

		notify_property_list_changed();
		emit_changed();
	};
}

Ref<Material> DualMesh::get_material() const {
	return material;
}

Array DualMesh::get_mesh_arrays() {
	return _surface_get_arrays(0);
}

void DualMesh::set_custom_aabb(const AABB &p_custom) {
	custom_aabb = p_custom;
	RenderingServer::get_singleton()->mesh_set_custom_aabb(mesh, custom_aabb);
	emit_changed();
}

AABB DualMesh::get_custom_aabb() const {
	return custom_aabb;
}

void DualMesh::set_flip_faces(bool p_enable) {
	flip_faces = p_enable;
	_request_update();
}

bool DualMesh::get_flip_faces() const {
	return flip_faces;
}

} // namespace godot
