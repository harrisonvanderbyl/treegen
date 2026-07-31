#pragma once

#include "flower_generator.h"

#include <godot_cpp/classes/material.hpp>
#include <godot_cpp/classes/mesh.hpp>
#include <godot_cpp/classes/primitive_mesh.hpp>
#include <godot_cpp/classes/rendering_server.hpp>
#include <godot_cpp/variant/aabb.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/rid.hpp>
#include <godot_cpp/variant/typed_array.hpp>

namespace godot {

/// Base class for procedural meshes that emit two surfaces: a primary mesh
/// (produced by the subclass via _create_mesh_array) and an optional secondary
/// mesh produced by a FlowerGenerator driven by the leaf transforms.
class DualMesh : public Mesh {
	GDCLASS(DualMesh, Mesh)

private:
	RID mesh;
	mutable AABB aabb;
	AABB custom_aabb;
	Ref<FlowerGenerator> mesh_a;
	Ref<Material> material;
	bool flip_faces = false;

	mutable int array_len = 0;
	mutable int index_array_len = 0;
	mutable int array_len_a = 0;
	mutable int index_array_len_a = 0;
	mutable bool pending_request = false;
	void _update() const;

protected:
	Mesh::PrimitiveType primitive_type = Mesh::PRIMITIVE_TRIANGLES;

	static void _bind_methods();

	virtual void _create_mesh_array(Array &p_arr, const Dictionary &p_tr) const = 0;
	void _request_update();

public:
	int32_t _get_surface_count() const override;
	int32_t _surface_get_array_len(int32_t p_index) const override;
	int32_t _surface_get_array_index_len(int32_t p_index) const override;
	Array _surface_get_arrays(int32_t p_index) const override;
	TypedArray<Array> _surface_get_blend_shape_arrays(int32_t p_index) const override;
	Dictionary _surface_get_lods(int32_t p_index) const override;
	uint32_t _surface_get_format(int32_t p_index) const override;
	uint32_t _surface_get_primitive_type(int32_t p_index) const override;
	void _surface_set_material(int32_t p_index, const Ref<Material> &p_material) override;
	Ref<Material> _surface_get_material(int32_t p_index) const override;
	int32_t _get_blend_shape_count() const override;
	StringName _get_blend_shape_name(int32_t p_index) const override;
	void _set_blend_shape_name(int32_t p_index, const StringName &p_name) override;
	AABB _get_aabb() const override;
	RID _get_rid() const override;

	virtual Dictionary create_tree() const = 0;

	Array leaf_transforms;

	void set_mesh_a(const Ref<FlowerGenerator> &p_mesh);
	Ref<FlowerGenerator> get_mesh_a() const;

	void set_material(const Ref<Material> &p_material);
	Ref<Material> get_material() const;

	Array get_mesh_arrays();

	void set_custom_aabb(const AABB &p_custom);
	AABB get_custom_aabb() const;

	void set_flip_faces(bool p_enable);
	bool get_flip_faces() const;

	DualMesh();
	~DualMesh();
};

} // namespace godot
