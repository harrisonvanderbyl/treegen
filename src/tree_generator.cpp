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

// Integer hash (finalizer from MurmurHash3). Provides good avalanche so that
// consecutive branch IDs produce uncorrelated random values.
uint32_t TreeGenerator::_hash(uint32_t x) {
	x ^= x >> 16u;
	x *= 0x85ebca6bu;
	x ^= x >> 13u;
	x *= 0xc2b2ae35u;
	x ^= x >> 16u;
	return x;
}

// Returns a value in [-1, 1] by hashing the current seed together with `id`.
float TreeGenerator::_rand(uint32_t id) const {
	uint32_t h = _hash(uint32_t(seed) ^ id);
	// Map [0, UINT32_MAX] → [-1, 1].
	return (float(h) / float(UINT32_MAX)) * 2.0f - 1.0f;
}

void TreeGenerator::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_seed", "seed"), &TreeGenerator::set_seed);
	ClassDB::bind_method(D_METHOD("get_seed"), &TreeGenerator::get_seed);
	ClassDB::bind_method(D_METHOD("set_randomness", "randomness"), &TreeGenerator::set_randomness);
	ClassDB::bind_method(D_METHOD("get_randomness"), &TreeGenerator::get_randomness);
	ClassDB::bind_method(D_METHOD("set_curvature", "curvature"), &TreeGenerator::set_curvature);
	ClassDB::bind_method(D_METHOD("get_curvature"), &TreeGenerator::get_curvature);
	ClassDB::bind_method(D_METHOD("set_gravity", "gravity"), &TreeGenerator::set_gravity);
	ClassDB::bind_method(D_METHOD("get_gravity"), &TreeGenerator::get_gravity);
	ClassDB::bind_method(D_METHOD("set_clump", "clump"), &TreeGenerator::set_clump);
	ClassDB::bind_method(D_METHOD("get_clump"), &TreeGenerator::get_clump);
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
	ClassDB::bind_method(D_METHOD("gen", "w", "h", "home", "branch_id", "depth"), &TreeGenerator::gen);
	ClassDB::bind_method(D_METHOD("add_tree_branch", "inf"), &TreeGenerator::add_tree_branch);
	ClassDB::bind_method(D_METHOD("create_tree"), &TreeGenerator::create_tree);

	// --- Variation parameters ---
	ADD_GROUP("Variation", "");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "seed", PROPERTY_HINT_RANGE, "0,9999,1"), "set_seed", "get_seed");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "randomness", PROPERTY_HINT_RANGE, "0.0,1.0,0.01"), "set_randomness", "get_randomness");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "curvature", PROPERTY_HINT_RANGE, "0.0,1.0,0.01"), "set_curvature", "get_curvature");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "gravity", PROPERTY_HINT_RANGE, "0.0,1.0,0.01"), "set_gravity", "get_gravity");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "clump", PROPERTY_HINT_RANGE, "0.0,1.0,0.01"), "set_clump", "get_clump");

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

	_request_update();
}

PackedVector3Array TreeGenerator::get_splits() const {
	return splits;
}

void TreeGenerator::set_rotation(float p_rotation) {
	rotation = p_rotation;
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

void TreeGenerator::set_seed(int p_seed) {
	seed = p_seed;
	_request_update();
}

int TreeGenerator::get_seed() const {
	return seed;
}

void TreeGenerator::set_randomness(float p_randomness) {
	randomness = p_randomness;
	_request_update();
}

float TreeGenerator::get_randomness() const {
	return randomness;
}

void TreeGenerator::set_curvature(float p_curvature) {
	curvature = p_curvature;
	_request_update();
}

float TreeGenerator::get_curvature() const {
	return curvature;
}

void TreeGenerator::set_gravity(float p_gravity) {
	gravity = p_gravity;
	_request_update();
}

float TreeGenerator::get_gravity() const {
	return gravity;
}

void TreeGenerator::set_clump(float p_clump) {
	clump = p_clump;
	_request_update();
}

float TreeGenerator::get_clump() const {
	return clump;
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

// Builds a single branch descriptor at the top of `home` with width `w`,
// height `h`, a deterministic branch ID, and the remaining recursion depth.
// The descriptor contains:
//   - "branches":      child branch descriptors (one per split)
//   - "myTransform":   transform of this branch's own cylinder
//   - "leaftransform": transform used to place leaves/flowers here
//
// Per-branch randomness is derived by hashing (seed ^ branch_id) so that:
//   - Different seeds produce structurally different trees.
//   - The same seed always produces exactly the same tree.
//   - Each child of a branch gets a unique ID via a secondary hash of
//     (branch_id * 37 + child_index), so siblings don't share values.
//
// Parameters applied per child branch:
//   clump     — scales tilt toward 0 (branches grow more upright).
//   curvature — random tilt perturbation that makes branches wander.
//   randomness— random yaw jitter that shuffles azimuthal distribution.
//   gravity   — rotates the tip toward world -Y, increasing with depth.
Dictionary TreeGenerator::gen(float w, float h, Transform3D home, int p_branch_id, int p_depth) const {
	Dictionary branch;
	// Move to the top of the parent branch; children emanate from here.
	Transform3D branch_origin = home.translated_local(Vector3(0.0, h, 0.0));

	// Fraction of total depth consumed so far: 0 at root, 1 at tips.
	// Gravity increases toward the tips so deep branches droop the most.
	float depth_fraction = (branch_depth > 0)
			? (1.0f - float(p_depth) / float(branch_depth))
			: 0.0f;

	branch["branches"] = Array{};
	for (int i = 0; i < splits.size(); i++) {
		// Unique deterministic ID for this child.
		uint32_t child_id = _hash(uint32_t(p_branch_id) * 37u + uint32_t(i));

		float position_along_trunk = float(i) / float(splits.size());
		float bent_position = ((position_along_trunk - 0.5f) * distribution) + 0.5f;

		// --- Base orientation from the splits array ---
		float tilt_angle = float(splits[i].x) * PI / 2.0f;
		float yaw_angle = TAU * (position_along_trunk + rotation);

		// Clumping: pull tilt toward 0 so branches grow more upright/bunched.
		tilt_angle *= MAX(0.0f, 1.0f - clump);

		// Curvature: random tilt perturbation making branches wander off-axis.
		tilt_angle += _rand(child_id + 1u) * PI * 0.5f * curvature;

		// Randomness: random yaw jitter shuffles branches around the azimuth.
		yaw_angle += _rand(child_id + 2u) * TAU * randomness;

		// Build the local orientation: tilt around +Z then yaw around +Y.
		Transform3D orient = IDENTITY
				.rotated(Vector3(0.0f, 0.0f, 1.0f), tilt_angle)
				.rotated(Vector3(0.0f, 1.0f, 0.0f), yaw_angle);

		// Compose with the parent's world transform to get the child's world transform.
		Transform3D child_world = branch_origin * orient;

		// Gravity: tilt the branch toward world -Y in world space.
		// We rotate the world-space basis so the branch's up-axis droops downward,
		// keeping the branch origin fixed. Depth fraction ensures tips droop more.
		if (gravity > 0.0f) {
			Vector3 branch_up = child_world.basis.get_column(1).normalized();
			Vector3 world_down = Vector3(0.0f, -1.0f, 0.0f);
			Vector3 gravity_axis = branch_up.cross(world_down);
			float axis_len = gravity_axis.length();
			if (axis_len > 0.001f) {
				float gravity_amount = gravity * PI * 0.5f * depth_fraction;
				Basis drooped = child_world.basis.rotated(gravity_axis / axis_len, gravity_amount);
				child_world = Transform3D(drooped, child_world.origin);
			}
		}

		// Parent alignment: snap yaw to the nearest radial segment face so the
		// top ring of the parent cylinder aligns cleanly with the child's base.
		float snapped = round((position_along_trunk + rotation) * float(radial_segments)) / float(radial_segments);
		Transform3D parent_align = IDENTITY.rotated(Vector3(0.0f, 1.0f, 0.0f), TAU * snapped);

		Dictionary child;
		child["tran"] = child_world;
		child["parent_align"] = parent_align;
		child["w"] = sqrt((w * w) * radius_decay * (1.0f - bent_position));
		child["h"] = sqrt(h * h * (1.0f - bent_position) * length_decay);
		child["id"] = int(child_id); // Pass the unique ID down to the next level.
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
//   - "b":      the branch descriptor (from gen())
//   - "d":      remaining recursion depth
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
			int child_id = int(child["id"]);
			Dictionary child_branch = gen(child["w"], child["h"], child["tran"], child_id, remaining_depth - 1);

			// Only recurse into branches that are thick enough to be visible.
			if (float(child["w"]) > 0.1f) {
				// Parent alignment stored per-child in gen() aligns the parent's
				// top ring to the child's azimuthal position.
				child_branch["parent"] = branch_transform * Transform3D(Dictionary(child)["parent_align"]);

				Dictionary recursion_input;
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
	// ID 0 for the root; children derive their IDs via _hash(parent_id * 37 + i).
	Dictionary root_branch = gen(width, height, Transform3D(), 0, branch_depth);

	// The root's "parent" is a dummy transform anchoring the base of the trunk.
	root_branch["parent"] = Transform3D().scaled(Vector3(width, height, width)).translated(Vector3(0, -0.5, 0));

	Dictionary input;
	input["b"] = root_branch;
	input["d"] = branch_depth;

	return add_tree_branch(input);
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
