#pragma once

#include "dual_mesh.h"

#include <godot_cpp/variant/packed_vector3_array.hpp>
#include <godot_cpp/variant/transform3d.hpp>

namespace godot {

/// Procedural tree generator.
///
/// Builds a branching tree mesh by recursively splitting a trunk into child
/// branches. The shape is controlled by parameters grouped into Branches,
/// Trunk, and Mesh sections. Each float parameter is exposed with a sensible
/// PROPERTY_HINT_RANGE so the inspector presents usable sliders.
class TreeGenerator : public DualMesh {
	GDCLASS(TreeGenerator, DualMesh)

private:
	// --- Branching parameters ---
	int branch_depth = 3;
	float distribution = 0.5;
	float auto_split = 0.5;
	PackedVector3Array splits = PackedVector3Array();
	Array sid;
	Array sid_left;
	float rotation = 0;
	float radius_decay = 1.0;
	float length_decay = 0.5;

	// --- Trunk parameters ---
	float radius = 0.1;
	float height = 2.0;
	float width = 3.0;
	float leaf_scale = 1.0;

	// --- Mesh parameters ---
	int radial_segments = 10;
	int rings = 3;

	Array transforms;

protected:
	static void _bind_methods();
	void _create_mesh_array(Array &p_arr, const Dictionary &p_tr) const override;

public:
	void set_branch_depth(int p_branch_depth);
	int get_branch_depth() const;
	void set_distribution(float p_distribution);
	float get_distribution() const;
	void set_auto_split(float p_auto_split);
	float get_auto_split() const;
	void set_splits(const PackedVector3Array p_splits);
	PackedVector3Array get_splits() const;
	void set_rotation(float p_rotation);
	float get_rotation() const;
	void set_radius_decay(float p_radius_decay);
	float get_radius_decay() const;
	void set_length_decay(float p_length_decay);
	float get_length_decay() const;

	void set_radius(float p_radius);
	float get_radius() const;
	void set_height(float p_height);
	float get_height() const;
	void set_width(float p_width);
	float get_width() const;
	void set_leaf_scale(float p_leaf_scale);
	float get_leaf_scale() const;

	void set_radial_segments(int p_segments);
	int get_radial_segments() const;
	void set_rings(int p_rings);
	int get_rings() const;

	void set_transforms(const Array p_transforms);
	Array get_transforms();

	void update_parts();
	Dictionary gen(float w, float h, Transform3D home) const;
	Dictionary add_tree_branch(Dictionary inf) const;
	Dictionary create_tree() const override;

	TreeGenerator() {}
};

} // namespace godot
