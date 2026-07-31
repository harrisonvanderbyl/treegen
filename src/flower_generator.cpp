#include "flower_generator.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/math.hpp>
#include <godot_cpp/variant/packed_float32_array.hpp>
#include <godot_cpp/variant/packed_int32_array.hpp>
#include <godot_cpp/variant/packed_vector2_array.hpp>
#include <godot_cpp/variant/packed_vector3_array.hpp>

namespace godot {

void FlowerGenerator::_bind_methods() {
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
	petal_curve_left.instantiate();
	petal_curve_top.instantiate();
	petal_curve_top->add_point(Vector2(0, 0));
	petal_curve_left->add_point(Vector2(0, 0));
	petal_curve_left->add_point(Vector2(1.0, 0.0));
	petal_curve_left->add_point(Vector2(0.5, 0.5));
}

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
	petal_segments = Vector2(MAX(p_petal_segments.x, 2.0), MAX(p_petal_segments.y, 2.0));
	request_update();
}

Vector2 FlowerGenerator::get_petal_segments() const {
	return petal_segments;
}

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
	for (int leaft = 0; leaft < p_transforms.size(); leaft++) {
		for (int i = 0; i < petals; i++) {
			float ring_angle = ((float)i / float(petals)) * (3.1415 * (2.0 + 1.68 * float(petal_rings - 1)));
			float scale = 1.0 / (1.0 + petal_scale * float(i) / float(petals));
			float petalangle = Math::lerp_angle(petal_angle.x, petal_angle.y, float(i) / float(petals));
			Transform3D petalTransform = Transform3D();
			petalTransform.scale(Vector3(petal_height, petal_height, petal_width));

			petalTransform.rotate(Vector3(0, 0, 1), petalangle);
			petalTransform.rotate(Vector3(0, 1, 0), ring_angle);
			petalTransform.scale(Vector3(scale, scale, scale));
			petalTransform = (Transform3D() * Transform3D(p_transforms[leaft])) * petalTransform;
			float ppetal = 1.0 / (float)petal_segments.x;
			float ppetalw = 1.0 / (float)petal_segments.y;
			Vector3 scales = Vector3(ppetal, 1.0, ppetalw);
			for (float k = 0.0; k < 1.0; k += ppetal) {
				for (float l = 0.0; l < 1.0; l += ppetalw) {
					Vector2 offset2 = Vector2(k, l);

					Vector3 offset = Vector3(offset2.x, 0, offset2.y - 0.5);

					float widtha = petal_curve_left->sample((float)k);
					float heighta = petal_curve_top->sample(offset.x);
					float heightab = petal_curve_top->sample(offset.x + ppetal * 0.01);
					float widthb = petal_curve_left->sample(((float)k + ppetal));
					float heightb = petal_curve_top->sample((offset.x + ppetal));
					float heightbb = petal_curve_top->sample((offset.x + ppetal * 1.01));
					Vector3 scalews = Vector3(1.0, 1.0, widtha);
					Vector3 scalewb = Vector3(1.0, 1.0, widthb);
					Vector3 p1 = petalTransform.xform(scalews * (Vector3(0, heighta, 0) * scales + offset));
					Vector3 p2 = petalTransform.xform(scalews * (Vector3(0, heighta, 1) * scales + offset));
					Vector3 p3 = petalTransform.xform(scalewb * (Vector3(1, heightb, 1) * scales + offset));
					Vector3 p4 = petalTransform.xform(scalewb * (Vector3(1, heightb, 0) * scales + offset));
					Vector3 p1b = petalTransform.xform(scalews * (Vector3(0.01, heightab, 0) * scales + offset));
					Vector3 p4b = petalTransform.xform(scalewb * (Vector3(1.01, heightbb, 0) * scales + offset));
					points.push_back(p1);
					points.push_back(p2);
					points.push_back(p3);
					points.push_back(p4);
					Vector3 normal = (p1 - p2).cross(p1 - p1b).normalized();
					Vector3 normal2 = (p4 - p3).cross(p4 - p4b).normalized();
					normals.push_back(normal);
					normals.push_back(normal);
					normals.push_back(normal2);
					normals.push_back(normal2);

					ADD_TANGENT(0, 0, 1, 0);
					ADD_TANGENT(0, 0, 1, 0);
					ADD_TANGENT(0, 0, 1, 0);
					ADD_TANGENT(0, 0, 1, 0);

					uvs.push_back(offset2);
					uvs.push_back(offset2 + Vector2(0, ppetalw));
					uvs.push_back(offset2 + Vector2(ppetal, ppetalw));
					uvs.push_back(offset2 + Vector2(ppetal, 0));

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
	Array arr = Array();
	arr.push_back(Transform3D());
	Array p_array;
	p_array.resize(Mesh::ARRAY_MAX);
	create_flower(p_array, arr);
	return p_array;
}

} // namespace godot
