#ifndef FLOWER_GEN_H
#define FLOWER_GEN_H

#include <scene/3d/mesh_instance_3d.h>
#include <scene/resources/3d/primitive_meshes.h>
// Generator that creates a lower from inputed parameters
class FlowerGen : public PrimitiveMesh {
	GDCLASS(FlowerGen, PrimitiveMesh)
private:
	int petals = 5;
	float petal_width = 0.5;
	float petal_height = 0.5;
	Ref<Curve> petal_curveLeft = Ref<Curve>();
	Ref<Curve> petal_curveTop = Ref<Curve>();

	Vector2 petal_angle = Vector2(0, 0);
	int petal_rings = 1;
	float petal_scale = 0.0;
	Vector2 petal_segments = Vector2(3, 3);

protected:
	static void _bind_methods();
	void _create_mesh_array(Array &p_arr) const override;

public:
	void create_flower(Array &p_arr, Array transforms) const;
	void set_petals(const int p_petals);
	int get_petals() const;
	void set_petal_width(const float p_petal_width);
	float get_petal_width() const;
	void set_petal_height(const float p_petal_height);
	float get_petal_height() const;
	void set_petal_curveLeft(const Ref<Curve> &p_petal_curveLeft);
	Ref<Curve> get_petal_curveLeft() const;
	void set_petal_curveTop(const Ref<Curve> &p_petal_curveTop);
	Ref<Curve> get_petal_curveTop() const;
	void set_petal_angle(const Vector2 p_petal_angle);
	Vector2 get_petal_angle() const;
	void set_petal_rings(const int p_petal_rings);
	int get_petal_rings() const;
	void set_petal_scale(const float p_petal_scale);
	float get_petal_scale() const;
	void set_petal_segments(const Vector2 p_petal_segments);
	Vector2 get_petal_segments() const;

	FlowerGen() {
		petal_curveLeft.instantiate();
		petal_curveTop.instantiate();
		petal_curveTop->add_point(Vector2(0, 0));
		petal_curveLeft->add_point(Vector2(0, 0));
		petal_curveLeft->add_point(Vector2(1.0, 0.0));
		petal_curveLeft->add_point(Vector2(0.5, 0.5));
	};
};

#endif // FLOWER_GEN_H
