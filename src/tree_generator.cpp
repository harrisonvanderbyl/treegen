#include "tree_generator.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/packed_float32_array.hpp>
#include <godot_cpp/variant/packed_int32_array.hpp>
#include <godot_cpp/variant/packed_vector2_array.hpp>
#include <godot_cpp/variant/packed_vector3_array.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

namespace godot {

// Reusable identity transform, used as a base for building scaled/rotated transforms.
static const Transform3D IDENTITY = Transform3D();

// Shorthand for PI; the codebase uses 3.1415 throughout for historical reasons.
static constexpr float PI = 3.1415f;
static constexpr float TAU = 2.0f * PI;

void TreeGenerator::_bind_methods() {
	// --- Bind getters/setters ---
	ClassDB::bind_method(D_METHOD("set_branch_depth", "branch_depth"), &TreeGenerator::set_branch_depth);
	ClassDB::bind_method(D_METHOD("get_branch_depth"), &TreeGenerator::get_branch_depth);
	ClassDB::bind_method(D_METHOD("set_distribution", "distribution"), &TreeGenerator::set_distribution);
	ClassDB::bind_method(D_METHOD("get_distribution"), &TreeGenerator::get_distribution);
	ClassDB::bind_method(D_METHOD("set_auto_split", "auto_split"), &TreeGenerator::set_auto_split);
	ClassDB::bind_method(D_METHOD("get_auto_split"), &TreeGenerator::get_auto_split);
	ClassDB::bind_method(D_METHOD("set_splits", "splits"), &TreeGenerator::set_splits);
	ClassDB::bind_method(D_METHOD("get_splits"), &TreeGenerator::get_splits);
	ClassDB::bind_method(D_METHOD("set_rotation", "rotation"), &TreeGenerator::set_rotation);
	ClassDB::bind_method(D_METHOD("get_rotation"), &TreeGenerator::get_rotation);
	ClassDB::bind_method(D_METHOD("set_radius_decay", "radius_decay"), &TreeGenerator::set_radius_decay);
	ClassDB::bind_method(D_METHOD("get_radius_decay"), &TreeGenerator::get_radius_decay);
	ClassDB::bind_method(D_METHOD("set_length_decay", "length_decay"), &TreeGenerator::set_length_decay);
	ClassDB::bind_method(D_METHOD("get_length_decay"), &TreeGenerator::get_length_decay);
	ClassDB::bind_method(D_METHOD("set_radius", "radius"), &TreeGenerator::set_radius);
	ClassDB::bind_method(D_METHOD("get_radius"), &TreeGenerator::get_radius);
	ClassDB::bind_method(D_METHOD("set_height", "height"), &TreeGenerator::set_height);
	ClassDB::bind_method(D_METHOD("get_height"), &TreeGenerator::get_height);
	ClassDB::bind_method(D_METHOD("set_width", "width"), &TreeGenerator::set_width);
	ClassDB::bind_method(D_METHOD("get_width"), &TreeGenerator::get_width);
	ClassDB::bind_method(D_METHOD("set_radial_segments", "radial_segments"), &TreeGenerator::set_radial_segments);
	ClassDB::bind_method(D_METHOD("get_radial_segments"), &TreeGenerator::get_radial_segments);
	ClassDB::bind_method(D_METHOD("set_rings", "rings"), &TreeGenerator::set_rings);
	ClassDB::bind_method(D_METHOD("get_rings"), &TreeGenerator::get_rings);
	ClassDB::bind_method(D_METHOD("set_leaf_scale", "leaf_scale"), &TreeGenerator::set_leaf_scale);
	ClassDB::bind_method(D_METHOD("get_leaf_scale"), &TreeGenerator::get_leaf_scale);
	ClassDB::bind_method(D_METHOD("get_transforms"), &TreeGenerator::get_transforms);
	ClassDB::bind_method(D_METHOD("gen", "w", "h", "home"), &TreeGenerator::gen);
	ClassDB::bind_method(D_METHOD("add_tree_branch", "inf"), &TreeGenerator::add_tree_branch);
	ClassDB::bind_method(D_METHOD("create_tree"), &TreeGenerator::create_tree);

	// --- Branching parameters ---
	ADD_GROUP("Branches", "branch_");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "branch_depth", PROPERTY_HINT_RANGE, "1,10,1"), "set_branch_depth", "get_branch_depth");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "branch_distribution", PROPERTY_HINT_RANGE, "0.0,2.0,0.01"), "set_distribution", "get_distribution");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "branch_auto_split", PROPERTY_HINT_RANGE, "0.0,2.0,0.01"), "set_auto_split", "get_auto_split");
	ADD_PROPERTY(PropertyInfo(Variant::PACKED_VECTOR3_ARRAY, "branch_splits", PROPERTY_HINT_NONE, ""), "set_splits", "get_splits");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "branch_rotation", PROPERTY_HINT_RANGE, "0.0,1.0,0.01"), "set_rotation", "get_rotation");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "branch_radius_decay", PROPERTY_HINT_RANGE, "0.0,1.0,0.01"), "set_radius_decay", "get_radius_decay");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "branch_length_decay", PROPERTY_HINT_RANGE, "0.01,2.0,0.01"), "set_length_decay", "get_length_decay");

	// --- Trunk parameters ---
	ADD_GROUP("Trunk", "trunk_");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "trunk_radius", PROPERTY_HINT_RANGE, "0.01,1.0,0.01"), "set_radius", "get_radius");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "trunk_height", PROPERTY_HINT_RANGE, "0.01,10.0,0.01"), "set_height", "get_height");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "trunk_width", PROPERTY_HINT_RANGE, "0.01,10.0,0.01"), "set_width", "get_width");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "trunk_leaf_scale", PROPERTY_HINT_RANGE, "0.0,20.0,0.01"), "set_leaf_scale", "get_leaf_scale");

	// --- Mesh parameters ---
	ADD_GROUP("Mesh", "mesh_");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "mesh_radial_segments", PROPERTY_HINT_RANGE, "3,20,1"), "set_radial_segments", "get_radial_segments");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "mesh_rings", PROPERTY_HINT_RANGE, "0,10,1"), "set_rings", "get_rings");
}

// ============================================================================
// Getters / setters
// ============================================================================

void TreeGenerator::set_branch_depth(int p_branch_depth) {
	branch_depth = p_branch_depth;
	_request_update();
}

int TreeGenerator::get_branch_depth() const {
	return branch_depth;
}

void TreeGenerator::set_distribution(float p_distribution) {
	distribution = p_distribution;
	_request_update();
}

float TreeGenerator::get_distribution() const {
	return distribution;
}

void TreeGenerator::set_auto_split(float p_auto_split) {
	auto_split = p_auto_split;

	// Ensure there are at least two split entries so the trunk always forks.
	if (splits.size() < 2) {
		splits.clear();
		splits.push_back(Vector3(0, 0, 0));
		splits.push_back(Vector3(0, 0, 0));
	}

	// Rebuild the split angle (x component) for every split based on the new
	// auto_split value. The position along the trunk is bent by `distribution`.
	PackedVector3Array rebuilt;
	for (int i = 0; i < splits.size(); i++) {
		float position_along_trunk = float(i) / float(splits.size());
		float bent_position = ((position_along_trunk - 0.5f) * distribution) + 0.5f;
		rebuilt.append(Vector3(bent_position * auto_split, 0, 0));
	}
	splits = rebuilt;

	update_parts();
	_request_update();
}

float TreeGenerator::get_auto_split() const {
	return auto_split;
}

void TreeGenerator::set_splits(PackedVector3Array p_splits) {
	// Normalize the split weights (y component) so their squared sum is 1.
	// This keeps the relative influence of each split stable regardless of
	// the raw values the user enters.
	float total_weight_sq = 0.0;
	for (int i = 0; i < p_splits.size(); i++) {
		total_weight_sq += p_splits[i].y * p_splits[i].y;
	}
	float total_weight = MAX(sqrt(total_weight_sq), 0.0001);

	PackedVector3Array normalized;
	for (int i = 0; i < p_splits.size(); i++) {
		normalized.append(Vector3(p_splits[i].x, p_splits[i].y / total_weight, p_splits[i].z));
	}
	splits = normalized;

	update_parts();
	_request_update();
}

PackedVector3Array TreeGenerator::get_splits() const {
	return splits;
}

void TreeGenerator::set_rotation(float p_rotation) {
	rotation = p_rotation;
	update_parts();
	_request_update();
}

float TreeGenerator::get_rotation() const {
	return rotation;
}

void TreeGenerator::set_radius_decay(float p_radius_decay) {
	radius_decay = p_radius_decay;
	_request_update();
}

float TreeGenerator::get_radius_decay() const {
	return radius_decay;
}

void TreeGenerator::set_length_decay(float p_length_decay) {
	length_decay = p_length_decay;
	_request_update();
}

float TreeGenerator::get_length_decay() const {
	return length_decay;
}

void TreeGenerator::set_radius(float p_radius) {
	radius = p_radius;
	_request_update();
}

float TreeGenerator::get_radius() const {
	return radius;
}

void TreeGenerator::set_height(float p_height) {
	height = p_height;
	_request_update();
}

float TreeGenerator::get_height() const {
	return height;
}

void TreeGenerator::set_width(float p_width) {
	width = p_width;
	_request_update();
}

float TreeGenerator::get_width() const {
	return width;
}

void TreeGenerator::set_radial_segments(int p_segments) {
	radial_segments = p_segments;
	update_parts();
	_request_update();
}

int TreeGenerator::get_radial_segments() const {
	return radial_segments;
}

void TreeGenerator::set_rings(int p_rings) {
	rings = p_rings;
	_request_update();
}

int TreeGenerator::get_rings() const {
	return rings;
}

void TreeGenerator::set_leaf_scale(float p_leaf_scale) {
	leaf_scale = p_leaf_scale;
	_request_update();
}

float TreeGenerator::get_leaf_scale() const {
	return leaf_scale;
}

Array TreeGenerator::get_transforms() {
	if (!transforms.is_empty()) {
		return transforms;
	}
	// Lazily build the tree and cache the branch transforms.
	Dictionary tree_data = create_tree();
	transforms = tree_data["tree"];
	return transforms;
}

// Builds a single branch descriptor at the top of `home` with width `w` and
// height `h`. The descriptor contains:
//   - "branches": child branch descriptors (one per split)
//   - "myTransform": the transform of this branch's own cylinder
//   - "leaftransform": transform used to place leaves/flowers at this branch
Dictionary TreeGenerator::gen(float w, float h, Transform3D home) const {
	Dictionary branch;
	// Move to the top of the parent branch; children emanate from here.
	Transform3D branch_origin = home.translated_local(Vector3(0.0, h, 0.0));

	branch["branches"] = Array{};
	for (int i = 0; i < splits.size(); i++) {
		Dictionary child;
		float position_along_trunk = float(i) / float(splits.size());
		float bent_position = ((position_along_trunk - 0.5f) * distribution) + 0.5f;

		child["tran"] = (branch_origin * Transform3D(branch_orientations[i]));
		child["w"] = sqrt((w * w) * radius_decay * (1.0f - bent_position));
		child["h"] = sqrt(h * h * (1.0f - bent_position) * length_decay);
		Array(branch["branches"]).append(child);
	}

	// This branch's own cylinder: centered at half height, scaled to (w, h, w).
	branch["myTransform"] = home.translated_local(Vector3(0.0, h / 2.0, 0.0)) * IDENTITY.scaled(Vector3(w, h, w));

	// Leaf/flower transform: centered at half height, scaled by leaf_scale.
	branch["leaftransform"] = home.translated_local(Vector3(0.0, h / 2.0, 0.0)) * IDENTITY.scaled(Vector3(leaf_scale, leaf_scale, leaf_scale));

	return branch;
}

// Recursively expands a branch descriptor into flat arrays of tree transforms,
// leaf transforms, and parent transforms. `inf` is a Dictionary with keys:
//   - "b": the branch descriptor (from gen())
//   - "d": remaining recursion depth
//   - "parent": the parent alignment transform
Dictionary TreeGenerator::add_tree_branch(Dictionary inf) const {
	Array leaf_transforms;
	Array tree_transforms;
	Array parent_transforms;

	Transform3D branch_transform = Dictionary(inf["b"])["myTransform"];
	Transform3D leaf_transform = Dictionary(inf["b"])["leaftransform"];
	int remaining_depth = int(inf["d"]);

	if (remaining_depth > 0) {
		Array children = Dictionary(inf["b"])["branches"];
		for (int child_index = 0; child_index < children.size(); child_index++) {
			Dictionary child = children[child_index];
			Dictionary child_branch = gen(child["w"], child["h"], child["tran"]);

			// Only recurse into branches that are thick enough to be visible.
			if (float(child["w"]) > 0.1f) {
				Dictionary recursion_input;
				child_branch["parent"] = branch_transform * Transform3D(parent_alignments[child_index]);
				recursion_input["b"] = child_branch;
				recursion_input["d"] = remaining_depth - 1;

				Dictionary recursion_result = add_tree_branch(recursion_input);
				Array child_trees = recursion_result["tree"];
				Array child_leaves = recursion_result["leaf"];
				Array child_parents = recursion_result["parent"];

				for (int i = 0; i < child_trees.size(); i++) {
					tree_transforms.append(child_trees[i]);
					parent_transforms.append(child_parents[i]);
				}
				for (int i = 0; i < child_leaves.size(); i++) {
					leaf_transforms.append(child_leaves[i]);
				}
			} else {
				// Thin branches terminate with a leaf at the first child only,
				// avoiding duplicate leaves when multiple splits collapse.
				if (child_index == 0) {
					leaf_transforms.append(leaf_transform);
				}
			}
		}
	} else {
		// At depth 0, this branch ends in a leaf.
		leaf_transforms.append(leaf_transform);
	}

	// Append this branch's own transform last (post-order).
	tree_transforms.append(branch_transform);
	parent_transforms.append(Dictionary(inf["b"])["parent"]);

	Dictionary result;
	result["tree"] = tree_transforms;
	result["leaf"] = leaf_transforms;
	result["parent"] = parent_transforms;
	return result;
}

Dictionary TreeGenerator::create_tree() const {
	// Generate the root branch from the trunk parameters.
	Dictionary root_branch = gen(width, height, Transform3D());

	// The root's "parent" is a dummy transform used for the base of the trunk.
	root_branch["parent"] = Transform3D().scaled(Vector3(width, height, width)).translated(Vector3(0, -0.5, 0));

	Dictionary input;
	input["b"] = root_branch;
	input["d"] = branch_depth;

	return add_tree_branch(input);
}

// Rebuilds the per-split orientation and parent-alignment transforms from the
// current splits, rotation, and radial_segments. Called whenever any of those
// parameters change.
void TreeGenerator::update_parts() {
	branch_orientations = Array();
	parent_alignments = Array();

	for (int i = 0; i < splits.size(); i++) {
		float position_along_trunk = float(i) / float(splits.size());

		// Each child branch is tilted by its split angle (x) around Z, then
		// rotated around Y to distribute children around the trunk.
		float tilt_angle = float(splits[i].x) * PI / 2.0f;
		float yaw_angle = TAU * (position_along_trunk + rotation);
		branch_orientations.append(
				IDENTITY.rotated(Vector3(0.0, 0.0, 1.0), tilt_angle)
						.rotated(Vector3(0.0, 1.0, 0.0), yaw_angle));

		// The parent alignment snaps each child to the nearest radial segment
		// so the top of the parent cylinder meets the child cleanly.
		float snapped_position = round((position_along_trunk + rotation) * float(radial_segments)) / float(radial_segments);
		float align_angle = TAU * snapped_position;
		parent_alignments.append(
				IDENTITY.rotated(Vector3(0, 1, 0), align_angle));
	}
}

// Builds the trunk/branch mesh geometry into `p_arr` from the tree data in
// `p_tr`. Each entry in the "tree" array becomes a tapered cylinder segment
// that interpolates from its own transform to its parent's top.
void TreeGenerator::_create_mesh_array(Array &p_arr, const Dictionary &p_tr) const {
#define ADD_TANGENT(m_x, m_y, m_z, m_d) \
	tangents.push_back(m_x);            \
	tangents.push_back(m_y);            \
	tangents.push_back(m_z);            \
	tangents.push_back(m_d);

	p_arr[Mesh::ARRAY_VERTEX] = Array();
	p_arr[Mesh::ARRAY_NORMAL] = Array();

	Array tree = p_tr["tree"];
	Array leaf = p_tr["leaf"];
	Array parent = p_tr["parent"];

	PackedVector3Array points;
	PackedVector3Array normals;
	PackedFloat32Array tangents;
	PackedVector2Array uvs;
	PackedInt32Array indices;

	int point = 0;
	int thisrow = 0;
	int prevrow = 0;

	// Generate a cylinder for each branch transform in the tree array.
	for (int cr = 0; cr < tree.size(); cr++) {
		Transform3D branch_transform = tree[cr];
		Transform3D parent_transform = parent[cr];
		// The parent's top is at y=0.5 in its local space.
		Transform3D parent_top = parent_transform * IDENTITY.translated_local(Vector3(0.0, 0.5, 0.0));

		for (int j = 0; j <= (rings + 1); j++) {
			float v = float(j) / float(rings + 1);

			// The cylinder spans y in [-0.4, 0.1] locally; v=0 is the bottom.
			float y = 0.1f - v * 0.5f;

			for (int i = 0; i <= radial_segments; i++) {
				float u = float(i) / float(radial_segments);

				float x = sin(u * TAU);
				float z = cos(u * TAU);

				// Point on the cylinder surface at radius `radius`.
				Vector3 surface_point = Vector3(x * radius, y, z * radius);

				// Interpolate between this branch and its parent's top so the
				// segment tapers and connects smoothly.
				Transform3D cur = branch_transform.interpolate_with(parent_top, v);
				Vector3 p = (cur * Transform3D(Basis(), surface_point)).origin;

				// Normal points outward from the cylinder's central axis.
				Vector3 center = (cur * Transform3D(Basis(), Vector3(0, y, 0))).origin;
				Vector3 normal = (p - center).normalized();

				// Tangent is perpendicular to the normal and the branch axis.
				Vector3 tangent = cur.basis[1].normalized().cross(normal).normalized();

				points.push_back(p);
				normals.push_back(normal);
				ADD_TANGENT(tangent.x, tangent.y, tangent.z, 1.0)
				uvs.push_back(Vector2(u, v));
				point++;

				// Emit two triangles per quad (once we have a full grid cell).
				if (i > 0 && j > 0) {
					indices.push_back(prevrow + i - 1);
					indices.push_back(prevrow + i);
					indices.push_back(thisrow + i - 1);

					indices.push_back(prevrow + i);
					indices.push_back(thisrow + i);
					indices.push_back(thisrow + i - 1);
				}
			}

			prevrow = thisrow;
			thisrow = point;
		}

		thisrow = point;
	}

	p_arr[Mesh::ARRAY_VERTEX] = points;
	p_arr[Mesh::ARRAY_NORMAL] = normals;
	p_arr[Mesh::ARRAY_TANGENT] = tangents;
	p_arr[Mesh::ARRAY_TEX_UV] = uvs;
	p_arr[Mesh::ARRAY_INDEX] = indices;
#undef ADD_TANGENT
}

} // namespace godot
