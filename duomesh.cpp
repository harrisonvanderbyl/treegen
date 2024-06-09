
#include "duomesh.h"
#include "servers/rendering_server.h"
/**
  DuoMesh
*/
void DuoMesh::_update() const {
	Dictionary tr = createTree();
	Array arr;
	arr.resize(RS::ARRAY_MAX);
	_create_mesh_array(arr, tr);

	Vector<Vector3> points = arr[RS::ARRAY_VERTEX];

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

	Vector<int> indices = arr[RS::ARRAY_INDEX];

	if (flip_faces) {
		Vector<Vector3> normals = arr[RS::ARRAY_NORMAL];

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
				int *w = indices.ptrw();
				for (int i = 0; i < ic; i += 3) {
					SWAP(w[i + 0], w[i + 1]);
				}
			}
			arr[RS::ARRAY_NORMAL] = normals;
			arr[RS::ARRAY_INDEX] = indices;
		}
	}

	array_len = pc;
	index_array_len = indices.size();
	// in with the new
	RenderingServer::get_singleton()->mesh_clear(mesh);
	RenderingServer::get_singleton()->mesh_add_surface_from_arrays(mesh, (RenderingServer::PrimitiveType)primitive_type, arr);
	RenderingServer::get_singleton()->mesh_surface_set_material(mesh, 0, material.is_null() ? RID() : material->get_rid());

	Array arr2;
	arr2.resize(RS::ARRAY_MAX);
	if (mesh_a.is_valid()) {
		Array tree = tr["tree"];
		Array leaf = tr["leaf"];
		mesh_a->create_flower(arr2, leaf);

		Vector<Vector3> pointsa = arr2[RS::ARRAY_VERTEX];

		array_len_a = pointsa.size();
		index_array_len_a = Array(arr2[RS::ARRAY_INDEX]).size();
		// in with the new
		RenderingServer::get_singleton()->mesh_add_surface_from_arrays(mesh, (RenderingServer::PrimitiveType)primitive_type, arr2);
		RenderingServer::get_singleton()->mesh_surface_set_material(mesh, 1, mesh_a->get_material().is_null() ? RID() : mesh_a->get_material()->get_rid());
	}
	pending_request = false;

	clear_cache();

	const_cast<DuoMesh *>(this)->emit_changed();
}

void DuoMesh::_request_update() {
	if (pending_request) {
		return;
	}
	_update();
}

int DuoMesh::get_surface_count() const {
	if (pending_request) {
		_update();
	}
	return 1;
}

int DuoMesh::surface_get_array_len(int p_idx) const {
	ERR_FAIL_INDEX_V(p_idx, 1, -1);
	if (pending_request) {
		_update();
	}
	if (p_idx == 0)
		return array_len;
	else
		return array_len_a;
}

int DuoMesh::surface_get_array_index_len(int p_idx) const {
	ERR_FAIL_INDEX_V(p_idx, 1, -1);
	if (pending_request) {
		_update();
	}
	if (p_idx == 0)
		return index_array_len;
	else
		return index_array_len_a;
}

Array DuoMesh::surface_get_arrays(int p_surface) const {
	ERR_FAIL_INDEX_V(p_surface, 1, Array());
	if (pending_request) {
		_update();
	}

	return RenderingServer::get_singleton()->mesh_surface_get_arrays(mesh, 0);
}

TypedArray<Array> DuoMesh::surface_get_blend_shape_arrays(int p_surface) const {
	ERR_FAIL_INDEX_V(p_surface, 1, Array());
	if (pending_request) {
		_update();
	}

	return Array();
}

BitField<Mesh::ArrayFormat> DuoMesh::surface_get_format(int p_idx) const {
	ERR_FAIL_INDEX_V(p_idx, 1, 0);
	if (pending_request) {
		_update();
	}

	return RS::ARRAY_FORMAT_VERTEX | RS::ARRAY_FORMAT_NORMAL | RS::ARRAY_FORMAT_TANGENT | RS::ARRAY_FORMAT_TEX_UV | RS::ARRAY_FORMAT_INDEX;
}

Mesh::PrimitiveType DuoMesh::surface_get_primitive_type(int p_idx) const {
	return primitive_type;
}

void DuoMesh::set_mesh_a(const Ref<FlowerGen> &p_mesh) {
	mesh_a = p_mesh;
	pending_request = true;
	_request_update();
}

Ref<FlowerGen> DuoMesh::get_mesh_a() const {
	return mesh_a;
}

void DuoMesh::surface_set_material(int p_idx, const Ref<Material> &p_material) {
	ERR_FAIL_INDEX(p_idx, 1);

	set_material(p_material);
}

Ref<Material> DuoMesh::surface_get_material(int p_idx) const {
	ERR_FAIL_INDEX_V(p_idx, 1, nullptr);

	return material;
}

int DuoMesh::get_blend_shape_count() const {
	return 0;
}

StringName DuoMesh::get_blend_shape_name(int p_index) const {
	return StringName();
}

void DuoMesh::set_blend_shape_name(int p_index, const StringName &p_name) {
}

AABB DuoMesh::get_aabb() const {
	if (pending_request) {
		_update();
	}

	return aabb;
}

RID DuoMesh::get_rid() const {
	if (pending_request) {
		_update();
	}
	return mesh;
}

void DuoMesh::_bind_methods() {
	ClassDB::bind_method(D_METHOD("_update"), &DuoMesh::_update);

	ClassDB::bind_method(D_METHOD("set_mesh_a", "mesh"), &DuoMesh::set_mesh_a);
	ClassDB::bind_method(D_METHOD("get_mesh_a"), &DuoMesh::get_mesh_a);

	ClassDB::bind_method(D_METHOD("set_material", "material"), &DuoMesh::set_material);
	ClassDB::bind_method(D_METHOD("get_material"), &DuoMesh::get_material);

	ClassDB::bind_method(D_METHOD("get_mesh_arrays"), &DuoMesh::get_mesh_arrays);

	ClassDB::bind_method(D_METHOD("set_custom_aabb", "aabb"), &DuoMesh::set_custom_aabb);
	ClassDB::bind_method(D_METHOD("get_custom_aabb"), &DuoMesh::get_custom_aabb);

	ClassDB::bind_method(D_METHOD("set_flip_faces", "flip_faces"), &DuoMesh::set_flip_faces);
	ClassDB::bind_method(D_METHOD("get_flip_faces"), &DuoMesh::get_flip_faces);

	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "mesh_a", PROPERTY_HINT_RESOURCE_TYPE, "FlowerGen"), "set_mesh_a", "get_mesh_a");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "material", PROPERTY_HINT_RESOURCE_TYPE, "Material"), "set_material", "get_material");
	ADD_PROPERTY(PropertyInfo(Variant::AABB, "custom_aabb", PROPERTY_HINT_NONE, ""), "set_custom_aabb", "get_custom_aabb");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "flip_faces"), "set_flip_faces", "get_flip_faces");
}

void DuoMesh::set_material(const Ref<Material> &p_material) {
	material = p_material;
	if (!pending_request) {
		// just apply it, else it'll happen when _update is called.
		RenderingServer::get_singleton()->mesh_surface_set_material(mesh, 0, material.is_null() ? RID() : material->get_rid());

		notify_property_list_changed();
		emit_changed();
	};
}

Ref<Material> DuoMesh::get_material() const {
	return material;
}

Array DuoMesh::get_mesh_arrays() {
	return surface_get_arrays(0);
}

void DuoMesh::set_custom_aabb(const AABB &p_custom) {
	custom_aabb = p_custom;
	RS::get_singleton()->mesh_set_custom_aabb(mesh, custom_aabb);
	emit_changed();
}

AABB DuoMesh::get_custom_aabb() const {
	return custom_aabb;
}

void DuoMesh::set_flip_faces(bool p_enable) {
	flip_faces = p_enable;
	_request_update();
}

bool DuoMesh::get_flip_faces() const {
	return flip_faces;
}
