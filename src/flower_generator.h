#pragma once

#include <godot_cpp/classes/curve.hpp>
#include <godot_cpp/classes/primitive_mesh.hpp>
#include <godot_cpp/variant/vector2.hpp>

namespace godot {

/// Procedural flower / petal mesh generator.
/// Produces a petal arrangement driven by curve resources and per-petal parameters.
class FlowerGenerator : public PrimitiveMesh {
	GDCLASS(FlowerGenerator, PrimitiveMesh)

private:
	int petals = 5;
	float petal_width = 0.5;
	float petal_height = 0.5;
	Ref<Curve> petal_curve_left;
	Ref<Curve> petal_curve_top;

	Vector2 petal_angle = Vector2(0, 0);
	int petal_rings = 1;
	float petal_scale = 0.0;
	Vector2 petal_segments = Vector2(3, 3);

protected:
	static void _bind_methods();

public:
	Array _create_mesh_array() const override;
	void create_flower(Array &p_array, const Array &p_transforms) const;

	void set_petals(int p_petals);
	int get_petals() const;
	void set_petal_width(float p_petal_width);
	float get_petal_width() const;
	void set_petal_height(float p_petal_height);
	float get_petal_height() const;
	void set_petal_curve_left(const Ref<Curve> &p_petal_curve_left);
	Ref<Curve> get_petal_curve_left() const;
	void set_petal_curve_top(const Ref<Curve> &p_petal_curve_top);
	Ref<Curve> get_petal_curve_top() const;
	void set_petal_angle(const Vector2 p_petal_angle);
	Vector2 get_petal_angle() const;
	void set_petal_rings(int p_petal_rings);
	int get_petal_rings() const;
	void set_petal_scale(float p_petal_scale);
	float get_petal_scale() const;
	void set_petal_segments(const Vector2 p_petal_segments);
	Vector2 get_petal_segments() const;

	FlowerGenerator();
};

} // namespace godot
