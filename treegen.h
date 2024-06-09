#ifndef TREE_GEN_H
#define TREE_GEN_H

#include "duomesh.h"
#include <scene/3d/mesh_instance_3d.h>
#include <scene/resources/3d/primitive_meshes.h>
// Generator based on a script, like GDScript, C# or NativeScript.
// The script is expected to properly handle multithreading.
class TreeGen : public DuoMesh {
	GDCLASS(TreeGen, DuoMesh)
private:
	int sections = 3;
	float distribution = 0.5;
	float auto_split = 0.5;
	PackedVector3Array splits = PackedVector3Array();
	Array sid = Array();
	Array sidL = Array();
	float rotation = 0;
	float radius_scale = 1.0;
	float radius_mult = 0.5;
	float radius = 1.0;
	float height = 2.0;
	float width = 3.0;
	int radial_segments = 10;
	int rings = 3;
	float leafScale = 2.0;

	Array transforms;

protected:
	static void _bind_methods();
	void _create_mesh_array(Array &p_arr, Dictionary tr) const;

public:
	void set_sections(const int p_sections);
	int get_sections() const;
	void set_distribution(const float p_distribution);
	float get_distribution() const;
	void set_auto_split(const float p_auto_split);
	float get_auto_split() const;
	void set_splits(const PackedVector3Array p_splitsB);
	PackedVector3Array get_splits() const;
	void set_rotation(const float p_rotation);
	float get_rotation() const;
	void set_radius_scale(const float p_radius_scale);
	float get_radius_scale() const;
	void set_radius_mult(const float p_radius_mult);
	float get_radius_mult() const;

	void set_radius(const float p_radius);
	float get_radius() const;

	void set_height(const float p_height);
	float get_height() const;

	void set_width(const float p_width);
	float get_width() const;

	void set_radial_segments(const int p_segments);
	int get_radial_segments() const;

	void set_rings(const int p_rings);
	int get_rings() const;

	void set_leafScale(const float p_leafScale);
	float get_leafScale() const;

	void set_transforms(const Array p_transforms);
	Array get_transforms();

	void updateParts();
	Dictionary gen(float w, float h, Transform3D home) const;
	Dictionary addtreebranch(Dictionary inf) const;
	Dictionary createTree() const;

	TreeGen(){};
};

#endif // TREE_GEN_H
