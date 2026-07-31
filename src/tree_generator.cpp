#include "tree_generator.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/packed_float32_array.hpp>
#include <godot_cpp/variant/packed_int32_array.hpp>
#include <godot_cpp/variant/packed_vector2_array.hpp>
#include <godot_cpp/variant/packed_vector3_array.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

namespace godot {

static const Transform3D IDENTITY = Transform3D();

void TreeGenerator::_bind_methods() {
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

	// Branching parameters.
	ADD_GROUP("Branches", "branch_");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "branch_depth", PROPERTY_HINT_RANGE, "1,10,1"), "set_branch_depth", "get_branch_depth");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "branch_distribution", PROPERTY_HINT_RANGE, "0.0,2.0,0.01"), "set_distribution", "get_distribution");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "branch_auto_split", PROPERTY_HINT_RANGE, "0.0,2.0,0.01"), "set_auto_split", "get_auto_split");
	ADD_PROPERTY(PropertyInfo(Variant::PACKED_VECTOR3_ARRAY, "branch_splits", PROPERTY_HINT_NONE, ""), "set_splits", "get_splits");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "branch_rotation", PROPERTY_HINT_RANGE, "0.0,1.0,0.01"), "set_rotation", "get_rotation");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "branch_radius_decay", PROPERTY_HINT_RANGE, "0.0,1.0,0.01"), "set_radius_decay", "get_radius_decay");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "branch_length_decay", PROPERTY_HINT_RANGE, "0.01,2.0,0.01"), "set_length_decay", "get_length_decay");

	// Trunk parameters.
	ADD_GROUP("Trunk", "trunk_");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "trunk_radius", PROPERTY_HINT_RANGE, "0.01,1.0,0.01"), "set_radius", "get_radius");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "trunk_height", PROPERTY_HINT_RANGE, "0.01,10.0,0.01"), "set_height", "get_height");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "trunk_width", PROPERTY_HINT_RANGE, "0.01,10.0,0.01"), "set_width", "get_width");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "trunk_leaf_scale", PROPERTY_HINT_RANGE, "0.0,20.0,0.01"), "set_leaf_scale", "get_leaf_scale");

	// Mesh parameters.
	ADD_GROUP("Mesh", "mesh_");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "mesh_radial_segments", PROPERTY_HINT_RANGE, "3,20,1"), "set_radial_segments", "get_radial_segments");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "mesh_rings", PROPERTY_HINT_RANGE, "0,10,1"), "set_rings", "get_rings");
}

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
	PackedVector3Array temp;
	if (splits.size() < 2) {
		splits.clear();
		splits.push_back(Vector3(0, 0, 0));
		splits.push_back(Vector3(0, 0, 0));
	}
	for (int i = 0; i < splits.size(); i++) {
		float tp = float(i) / float(splits.size());
		float atp = ((tp - 0.5) * float(distribution)) + 0.5;
		temp.append(Vector3(atp * auto_split, 0, 0));
	}
	splits = temp;
	update_parts();
	_request_update();
}

float TreeGenerator::get_auto_split() const {
	return auto_split;
}

void TreeGenerator::set_splits(PackedVector3Array p_splits) {
	PackedVector3Array temp = PackedVector3Array();

	float tot = 0.0;
	for (int i = 0; i < p_splits.size(); i++) {
		tot += p_splits[i].y * p_splits[i].y;
	}
	tot = MAX(sqrt(tot), 0.0001);
	for (int i = 0; i < p_splits.size(); i++) {
		temp.append(Vector3(p_splits[i].x, p_splits[i].y / tot, p_splits[i].z));
	}
	splits = temp;
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
	if (!transforms.is_empty())
		return transforms;
	else {
		Dictionary tr = create_tree();
		Array tree = tr["tree"];
		Array leaf = tr["leaf"];
		transforms = tree;
		return tree;
	}
}

Dictionary TreeGenerator::gen(float w, float h, Transform3D home) const {
	Dictionary thisbranch = Dictionary{};
	Transform3D az = home.translated_local((Vector3(0.0, h, 0.0)));
	thisbranch["branches"] = Array{};
	for (int i = 0; i < splits.size(); i++) {
		Dictionary z = Dictionary{};
		float tp = float(i) / float(splits.size());
		float atp = ((tp - 0.5) * float(distribution)) + 0.5;
		z["tran"] = (az * Transform3D(sid[i]));
		z["w"] = sqrt((w * w) * float(radius_decay) * (1.0 - atp));
		z["h"] = sqrt(h * h * (1.0 - atp) * float(length_decay));
		Array(thisbranch["branches"]).append(z);
	}
	Transform3D t = home.translated_local(Vector3(0.0, h / 2, 0.0)) * IDENTITY.scaled(Vector3(w, h, w));
	thisbranch["myTransform"] = t;

	thisbranch["leaftransform"] = home.translated_local(Vector3(0.0, h / 2, 0.0)) * IDENTITY.scaled(Vector3(leaf_scale, leaf_scale, leaf_scale));
	return thisbranch;
}

Dictionary TreeGenerator::add_tree_branch(Dictionary inf) const {
	Array leaftransforms = Array{};
	Array treetransforms = Array{};
	Array parentTransforms = Array{};
	Transform3D tree = Dictionary(inf["b"])["myTransform"];
	Transform3D leaf = Dictionary(inf["b"])["leaftransform"];
	if (int(inf["d"]) > 0) {
		for (int bi = 0; bi < Array(Dictionary(inf["b"])["branches"]).size(); bi++) {
			Dictionary b = Array(Dictionary(inf["b"])["branches"])[bi];
			Dictionary a = gen(b["w"], b["h"], b["tran"]);
			if (float(b["w"]) > 0.1) {
				Dictionary n = Dictionary{};
				a["parent"] = tree * Transform3D(sid_left[bi]);
				n["b"] = a;
				n["d"] = int(inf["d"]) - 1;

				Dictionary r = add_tree_branch(n);
				Array nt = r["tree"];
				Array nl = r["leaf"];
				Array npt = r["parent"];
				for (int yy = 0; yy < nt.size(); yy++) {
					treetransforms.append(nt[yy]);
					parentTransforms.append(npt[yy]);
				}
				for (int xx = 0; xx < nl.size(); xx++) {
					leaftransforms.append(nl[xx]);
				}
			} else {
				if (bi == 0) {
					leaftransforms.append(leaf);
				}
			}
		}
	} else {
		leaftransforms.append(leaf);
	}

	treetransforms.append(tree);
	Transform3D parent = Dictionary(inf["b"])["parent"];
	parentTransforms.append(parent);
	Dictionary ret = Dictionary{};
	ret["tree"] = treetransforms;
	ret["leaf"] = leaftransforms;
	ret["parent"] = parentTransforms;
	return ret;
}

Dictionary TreeGenerator::create_tree() const {
	Dictionary b = gen(width, height, Transform3D());
	Dictionary z = Dictionary{};
	b["parent"] = Transform3D().scaled(Vector3(width, height, width)).translated(Vector3(0, -0.5, 0));
	z["b"] = b;
	z["d"] = branch_depth;

	Dictionary t = add_tree_branch(z);
	return t;
}

void TreeGenerator::update_parts() {
	sid = Array();
	sid_left = Array();
	for (int i = 0; i < splits.size(); i++) {
		float tp = float(i) / float(splits.size());
		sid.append(
				IDENTITY.rotated(Vector3(0.0, 0.0, 1.0), (float(splits[i].x) * 3.1415 / 2)).rotated(Vector3(0.0, 1.0, 0.0), 2.0 * ((tp + rotation) * 3.1415)));
		sid_left.append(
				IDENTITY.rotated(Vector3(0, 1, 0), (2.0 * (round((float(i) / float(splits.size()) + rotation) * float(radial_segments)) / float(radial_segments)) * 3.1415)));
	}
}

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
	int i, j, prevrow, thisrow, point;
	float x, y, z, u, v, currradius;

	PackedVector3Array points;
	PackedVector3Array normals;
	PackedFloat32Array tangents;
	PackedVector2Array uvs;
	PackedInt32Array indices;

	point = 0;
	// Generate a Cylinder for the trunk for each transform in the tree array
	thisrow = 0;
	prevrow = 0;
	for (int cr = 0; cr < tree.size(); cr++) {
		Transform3D t = tree[cr];
		Transform3D n = parent[cr];
		Transform3D par = n * IDENTITY.translated_local(Vector3(0.0, 0.5, 0.0));

		for (j = 0; j <= (rings + 1); j++) {
			v = j;
			v /= (rings + 1);

			currradius = radius;
			y = 1.0 * v;
			y = (0.1) - y * 0.5;

			for (i = 0; i <= radial_segments; i++) {
				u = i;
				u /= radial_segments;

				x = sin(u * (3.1415 * 2.0));
				z = cos(u * (3.1415 * 2.0));

				Vector3 pointa = Vector3(x * currradius, y, z * currradius);
				Transform3D cur = t.interpolate_with(par, v);
				Vector3 p = ((cur)*Transform3D(Basis(), pointa)).origin;
				Vector3 c = ((cur)*Transform3D(Basis(), Vector3(0, y, 0))).origin;
				Vector3 nn = (p - (c)).normalized();
				Vector3 tangent = cur.basis[1].normalized().cross(nn).normalized();
				points.push_back(p);
				normals.push_back(nn);
				ADD_TANGENT(tangent.x, tangent.y, tangent.z, 1.0)
				uvs.push_back(Vector2(u, v));
				point++;

				if (i > 0 && j > 0) {
					indices.push_back(prevrow + i - 1);
					indices.push_back(prevrow + i);
					indices.push_back(thisrow + i - 1);

					indices.push_back(prevrow + i);
					indices.push_back(thisrow + i);
					indices.push_back(thisrow + i - 1);
				};
			};

			prevrow = thisrow;
			thisrow = point;
		};

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
