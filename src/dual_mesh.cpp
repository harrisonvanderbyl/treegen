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
	// Default leaf transform: a single transform rotated so leaves face up.
	leaf_transforms = Array();
	leaf_transforms.push_back(Transform3D(Basis(0, 0, 1, 0, 1, 0, 1, 0, 0), Vector3(0, 0, 0)));
}

DualMesh::~DualMesh() {
	RenderingServer::get_singleton()->free_rid(mesh);
}

// Rebuilds both mesh surfaces (primary + optional secondary) from scratch.
// Called lazily whenever a consumer reads mesh data while pending_request is set.
void DualMesh::_update() const {
	Dictionary tree_data = create_tree();

	// --- Primary surface (trunk/branches) ---
	Array primary_arrays;
	primary_arrays.resize(Mesh::ARRAY_MAX);
	_create_mesh_array(primary_arrays, tree_data);

	PackedVector3Array points = primary_arrays[Mesh::ARRAY_VERTEX];

	// Recompute the AABB from the primary surface vertices.
	aabb = AABB();
	int point_count = points.size();
	ERR_FAIL_COND(point_count == 0);
	{
		const Vector3 *read_ptr = points.ptr();
		for (int i = 0; i < point_count; i++) {
			if (i == 0) {
				aabb.position = read_ptr[i];
			} else {
				aabb.expand_to(read_ptr[i]);
			}
		}
	}

	PackedInt32Array indices = primary_arrays[Mesh::ARRAY_INDEX];

	// Optionally flip faces: negate normals and wind triangles backwards.
	if (flip_faces) {
		PackedVector3Array normals = primary_arrays[Mesh::ARRAY_NORMAL];

		if (normals.size() && indices.size()) {
			{
				int normal_count = normals.size();
				Vector3 *write_ptr = normals.ptrw();
				for (int i = 0; i < normal_count; i++) {
					write_ptr[i] = -write_ptr[i];
				}
			}
			{
				int index_count = indices.size();
				int32_t *write_ptr = indices.ptrw();
				for (int i = 0; i < index_count; i += 3) {
					SWAP(write_ptr[i + 0], write_ptr[i + 1]);
				}
			}
			primary_arrays[Mesh::ARRAY_NORMAL] = normals;
			primary_arrays[Mesh::ARRAY_INDEX] = indices;
		}
	}

	array_len = point_count;
	index_array_len = indices.size();

	// Upload the primary surface to the RenderingServer.
	RenderingServer::get_singleton()->mesh_clear(mesh);
	RenderingServer::get_singleton()->mesh_add_surface_from_arrays(mesh, (RenderingServer::PrimitiveType)primitive_type, primary_arrays);
	RenderingServer::get_singleton()->mesh_surface_set_material(mesh, 0, material.is_null() ? RID() : material->get_rid());

	// --- Secondary surface (flowers/leaves), if a FlowerGenerator is assigned ---
	Array secondary_arrays;
	secondary_arrays.resize(Mesh::ARRAY_MAX);
	if (mesh_a.is_valid()) {
		Array tree = tree_data["tree"];
		Array leaf = tree_data["leaf"];
		mesh_a->create_flower(secondary_arrays, leaf);

		PackedVector3Array secondary_points = secondary_arrays[Mesh::ARRAY_VERTEX];
		array_len_a = secondary_points.size();
		index_array_len_a = Array(secondary_arrays[Mesh::ARRAY_INDEX]).size();

		RenderingServer::get_singleton()->mesh_add_surface_from_arrays(mesh, (RenderingServer::PrimitiveType)primitive_type, secondary_arrays);
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

// ============================================================================
// Surface queries
//
// The mesh exposes up to two surfaces:
//   index 0 = primary (trunk/branches)
//   index 1 = secondary (flowers/leaves), only when mesh_a is set
// ============================================================================

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
	if (p_index == 0) {
		return array_len;
	}
	return array_len_a;
}

int32_t DualMesh::_surface_get_array_index_len(int32_t p_index) const {
	ERR_FAIL_INDEX_V(p_index, 1, -1);
	if (pending_request) {
		_update();
	}
	if (p_index == 0) {
		return index_array_len;
	}
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

// ============================================================================
// Property bindings
// ============================================================================

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

// ============================================================================
// Material / AABB / flip_faces setters
// ============================================================================

void DualMesh::set_material(const Ref<Material> &p_material) {
	material = p_material;
	if (!pending_request) {
		// Apply immediately; otherwise it will be applied during _update.
		RenderingServer::get_singleton()->mesh_surface_set_material(mesh, 0, material.is_null() ? RID() : material->get_rid());
		notify_property_list_changed();
		emit_changed();
	}
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
