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
	RID mesh; // RenderingServer RID for the underlying mesh resource.
	mutable AABB aabb; // Computed bounding box, updated during _update().
	AABB custom_aabb; // User-overridable AABB.
	Ref<FlowerGenerator> mesh_a; // Optional secondary mesh (flowers/leaves).
	Ref<Material> material; // Material for the primary surface.
	bool flip_faces = false; // If true, normals are negated and winding reversed.

	// Cached surface metrics (primary = surface 0, secondary = surface 1).
	mutable int array_len = 0; // Vertex count, primary surface.
	mutable int index_array_len = 0; // Index count, primary surface.
	mutable int array_len_a = 0; // Vertex count, secondary surface.
	mutable int index_array_len_a = 0; // Index count, secondary surface.
	mutable bool pending_request = false; // True when the mesh needs rebuilding.
	void _update() const;

	// Called when the assigned FlowerGenerator changes, so the tree's
	// secondary surface is regenerated.
	void _on_mesh_a_changed();

protected:
	Mesh::PrimitiveType primitive_type = Mesh::PRIMITIVE_TRIANGLES;

	static void _bind_methods();

	// Subclasses fill `p_arr` with the primary surface geometry, using the
	// tree data in `p_tr` (keys: "tree", "leaf", "parent").
	virtual void _create_mesh_array(Array &p_arr, const Dictionary &p_tr) const = 0;
	void _request_update();

public:
	// --- Mesh surface interface (overrides for Godot's Mesh API) ---
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

	// Subclasses build and return the full tree data dictionary.
	virtual Dictionary create_tree() const = 0;

	Array leaf_transforms; // Transforms for placing leaves/flowers.

	// --- Secondary mesh (flowers/leaves) ---
	void set_mesh_a(const Ref<FlowerGenerator> &p_mesh);
	Ref<FlowerGenerator> get_mesh_a() const;

	// --- Material ---
	void set_material(const Ref<Material> &p_material);
	Ref<Material> get_material() const;

	Array get_mesh_arrays();

	// --- AABB ---
	void set_custom_aabb(const AABB &p_custom);
	AABB get_custom_aabb() const;

	// --- Flip faces ---
	void set_flip_faces(bool p_enable);
	bool get_flip_faces() const;

	DualMesh();
	~DualMesh();
};

} // namespace godot
