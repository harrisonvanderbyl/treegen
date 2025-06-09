#ifndef FLOWER_GEN_H
#define FLOWER_GEN_H

#include <scene/3d/mesh_instance_3d.h>
#include <scene/resources/3d/primitive_meshes.h>

#define GETSET(c,t,p) void set_##p(c t p); t get_##p() c;
#define GETSETINP(C,c,t,p, u) t C::get_##p() c {return p;}; void C::set_##p(c t p_##p) {p = p_##p; u();};


// Generator that creates a lower from inputed parameters
class FlowerGen : public PrimitiveMesh {
	GDCLASS(FlowerGen, PrimitiveMesh)
private:
	int petals = 5;
	float petal_width = 0.5;
	float petal_height = 0.5;
	Ref<Curve> petal_curveLeft;
	Ref<Curve> petal_curveTop;

	Vector2 petal_angle = Vector2(0, 0);
	int petal_rings = 1;
	float petal_scale = 0.0;
	Vector2 petal_segments = Vector2(3, 3);

protected:
	static void _bind_methods();
	void _create_mesh_array(Array &p_arr) const override;

public:
	void create_flower(Array &p_arr, Array transforms) const;

	GETSET(const, int, petals)
	GETSET(const, float, petal_width)
	GETSET(const, float, petal_height)
	GETSET(const, Ref<Curve>, petal_curveLeft)
	GETSET(const, Ref<Curve>, petal_curveTop)
	GETSET(const, Vector2, petal_angle)
	GETSET(const, int, petal_rings)
	GETSET(const, float, petal_scale)
	GETSET(const, Vector2, petal_segments)

	FlowerGen() {
		petal_curveLeft = Ref<Curve>();
		petal_curveTop = Ref<Curve>();
		petal_curveLeft.instantiate();
		petal_curveTop.instantiate();
		petal_curveTop->add_point(Vector2(0, 0));
		petal_curveLeft->add_point(Vector2(0, 0));
		petal_curveLeft->add_point(Vector2(1.0, 0.0));
		petal_curveLeft->add_point(Vector2(0.5, 0.5));
	};
};

#endif // FLOWER_GEN_H
