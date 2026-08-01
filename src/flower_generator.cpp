#include "flower_generator.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/math.hpp>
#include <godot_cpp/variant/packed_float32_array.hpp>
#include <godot_cpp/variant/packed_int32_array.hpp>
#include <godot_cpp/variant/packed_vector2_array.hpp>
#include <godot_cpp/variant/packed_vector3_array.hpp>

namespace godot {

// Shorthand for PI; the codebase uses 3.1415 throughout for historical reasons.
static constexpr float PI = 3.1415f;

void FlowerGenerator::_bind_methods() {
	// --- Bind getters/setters ---
	ClassDB::bind_method(D_METHOD("set_petals", "petals"), &FlowerGenerator::set_petals);
	ClassDB::bind_method(D_METHOD("get_petals"), &FlowerGenerator::get_petals);
	ClassDB::bind_method(D_METHOD("set_petal_width", "petal_width"), &FlowerGenerator::set_petal_width);
	ClassDB::bind_method(D_METHOD("get_petal_width"), &FlowerGenerator::get_petal_width);
	ClassDB::bind_method(D_METHOD("set_petal_height", "petal_height"), &FlowerGenerator::set_petal_height);
	ClassDB::bind_method(D_METHOD("get_petal_height"), &FlowerGenerator::get_petal_height);
	ClassDB::bind_method(D_METHOD("set_petal_curve_left", "petal_curve_left"), &FlowerGenerator::set_petal_curve_left);
	ClassDB::bind_method(D_METHOD("get_petal_curve_left"), &FlowerGenerator::get_petal_curve_left);
	ClassDB::bind_method(D_METHOD("set_petal_curve_top", "petal_curve_top"), &FlowerGenerator::set_petal_curve_top);
	ClassDB::bind_method(D_METHOD("get_petal_curve_top"), &FlowerGenerator::get_petal_curve_top);
	ClassDB::bind_method(D_METHOD("set_petal_angle", "petal_angle"), &FlowerGenerator::set_petal_angle);
	ClassDB::bind_method(D_METHOD("get_petal_angle"), &FlowerGenerator::get_petal_angle);
	ClassDB::bind_method(D_METHOD("set_petal_rings", "petal_rings"), &FlowerGenerator::set_petal_rings);
	ClassDB::bind_method(D_METHOD("get_petal_rings"), &FlowerGenerator::get_petal_rings);
	ClassDB::bind_method(D_METHOD("set_petal_scale", "petal_scale"), &FlowerGenerator::set_petal_scale);
	ClassDB::bind_method(D_METHOD("get_petal_scale"), &FlowerGenerator::get_petal_scale);
	ClassDB::bind_method(D_METHOD("set_petal_segments", "petal_segments"), &FlowerGenerator::set_petal_segments);
	ClassDB::bind_method(D_METHOD("get_petal_segments"), &FlowerGenerator::get_petal_segments);

	ADD_GROUP("Petal", "petal_");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "petal_count", PROPERTY_HINT_RANGE, "1,100,1"), "set_petals", "get_petals");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "petal_width", PROPERTY_HINT_RANGE, "0.01,10,0.01"), "set_petal_width", "get_petal_width");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "petal_height", PROPERTY_HINT_RANGE, "0.01,10,0.01"), "set_petal_height", "get_petal_height");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "petal_curve_left", PROPERTY_HINT_RESOURCE_TYPE, "Curve"), "set_petal_curve_left", "get_petal_curve_left");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "petal_curve_top", PROPERTY_HINT_RESOURCE_TYPE, "Curve"), "set_petal_curve_top", "get_petal_curve_top");
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "petal_angle", PROPERTY_HINT_NONE, ""), "set_petal_angle", "get_petal_angle");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "petal_rings", PROPERTY_HINT_RANGE, "1,100,1"), "set_petal_rings", "get_petal_rings");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "petal_scale", PROPERTY_HINT_RANGE, "0.01,10,0.01"), "set_petal_scale", "get_petal_scale");
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "petal_segments", PROPERTY_HINT_RANGE, "2,100,1"), "set_petal_segments", "get_petal_segments");
}

FlowerGenerator::FlowerGenerator() {
	// Create default curves so the flower has a sensible shape out of the box.
	petal_curve_left.instantiate();
	petal_curve_top.instantiate();
	petal_curve_top->add_point(Vector2(0, 0));
	petal_curve_left->add_point(Vector2(0, 0));
	petal_curve_left->add_point(Vector2(1.0, 0.0));
	petal_curve_left->add_point(Vector2(0.5, 0.5));
}

// ============================================================================
// Getters / setters
// ============================================================================

void FlowerGenerator::set_petals(const int p_petals) {
	petals = p_petals;
	request_update();
}

int FlowerGenerator::get_petals() const {
	return petals;
}

void FlowerGenerator::set_petal_width(const float p_petal_width) {
	petal_width = p_petal_width;
	request_update();
}

float FlowerGenerator::get_petal_width() const {
	return petal_width;
}

void FlowerGenerator::set_petal_height(const float p_petal_height) {
	petal_height = p_petal_height;
	request_update();
}

float FlowerGenerator::get_petal_height() const {
	return petal_height;
}

void FlowerGenerator::set_petal_curve_left(const Ref<Curve> &p_petal_curve_left) {
	petal_curve_left = p_petal_curve_left;
	request_update();
}

Ref<Curve> FlowerGenerator::get_petal_curve_left() const {
	return petal_curve_left;
}

void FlowerGenerator::set_petal_curve_top(const Ref<Curve> &p_petal_curve_top) {
	petal_curve_top = p_petal_curve_top;
	request_update();
}

Ref<Curve> FlowerGenerator::get_petal_curve_top() const {
	return petal_curve_top;
}

Vector2 FlowerGenerator::get_petal_angle() const {
	return petal_angle;
}

void FlowerGenerator::set_petal_angle(const Vector2 p_petal_angle) {
	petal_angle = p_petal_angle;
	request_update();
}

void FlowerGenerator::set_petal_rings(const int p_petal_rings) {
	petal_rings = p_petal_rings;
	request_update();
}

int FlowerGenerator::get_petal_rings() const {
	return petal_rings;
}

void FlowerGenerator::set_petal_scale(const float p_petal_scale) {
	petal_scale = p_petal_scale;
	request_update();
}

float FlowerGenerator::get_petal_scale() const {
	return petal_scale;
}

void FlowerGenerator::set_petal_segments(const Vector2 p_petal_segments) {
	// Clamp each axis to a minimum of 2 so every petal has at least one quad.
	petal_segments = Vector2(MAX(p_petal_segments.x, 2.0), MAX(p_petal_segments.y, 2.0));
	request_update();
}

Vector2 FlowerGenerator::get_petal_segments() const {
	return petal_segments;
}

// Builds flower geometry into `p_array`. For each leaf transform in
// `p_transforms`, `petals` petals are generated. Each petal is a grid of
// quads whose width and height are shaped by the left/top curves.
void FlowerGenerator::create_flower(Array &p_array, const Array &p_transforms) const {
#define ADD_TANGENT(m_x, m_y, m_z, m_d) \
	tangents.push_back(m_x);            \
	tangents.push_back(m_y);            \
	tangents.push_back(m_z);            \
	tangents.push_back(m_d);

	PackedVector3Array points;
	PackedVector3Array normals;
	PackedFloat32Array tangents;
	PackedVector2Array uvs;
	PackedInt32Array indices;
	int point_index = 0;

	// Step sizes for the petal grid: how far each quad advances in u (length)
	// and v (width).
	float u_step = 1.0f / float(petal_segments.x);
	float v_step = 1.0f / float(petal_segments.y);
	Vector3 grid_scale = Vector3(u_step, 1.0f, v_step);

	for (int leaf_index = 0; leaf_index < p_transforms.size(); leaf_index++) {
		for (int petal_index = 0; petal_index < petals; petal_index++) {
			float petal_fraction = float(petal_index) / float(petals);

			// Ring angle spreads petals around the flower; extra rings add
			// overlap for a fuller look.
			float ring_angle = petal_fraction * (PI * (2.0f + 1.68f * float(petal_rings - 1)));
			// Each successive petal shrinks slightly based on petal_scale.
			float petal_shrink = 1.0f / (1.0f + petal_scale * petal_fraction);
			// Petal tilt interpolates between the two angle components.
			float petal_tilt = Math::lerp_angle(petal_angle.x, petal_angle.y, petal_fraction);

			// Build the petal's local transform: scale to petal dimensions,
			// tilt, rotate around the ring, apply shrink, then place at leaf.
			Transform3D petal_transform = Transform3D();
			petal_transform.scale(Vector3(petal_height, petal_height, petal_width));
			petal_transform.rotate(Vector3(0, 0, 1), petal_tilt);
			petal_transform.rotate(Vector3(0, 1, 0), ring_angle);
			petal_transform.scale(Vector3(petal_shrink, petal_shrink, petal_shrink));
			petal_transform = (Transform3D() * Transform3D(p_transforms[leaf_index])) * petal_transform;

			// Generate the petal as a grid of quads.
			for (float u = 0.0f; u < 1.0f; u += u_step) {
				for (float v = 0.0f; v < 1.0f; v += v_step) {
					Vector2 uv_offset = Vector2(u, v);
					Vector3 position = Vector3(uv_offset.x, 0, uv_offset.y - 0.5f);

					// Sample the shaping curves at the quad's corners and just
					// past them (for normal computation via finite difference).
					float width_at_u = petal_curve_left->sample(u);
					float width_at_u_next = petal_curve_left->sample(u + u_step);
					float height_at_u = petal_curve_top->sample(position.x);
					float height_at_u_next = petal_curve_top->sample(position.x + u_step);
					float height_at_u_eps = petal_curve_top->sample(position.x + u_step * 0.01f);
					float height_at_u_next_eps = petal_curve_top->sample(position.x + u_step * 1.01f);

					Vector3 scale_width = Vector3(1.0, 1.0, width_at_u);
					Vector3 scale_width_next = Vector3(1.0, 1.0, width_at_u_next);

					// Four corners of the quad.
					Vector3 corner_00 = petal_transform.xform(scale_width * (Vector3(0, height_at_u, 0) * grid_scale + position));
					Vector3 corner_01 = petal_transform.xform(scale_width * (Vector3(0, height_at_u, 1) * grid_scale + position));
					Vector3 corner_11 = petal_transform.xform(scale_width_next * (Vector3(1, height_at_u_next, 1) * grid_scale + position));
					Vector3 corner_10 = petal_transform.xform(scale_width_next * (Vector3(1, height_at_u_next, 0) * grid_scale + position));

					// Two points slightly ahead along u, for normal estimation.
					Vector3 corner_00_ahead = petal_transform.xform(scale_width * (Vector3(0.01f, height_at_u_eps, 0) * grid_scale + position));
					Vector3 corner_10_ahead = petal_transform.xform(scale_width_next * (Vector3(1.01f, height_at_u_next_eps, 0) * grid_scale + position));

					points.push_back(corner_00);
					points.push_back(corner_01);
					points.push_back(corner_11);
					points.push_back(corner_10);

					// Normals via cross products using the ahead-points.
					Vector3 normal_front = (corner_00 - corner_01).cross(corner_00 - corner_00_ahead).normalized();
					Vector3 normal_back = (corner_10 - corner_11).cross(corner_10 - corner_10_ahead).normalized();
					normals.push_back(normal_front);
					normals.push_back(normal_front);
					normals.push_back(normal_back);
					normals.push_back(normal_back);

					ADD_TANGENT(0, 0, 1, 0);
					ADD_TANGENT(0, 0, 1, 0);
					ADD_TANGENT(0, 0, 1, 0);
					ADD_TANGENT(0, 0, 1, 0);

					uvs.push_back(uv_offset);
					uvs.push_back(uv_offset + Vector2(0, v_step));
					uvs.push_back(uv_offset + Vector2(u_step, v_step));
					uvs.push_back(uv_offset + Vector2(u_step, 0));

					// Two triangles per quad.
					indices.push_back(point_index);
					indices.push_back(point_index + 2);
					indices.push_back(point_index + 1);
					indices.push_back(point_index);
					indices.push_back(point_index + 3);
					indices.push_back(point_index + 2);

					point_index += 4;
				}
			}
		}
	}

	p_array[Mesh::ARRAY_VERTEX] = points;
	p_array[Mesh::ARRAY_NORMAL] = normals;
	p_array[Mesh::ARRAY_TANGENT] = tangents;
	p_array[Mesh::ARRAY_TEX_UV] = uvs;
	p_array[Mesh::ARRAY_INDEX] = indices;
#undef ADD_TANGENT
}

Array FlowerGenerator::_create_mesh_array() const {
	// PrimitiveMesh interface: generate a standalone flower at the origin.
	Array leaf_transforms;
	leaf_transforms.push_back(Transform3D());
	Array p_array;
	p_array.resize(Mesh::ARRAY_MAX);
	create_flower(p_array, leaf_transforms);
	return p_array;
}

} // namespace godot
