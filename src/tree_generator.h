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
	int branch_depth = 3; // Recursion depth: how many times the trunk splits.
	float distribution = 0.5; // Bends split positions toward the trunk center.
	float auto_split = 0.5; // Auto-generated split angle for each branch.
	PackedVector3Array splits = PackedVector3Array(); // Per-split control vectors (x=angle, y=weight, z=unused).
	Array branch_orientations; // Per-split orientation transform applied to each child branch.
	Array parent_alignments; // Per-split transform aligning a parent's top to its child.
	float rotation = 0; // Rotates the whole branch arrangement around the trunk.
	float radius_decay = 1.0; // How quickly child branch radius shrinks.
	float length_decay = 0.5; // How quickly child branch length shrinks.

	// --- Trunk parameters ---
	float radius = 0.1; // Base trunk radius.
	float height = 2.0; // Base trunk height.
	float width = 3.0; // Base trunk width.
	float leaf_scale = 1.0; // Scale of the leaf/flower transforms.

	// --- Mesh parameters ---
	int radial_segments = 10; // Cylinder segments around the branch circumference.
	int rings = 3; // Rings along each branch segment for smoothness.

	Array transforms; // Cached tree branch transforms.

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
